/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once

#include <SofaBaseMechanics/DiagonalMass.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/helper/io/XspLoader.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <SofaBaseTopology/TopologyData.inl>
#include <SofaBaseTopology/RegularGridTopology.h>
#include <SofaBaseMechanics/AddMToMatrixFunctor.h>
#include <sofa/simulation/AnimateEndEvent.h>

#include <sofa/defaulttype/typeinfo/DataTypeInfo[VecTypes].h>
#include <sofa/defaulttype/typeinfo/DataTypeInfo[RigidTypes].h>

namespace sofa::component::mass
{

using sofa::core::objectmodel::ComponentState;
using namespace sofa::core::topology;

template <class DataTypes, class MassType>
DiagonalMass<DataTypes, MassType>::DiagonalMass()
    : d_vertexMass( initData(&d_vertexMass, "vertexMass", "Specify a vector giving the mass of each vertex. \n"
                                                          "If unspecified or wrongly set, the massDensity or totalMass information is used.") )
    , d_massDensity( initData(&d_massDensity, Real(1.0),"massDensity","Specify one single real and positive value for the mass density. \n"
                                                                      "If unspecified or wrongly set, the totalMass information is used.") )
    , d_totalMass(initData(&d_totalMass, Real(1.0), "totalMass", "Specify the total mass resulting from all particles. \n"
                                                                 "If unspecified or wrongly set, the default value is used: totalMass = 1.0"))
    , d_computeMassOnRest(initData(&d_computeMassOnRest, true, "computeMassOnRest", "If true, the mass of every element is computed based on the rest position rather than the position"))
    , d_showCenterOfGravity( initData(&d_showCenterOfGravity, false, "showGravityCenter", "Display the center of gravity of the system" ) )
    , d_showAxisSize( initData(&d_showAxisSize, 1.0f, "showAxisSizeFactor", "Factor length of the axis displayed (only used for rigids)" ) )
    , d_fileMass( initData(&d_fileMass,  "filename", "Xsp3.0 file to specify the mass parameters" ) )
    , m_pointHandler(nullptr)
    , l_topology(initLink("topology", "link to the topology container"))
    , m_topologyType(TOPOLOGY_UNKNOWN)
    , m_topology(nullptr)
{
    this->addAlias(&d_fileMass,"fileMass");
}

template <class DataTypes, class MassType>
DiagonalMass<DataTypes, MassType>::~DiagonalMass()
{
    if (m_pointHandler)
        delete m_pointHandler;
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::applyCreateFunction(PointID, MassType &m, const Point &, const sofa::helper::vector<PointID> &, const sofa::helper::vector<double> &)
{
    m=0;
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::applyPointDestruction(const sofa::helper::vector<PointID> & pointsRemoved)
{
    helper::WriteAccessor<Data<Masvector> > masses(dm->d_vertexMass);
    helper::WriteAccessor<Data<Real> > totalMass(dm->d_totalMass);

    size_t numberPointsRemoved = pointsRemoved.size();
    size_t newSize = masses.size()-numberPointsRemoved;
    size_t counter = 0;

    // Remove the mass of the removed points from totalMass
    for(size_t i=0; i<numberPointsRemoved; i++)
    {
        totalMass -= masses[pointsRemoved[i]];
    }

    // Resize the vertexMass vector and remove removed indices
    bool removedPointFound;
    for(size_t i=0; i<newSize; i++)
    {
        removedPointFound = false;
        for(size_t j=0; j<numberPointsRemoved; j++)
        {
            if(i == pointsRemoved[j])
                removedPointFound = true;
        }
        if(removedPointFound)
            counter++;
        else
            masses[i-counter] = masses[i];
    }

    masses.resize(newSize);
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::ApplyTopologyChange(const core::topology::PointsRemoved* e)
{
    if(!dm->d_computeMassOnRest.getValue())
        msg_warning("DiagonalMassPointHandler") << "ApplyTopologyChange: option computeMassOnRest should be true to have consistent topological change";

    const auto& pointsRemoved = e->getArray();
    applyPointDestruction(pointsRemoved);
    dm->cleanTracker();
    dm->printMass();

    if(dm->f_printLog.getValue())
    {
        msg_info("DiagonalMassPointHandler")<<"ApplyTopologyChange: EdgesRemoved";
        msg_info("DiagonalMassPointHandler")<<"Size of vertexMass: "<< dm->d_vertexMass.getValue().size();
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::applyEdgeCreation(const sofa::helper::vector< EdgeID >& edgeAdded,
        const sofa::helper::vector< Edge >& /*elems*/,
        const sofa::helper::vector< sofa::helper::vector< EdgeID > >& /*ancestors*/,
        const sofa::helper::vector< sofa::helper::vector< double > >& /*coefs*/)
{
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_EDGESET)
    {
        helper::WriteAccessor<Data<Masvector> > masses(dm->d_vertexMass);
        helper::WriteAccessor<Data<Real> > totalMass(dm->d_totalMass);

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=typename DataTypes::Real(0);
        unsigned int i;

        for (i=0; i<edgeAdded.size(); ++i)
        {
            /// get the edge to be added
            const Edge &e=dm->m_topology->getEdge(edgeAdded[i]);
            // compute its mass based on the mass density and the edge length
            if(dm->edgeGeo)
            {
                mass=(md*dm->edgeGeo->computeRestEdgeLength(edgeAdded[i]))/(typename DataTypes::Real(2.0));
            }
            // added mass on its two vertices
            masses[e[0]]+=mass;
            masses[e[1]]+=mass;

            totalMass += 2.0*mass;
        }
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::applyEdgeDestruction(const sofa::helper::vector<EdgeID> & edgeRemoved)
{
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_EDGESET)
    {
        helper::WriteAccessor<Data<Masvector> > masses(dm->d_vertexMass);
        helper::WriteAccessor<Data<Real> > totalMass(dm->d_totalMass);

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=typename DataTypes::Real(0);
        unsigned int i;

        for (i=0; i<edgeRemoved.size(); ++i)
        {
            /// get the edge to be added
            const Edge &e=dm->m_topology->getEdge(edgeRemoved[i]);
            // compute its mass based on the mass density and the edge length
            if(dm->edgeGeo)
            {
                mass=(md*dm->edgeGeo->computeRestEdgeLength(edgeRemoved[i]))/(typename DataTypes::Real (2.0));
            }
            // removed mass on its two vertices
            masses[e[0]]-=mass;
            masses[e[1]]-=mass;

            totalMass -= 2.0*mass;
        }
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::ApplyTopologyChange(const core::topology::EdgesAdded* e)
{
    const auto& edgeIndex = e->getIndexArray();
    const sofa::helper::vector< Edge >& edges = e->getArray();
    const auto& ancestors = e->ancestorsList;
    const sofa::helper::vector< sofa::helper::vector< double > >& coeffs = e->coefs;

    if(dm->edgeGeo)
    {
        if(!dm->d_computeMassOnRest.getValue())
            msg_warning("DiagonalMassPointHandler") << "ApplyTopologyChange: option computeMassOnRest should be true to have consistent topological change";

        if(dm->f_printLog.getValue())
            msg_info("DiagonalMassPointHandler")<<"ApplyTopologyChange: EdgesAdded";

        applyEdgeCreation(edgeIndex, edges, ancestors, coeffs);
        dm->cleanTracker();
        dm->printMass();
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::ApplyTopologyChange(const core::topology::EdgesRemoved* e)
{
    const auto& edgeRemoved = e->getArray();

    if(dm->edgeGeo)
    {
        if(!dm->d_computeMassOnRest.getValue())
            msg_warning("DiagonalMassPointHandler") << "ApplyTopologyChange: option computeMassOnRest should be true to have consistent topological change";

        if(dm->f_printLog.getValue())
            msg_info("DiagonalMassPointHandler")<<"ApplyTopologyChange: EdgesRemoved";

        applyEdgeDestruction(edgeRemoved);
        dm->cleanTracker();
        dm->printMass();
    }
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::applyTriangleCreation(const sofa::helper::vector< TriangleID >& triangleAdded,
        const sofa::helper::vector< Triangle >& /*elems*/,
        const sofa::helper::vector< sofa::helper::vector< TriangleID > >& /*ancestors*/,
        const sofa::helper::vector< sofa::helper::vector< double > >& /*coefs*/)
{
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_TRIANGLESET)
    {
        helper::WriteAccessor<Data<Masvector> > masses(dm->d_vertexMass);
        helper::WriteAccessor<Data<Real> > totalMass(dm->d_totalMass);

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=typename DataTypes::Real(0);
        unsigned int i;

        for (i=0; i<triangleAdded.size(); ++i)
        {
            /// get the triangle to be added
            const Triangle &t=dm->m_topology->getTriangle(triangleAdded[i]);
            // compute its mass based on the mass density and the triangle area
            if(dm->triangleGeo)
            {
                mass=(md*dm->triangleGeo->computeRestTriangleArea(triangleAdded[i]))/(typename DataTypes::Real(3.0));
            }
            // added mass on its three vertices
            masses[t[0]]+=mass;
            masses[t[1]]+=mass;
            masses[t[2]]+=mass;

            totalMass+= 3.0*mass;
        }
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::applyTriangleDestruction(const sofa::helper::vector<TriangleID > & triangleRemoved)
{
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_TRIANGLESET)
    {
        helper::WriteAccessor<Data<Masvector> > masses(dm->d_vertexMass);
        helper::WriteAccessor<Data<Real> > totalMass(dm->d_totalMass);

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=typename DataTypes::Real(0);
        unsigned int i;

        for (i=0; i<triangleRemoved.size(); ++i)
        {
            /// get the triangle to be added
            const Triangle &t=dm->m_topology->getTriangle(triangleRemoved[i]);

            /// compute its mass based on the mass density and the triangle area
            if(dm->triangleGeo)
            {
                mass=(md*dm->triangleGeo->computeRestTriangleArea(triangleRemoved[i]))/(typename DataTypes::Real(3.0));
            }

            /// removed  mass on its three vertices
            masses[t[0]]-=mass;
            masses[t[1]]-=mass;
            masses[t[2]]-=mass;

            totalMass -= 3.0 * mass;
        }
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::ApplyTopologyChange(const core::topology::TrianglesAdded* e)
{
    const auto& triangleAdded = e->getIndexArray();
    const sofa::helper::vector< Triangle >& elems = e->getElementArray();
    const auto& ancestors = e->ancestorsList;
    const sofa::helper::vector< sofa::helper::vector< double > >& coefs = e->coefs;

    if(dm->triangleGeo)
    {
        if(!dm->d_computeMassOnRest.getValue())
            msg_warning("DiagonalMassPointHandler") << "ApplyTopologyChange: option computeMassOnRest should be true to have consistent topological change";

        if(dm->f_printLog.getValue())
            msg_info("DiagonalMassPointHandler")<<"ApplyTopologyChange: TrianglesAdded";

        applyTriangleCreation(triangleAdded,elems,ancestors,coefs);
        dm->cleanTracker();
        dm->printMass();
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::ApplyTopologyChange(const core::topology::TrianglesRemoved* e)
{
    const auto& triangleRemoved = e->getArray();

    if(dm->triangleGeo)
    {
        if(!dm->d_computeMassOnRest.getValue())
            msg_warning("DiagonalMassPointHandler") << "ApplyTopologyChange: option computeMassOnRest should be true to have consistent topological change";

        if(dm->f_printLog.getValue())
            msg_info("DiagonalMassPointHandler")<<"ApplyTopologyChange: TrianglesRemoved";

        applyTriangleDestruction(triangleRemoved);
        dm->cleanTracker();
        dm->printMass();
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::applyTetrahedronCreation(const sofa::helper::vector< TetrahedronID >& tetrahedronAdded,
        const sofa::helper::vector< Tetrahedron >& /*elems*/,
        const sofa::helper::vector< sofa::helper::vector< TetrahedronID > >& /*ancestors*/,
        const sofa::helper::vector< sofa::helper::vector< double > >& /*coefs*/)
{
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_TETRAHEDRONSET)
    {
        helper::WriteAccessor<Data<Masvector> > masses(dm->d_vertexMass);
        helper::WriteAccessor<Data<Real> > totalMass(dm->d_totalMass);

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=typename DataTypes::Real(0);
        unsigned int i;

        for (i=0; i<tetrahedronAdded.size(); ++i)
        {
            /// get the tetrahedron to be added
            const Tetrahedron &t=dm->m_topology->getTetrahedron(tetrahedronAdded[i]);

            /// compute its mass based on the mass density and the tetrahedron volume
            if(dm->tetraGeo)
            {
                mass=(md*dm->tetraGeo->computeRestTetrahedronVolume(tetrahedronAdded[i]))/(typename DataTypes::Real(4.0));
            }

            /// added  mass on its four vertices
            masses[t[0]]+=mass;
            masses[t[1]]+=mass;
            masses[t[2]]+=mass;
            masses[t[3]]+=mass;

            totalMass += 4.0*mass;
        }

    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::applyTetrahedronDestruction(const sofa::helper::vector<TetrahedronID> & tetrahedronRemoved)
{
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_TETRAHEDRONSET)
    {
        helper::WriteAccessor<Data<Masvector> > masses(dm->d_vertexMass);
        helper::WriteAccessor<Data<Real> > totalMass(dm->d_totalMass);

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=typename DataTypes::Real(0);
        unsigned int i;

        for (i=0; i<tetrahedronRemoved.size(); ++i)
        {
            /// get the tetrahedron to be added
            const Tetrahedron &t=dm->m_topology->getTetrahedron(tetrahedronRemoved[i]);
            if(dm->tetraGeo)
            {
                // compute its mass based on the mass density and the tetrahedron volume
                mass=(md*dm->tetraGeo->computeRestTetrahedronVolume(tetrahedronRemoved[i]))/(typename DataTypes::Real(4.0));
            }
            // removed  mass on its four vertices
            masses[t[0]]-=mass;
            masses[t[1]]-=mass;
            masses[t[2]]-=mass;
            masses[t[3]]-=mass;

            totalMass -= 4.0*mass;
        }

    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::ApplyTopologyChange(const core::topology::TetrahedraAdded* e)
{
    const auto& tetrahedronAdded = e->getIndexArray();
    const sofa::helper::vector< Tetrahedron >& elems = e->getElementArray();
    const auto& ancestors = e->ancestorsList;
    const sofa::helper::vector< sofa::helper::vector< double > >& coefs = e->coefs;

    if(dm->tetraGeo)
    {
        if(!dm->d_computeMassOnRest.getValue())
            msg_warning("DiagonalMassPointHandler") << "ApplyTopologyChange: option computeMassOnRest should be true to have consistent topological change";

        if(dm->f_printLog.getValue())
            msg_info("DiagonalMassPointHandler")<<"ApplyTopologyChange: TetrahedraAdded";

        applyTetrahedronCreation(tetrahedronAdded, elems, ancestors, coefs);
        dm->cleanTracker();
        dm->printMass();
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::ApplyTopologyChange(const core::topology::TetrahedraRemoved* e)
{
    const auto& tetrahedronRemoved = e->getArray();

    if(dm->tetraGeo)
    {
        if(!dm->d_computeMassOnRest.getValue())
            msg_warning("DiagonalMassPointHandler") << "ApplyTopologyChange: option computeMassOnRest should be true to have consistent topological change";

        if(dm->f_printLog.getValue())
            msg_info("DiagonalMassPointHandler")<<"ApplyTopologyChange: TetrahedraRemoved";

        applyTetrahedronDestruction(tetrahedronRemoved);
        dm->cleanTracker();
        dm->printMass();
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::applyHexahedronCreation(const sofa::helper::vector< HexahedronID >& hexahedronAdded,
        const sofa::helper::vector< Hexahedron >& /*elems*/,
        const sofa::helper::vector< sofa::helper::vector< HexahedronID > >& /*ancestors*/,
        const sofa::helper::vector< sofa::helper::vector< double > >& /*coefs*/)
{
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_HEXAHEDRONSET)
    {
        helper::WriteAccessor<Data<Masvector> > masses(dm->d_vertexMass);
        helper::WriteAccessor<Data<Real> > totalMass(dm->d_totalMass);

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=typename DataTypes::Real(0);
        unsigned int i;

        for (i=0; i<hexahedronAdded.size(); ++i)
        {
            /// get the tetrahedron to be added
            const Hexahedron &t=dm->m_topology->getHexahedron(hexahedronAdded[i]);
            // compute its mass based on the mass density and the tetrahedron volume
            if(dm->hexaGeo)
            {
                mass=(md*dm->hexaGeo->computeRestHexahedronVolume(hexahedronAdded[i]))/(typename DataTypes::Real(8.0));
            }
            // added  mass on its four vertices
            for (unsigned int j=0; j<8; ++j)
                masses[t[j]]+=mass;

            totalMass += 8.0*mass;
        }

    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::applyHexahedronDestruction(const sofa::helper::vector<HexahedronID> & hexahedronRemoved)
{
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_HEXAHEDRONSET)
    {
        helper::WriteAccessor<Data<Masvector> > masses(dm->d_vertexMass);
        helper::WriteAccessor<Data<Real> > totalMass(dm->d_totalMass);

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=(typename DataTypes::Real) 0;
        unsigned int i;

        for (i=0; i<hexahedronRemoved.size(); ++i)
        {
            /// get the tetrahedron to be added
            const Hexahedron &t=dm->m_topology->getHexahedron(hexahedronRemoved[i]);
            if(dm->hexaGeo)
            {
                // compute its mass based on the mass density and the tetrahedron volume
                mass=(md*dm->hexaGeo->computeRestHexahedronVolume(hexahedronRemoved[i]))/(typename DataTypes::Real(8.0));
            }
            // removed  mass on its four vertices
            for (unsigned int j=0; j<8; ++j)
                masses[t[j]]-=mass;

            totalMass -= 8.0*mass;
        }

    }
}
template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::ApplyTopologyChange(const core::topology::HexahedraAdded* e)
{
    const auto& hexahedronAdded = e->getIndexArray();
    const sofa::helper::vector< Hexahedron >& elems = e->getElementArray();
    const auto& ancestors = e->ancestorsList;
    const sofa::helper::vector< sofa::helper::vector< double > >& coefs = e->coefs;

    if(dm->hexaGeo)
    {
        if(!dm->d_computeMassOnRest.getValue())
            msg_warning("DiagonalMassPointHandler") << "ApplyTopologyChange: option computeMassOnRest should be true to have consistent topological change";

        if(dm->f_printLog.getValue())
            msg_info("DiagonalMassPointHandler")<<"ApplyTopologyChange: HexahedraAdded";

        applyHexahedronCreation(hexahedronAdded,elems,ancestors,coefs);
        dm->cleanTracker();
        dm->printMass();
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes,MassType>::DMassPointHandler::ApplyTopologyChange(const core::topology::HexahedraRemoved* e)
{
    const auto& hexahedronRemoved = e->getArray();

    if(dm->hexaGeo)
    {
        if(!dm->d_computeMassOnRest.getValue())
            msg_warning("DiagonalMassPointHandler") << "ApplyTopologyChange: option computeMassOnRest should be true to have consistent topological change";

        if(dm->f_printLog.getValue())
            msg_info("DiagonalMassPointHandler")<<"ApplyTopologyChange: HexahedraRemoved";

        applyHexahedronDestruction(hexahedronRemoved);
        dm->cleanTracker();
        dm->printMass();
    }
}



template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::clear()
{
    Masvector& masses = *d_vertexMass.beginEdit();
    masses.clear();
    d_vertexMass.endEdit();
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addMass(const MassType& m)
{
    Masvector& masses = *d_vertexMass.beginEdit();
    masses.push_back(m);
    d_vertexMass.endEdit();
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::resize(int vsize)
{
    Masvector& masses = *d_vertexMass.beginEdit();
    masses.resize(vsize);
    d_vertexMass.endEdit();
}

// -- Mass interface
template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addMDx(const core::MechanicalParams* /*mparams*/, DataVecDeriv& res, const DataVecDeriv& dx, SReal factor)
{
    const Masvector &masses= d_vertexMass.getValue();
    helper::WriteAccessor< DataVecDeriv > _res = res;
    helper::ReadAccessor< DataVecDeriv > _dx = dx;

    size_t n = masses.size();
    if (_dx.size() < n) n = _dx.size();
    if (_res.size() < n) n = _res.size();
    if (factor == 1.0)
    {
        for (size_t i=0; i<n; i++)
        {
            _res[i] += _dx[i] * masses[i];
        }
    }
    else
    {
        for (size_t i=0; i<n; i++)
        {
            _res[i] += (_dx[i] * masses[i]) * Real(factor);
        }
    }
}



template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::accFromF(const core::MechanicalParams* /*mparams*/, DataVecDeriv& a, const DataVecDeriv& f)
{

    const Masvector &masses= d_vertexMass.getValue();
    helper::WriteOnlyAccessor< DataVecDeriv > _a = a;
    const VecDeriv& _f = f.getValue();

    for (unsigned int i=0; i<masses.size(); i++)
    {
        _a[i] = _f[i] / masses[i];
    }
}

template <class DataTypes, class MassType>
SReal DiagonalMass<DataTypes, MassType>::getKineticEnergy( const core::MechanicalParams* /*mparams*/, const DataVecDeriv& v ) const
{

    const Masvector &masses= d_vertexMass.getValue();
    helper::ReadAccessor< DataVecDeriv > _v = v;
    SReal e = 0.0;
    for (unsigned int i=0; i<masses.size(); i++)
    {
        e += _v[i]*masses[i]*_v[i]; // v[i]*v[i]*masses[i] would be more efficient but less generic
    }
    return e/2;
}

template <class DataTypes, class MassType>
SReal DiagonalMass<DataTypes, MassType>::getPotentialEnergy( const core::MechanicalParams* /*mparams*/, const DataVecCoord& x ) const
{

    const Masvector &masses= d_vertexMass.getValue();
    helper::ReadAccessor< DataVecCoord > _x = x;
    SReal e = 0;
    // gravity
    defaulttype::Vec3d g ( this->getContext()->getGravity() );
    Deriv theGravity;
    DataTypes::set ( theGravity, g[0], g[1], g[2]);
    for (unsigned int i=0; i<masses.size(); i++)
    {
        e -= theGravity*masses[i]*_x[i];
    }
    return e;
}

// does nothing by default, need to be specialized in .cpp
template <class DataTypes, class MassType>
defaulttype::Vector6
DiagonalMass<DataTypes, MassType>::getMomentum ( const core::MechanicalParams*, const DataVecCoord& /*vx*/, const DataVecDeriv& /*vv*/  ) const
{
    return defaulttype::Vector6();
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addMToMatrix(const core::MechanicalParams *mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix)
{
    const Masvector &masses= d_vertexMass.getValue();
    const auto N = defaulttype::DataTypeInfo<Deriv>::size();
    AddMToMatrixFunctor<Deriv,MassType> calc;
    sofa::core::behavior::MultiMatrixAccessor::MatrixRef r = matrix->getMatrix(this->mstate);
    Real mFactor = Real(mparams->mFactorIncludingRayleighDamping(this->rayleighMass.getValue()));
    for (unsigned int i=0; i<masses.size(); i++)
        calc(r.matrix, masses[i], r.offset + N*i, mFactor);
}


template <class DataTypes, class MassType>
SReal DiagonalMass<DataTypes, MassType>::getElementMass(sofa::Index index) const
{
    return SReal(d_vertexMass.getValue()[index]);
}


//TODO: special case for Rigid Mass
template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::getElementMass(sofa::Index index, defaulttype::BaseMatrix *m) const
{
    static const defaulttype::BaseMatrix::Index dimension = defaulttype::BaseMatrix::Index(defaulttype::DataTypeInfo<Deriv>::size());
    if (m->rowSize() != dimension || m->colSize() != dimension) m->resize(dimension,dimension);

    m->clear();
    AddMToMatrixFunctor<Deriv,MassType>()(m, d_vertexMass.getValue()[index], 0, 1);
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::reinit()
{
    // Now update is handled through the doUpdateInternal mechanism
    // called at each begin of step through the UpdateInternalDataVisitor
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::initTopologyHandlers()
{
    // add the functions to handle topology changes.
    m_pointHandler = new DMassPointHandler(this, &d_vertexMass);
    d_vertexMass.createTopologicalEngine(m_topology, m_pointHandler);
    if (edgeGeo)
        d_vertexMass.linkToEdgeDataArray();
    if (triangleGeo)
        d_vertexMass.linkToTriangleDataArray();
    if (quadGeo)
        d_vertexMass.linkToQuadDataArray();
    if (tetraGeo)
        d_vertexMass.linkToTetrahedronDataArray();
    if (hexaGeo)
        d_vertexMass.linkToHexahedronDataArray();
    d_vertexMass.registerTopologicalData();
}

template <class DataTypes, class MassType>
bool DiagonalMass<DataTypes, MassType>::checkTopology()
{
    if (l_topology.empty())
    {
        msg_info() << "link to Topology container should be set to ensure right behavior. First Topology found in current context will be used.";
        l_topology.set(this->getContext()->getMeshTopologyLink());

    }

    m_topology = l_topology.get();
    msg_info() << "Topology path used: '" << l_topology.getLinkedPath() << "'";

    if (m_topology == nullptr)
    {
        msg_error() << "No topology component found at path: " << l_topology.getLinkedPath() << ", nor in current context: " << this->getContext()->name;
        return false;
    }

    this->getContext()->get(edgeGeo);
    this->getContext()->get(triangleGeo);
    this->getContext()->get(quadGeo);
    this->getContext()->get(tetraGeo);
    this->getContext()->get(hexaGeo);

    if (m_topology)
    {
        if (m_topology->getNbHexahedra() > 0)
        {
            if(!hexaGeo)
            {
                msg_error() << "Hexahedron topology found but geometry algorithms are missing. Add the component HexahedronSetGeometryAlgorithms.";
                return false;
            }
            else
            {
                msg_info() << "Hexahedral topology found.";
                edgeGeo = nullptr;
                triangleGeo = nullptr;
                quadGeo = nullptr;
                tetraGeo = nullptr;
                return true;
            }
        }
        else if (m_topology->getNbTetrahedra() > 0)
        {
            if(!tetraGeo)
            {
                msg_error() << "Tetrahedron topology found but geometry algorithms are missing. Add the component TetrahedronSetGeometryAlgorithms.";
                return false;
            }
            else
            {
                msg_info() << "Tetrahedral topology found.";
                edgeGeo = nullptr;
                triangleGeo = nullptr;
                quadGeo = nullptr;
                hexaGeo = nullptr;

                return true;
            }
        }
        else if (m_topology->getNbQuads() > 0)
        {
            if(!quadGeo)
            {
                msg_error() << "Quad topology found but geometry algorithms are missing. Add the component QuadSetGeometryAlgorithms.";
                return false;
            }
            else
            {
                msg_info() << "Quad topology found.";
                edgeGeo = nullptr;
                triangleGeo = nullptr;
                tetraGeo  = nullptr;
                hexaGeo = nullptr;
                return true;
            }
        }
        else if (m_topology->getNbTriangles() > 0)
        {
            if(!triangleGeo)
            {
                msg_error() << "Triangle topology found but geometry algorithms are missing. Add the component TriangleSetGeometryAlgorithms.";
                return false;
            }
            else
            {
                msg_info() << "Triangular topology found.";
                edgeGeo = nullptr;
                quadGeo = nullptr;
                tetraGeo  = nullptr;
                hexaGeo = nullptr;
                return true;
            }
        }
        else if (m_topology->getNbEdges() > 0)
        {
            if(!edgeGeo)
            {
                msg_error() << "Edge topology found but geometry algorithms are missing. Add the component EdgeSetGeometryAlgorithms.";
                return false;
            }
            else
            {
                msg_info() << "Edge topology found.";
                triangleGeo = nullptr;
                quadGeo = nullptr;
                tetraGeo  = nullptr;
                hexaGeo = nullptr;
                return true;
            }
        }
        else
        {
            msg_error() << "Topology empty.";
            return false;
        }
    }
    else
    {
        msg_error() << "Topology not found.";
        return false;
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::init()
{
    this->d_componentState.setValue(ComponentState::Valid);

    if (!d_fileMass.getValue().empty())
    {
        if(!load(d_fileMass.getFullPath().c_str())){
            this->d_componentState.setValue(ComponentState::Invalid);
            return;
        }
        msg_warning() << "File given as input for DiagonalMass, in this a case:" << msgendl
                      << "the topology won't be used to compute the mass" << msgendl
                      << "the update, the coherency and the tracking of mass information data are disable (listening = false)";
        Inherited::init();
    }
    else
    {
        if(!checkTopology())
        {
            this->d_componentState.setValue(ComponentState::Invalid);
            return;
        }
        Inherited::init();
        initTopologyHandlers();

        // TODO(dmarchal 2018-11-10): this code is duplicated with the one in RigidImpl we should factor it (remove in 1 year if not done or update the dates)
        if (this->mstate && d_vertexMass.getValue().size() > 0 && d_vertexMass.getValue().size() < unsigned(this->mstate->getSize()))
        {
            Masvector &masses= *d_vertexMass.beginEdit();
            size_t i = masses.size()-1;
            size_t n = size_t(this->mstate->getSize());
            masses.reserve(n);
            while (masses.size() < n)
                masses.push_back(masses[i]);
            d_vertexMass.endEdit();
        }

        massInitialization();

        this->trackInternalData(d_vertexMass);
        this->trackInternalData(d_massDensity);
        this->trackInternalData(d_totalMass);
    }
    this->d_componentState.setValue(ComponentState::Valid);
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::massInitialization()
{
    //Mass initialization process
    if(d_vertexMass.isSet() || d_massDensity.isSet() || d_totalMass.isSet() )
    {
        //totalMass data is prioritary on vertexMass and massDensity
        if (d_totalMass.isSet())
        {
            if(d_vertexMass.isSet() || d_massDensity.isSet())
            {
                msg_warning() << "totalMass value overriding other mass information (vertexMass or massDensity).\n"
                              << "To remove this warning you need to define only one single mass information data field.";
            }
            checkTotalMassInit();
            initFromTotalMass();
        }
        //massDensity is secondly considered
        else if(d_massDensity.isSet())
        {
            if(d_vertexMass.isSet())
            {
                msg_warning() << "massDensity value overriding the value of the attribute vertexMass.\n"
                              << "To remove this warning you need to set either vertexMass or massDensity data field, but not both.";
            }
            if(!checkMassDensity())
            {
                checkTotalMassInit();
                initFromTotalMass();
            }
            else
            {
                initFromMassDensity();
            }
        }
        //finally, the vertexMass is used
        else if(d_vertexMass.isSet())
        {
            if(!checkVertexMass())
            {
                checkTotalMassInit();
                initFromTotalMass();
            }
            else
            {
                initFromVertexMass();
            }
        }
    }
    // if no mass information provided, default initialization uses totalMass
    else
    {
        msg_info() << "No information about the mass is given." << msgendl
                      "Default : totalMass = 1.0";
        checkTotalMassInit();
        initFromTotalMass();
    }

    //Info post-init
    printMass();
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::printMass()
{
    if (this->f_printLog.getValue() == false)
        return;

    const Masvector &vertexM = d_vertexMass.getValue();

    Real average_vertex = 0.0;
    Real min_vertex = std::numeric_limits<Real>::max();
    Real max_vertex = 0.0;

    for(unsigned int i=0; i<vertexM.size(); i++)
    {
        average_vertex += vertexM[i];
        if(vertexM[i]<min_vertex)
            min_vertex = vertexM[i];
        if(vertexM[i]>max_vertex)
            max_vertex = vertexM[i];
    }
    if(vertexM.size() > 0)
    {
        average_vertex /= Real(vertexM.size());
    }

    msg_info() << "mass information computed :" << msgendl
               << "totalMass   = " << d_totalMass.getValue() << msgendl
               << "massDensity = " << d_massDensity.getValue() << msgendl
               << "mean vertexMass [min,max] = " << average_vertex << " [" << min_vertex << "," <<  max_vertex <<"]";
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::computeMass()
{
    if (m_topology && (d_massDensity.getValue() > 0 || d_vertexMass.getValue().size() == 0))
    {
        if (m_topology->getNbHexahedra()>0 && hexaGeo)
        {

            Masvector& masses = *d_vertexMass.beginEdit();
            m_topologyType=TOPOLOGY_HEXAHEDRONSET;

            masses.resize(this->mstate->getSize());
            for(unsigned int i=0; i<masses.size(); ++i)
              masses[i]=Real(0);

            Real md=d_massDensity.getValue();
            Real mass=Real(0);
            Real total_mass=Real(0);

            for (Topology::HexahedronID i=0; i<m_topology->getNbHexahedra(); ++i)
            {
                const Hexahedron &h=m_topology->getHexahedron(i);
                if (hexaGeo)
                {
                    if (d_computeMassOnRest.getValue())
                        mass=(md*hexaGeo->computeRestHexahedronVolume(i))/(Real(8.0));
                    else
                        mass=(md*hexaGeo->computeHexahedronVolume(i))/(Real(8.0));

                    for (unsigned int j = 0 ; j < h.size(); j++)
                    {
                        masses[h[j]] += mass;
                        total_mass += mass;
                    }
                }
            }

            d_totalMass.setValue(total_mass);
            d_vertexMass.endEdit();

        }
        else if (m_topology->getNbTetrahedra()>0 && tetraGeo)
        {

            Masvector& masses = *d_vertexMass.beginEdit();
            m_topologyType=TOPOLOGY_TETRAHEDRONSET;

            // resize array
            clear();
            masses.resize(this->mstate->getSize());

            for(unsigned int i=0; i<masses.size(); ++i)
                masses[i]=Real(0);

            Real md=d_massDensity.getValue();
            Real mass=Real(0);
            Real total_mass=Real(0);

            for (Topology::TetrahedronID i=0; i<m_topology->getNbTetrahedra(); ++i)
            {
                const Tetrahedron &t=m_topology->getTetrahedron(i);
                if(tetraGeo)
                {
                    if (d_computeMassOnRest.getValue())
                        mass=(md*tetraGeo->computeRestTetrahedronVolume(i))/(Real(4.0));
                    else
                        mass=(md*tetraGeo->computeTetrahedronVolume(i))/(Real(4.0));
                }
                for (unsigned int j = 0 ; j < t.size(); j++)
                {
                    masses[t[j]] += mass;
                    total_mass += mass;
                }
            }
            d_totalMass.setValue(total_mass);
            d_vertexMass.endEdit();
        }
        else if (m_topology->getNbQuads()>0 && quadGeo) {
            Masvector& masses = *d_vertexMass.beginEdit();
            m_topologyType=TOPOLOGY_QUADSET;

            // resize array
            clear();
            masses.resize(this->mstate->getSize());

            for(unsigned int i=0; i<masses.size(); ++i)
                masses[i]=Real(0);

            Real md=d_massDensity.getValue();
            Real mass=Real(0);
            Real total_mass=Real(0);

            for (Topology::QuadID i=0; i<m_topology->getNbQuads(); ++i)
            {
                const Quad &t=m_topology->getQuad(i);
                if(quadGeo)
                {
                    if (d_computeMassOnRest.getValue())
                        mass=(md*quadGeo->computeRestQuadArea(i))/(Real(4.0));
                    else
                        mass=(md*quadGeo->computeQuadArea(i))/(Real(4.0));
                }
                for (unsigned int j = 0 ; j < t.size(); j++)
                {
                    masses[t[j]] += mass;
                    total_mass += mass;
                }
            }
            d_totalMass.setValue(total_mass);
            d_vertexMass.endEdit();
        }
        else if (m_topology->getNbTriangles()>0 && triangleGeo)
        {
            Masvector& masses = *d_vertexMass.beginEdit();
            m_topologyType=TOPOLOGY_TRIANGLESET;

            // resize array
            clear();
            masses.resize(this->mstate->getSize());

            for(unsigned int i=0; i<masses.size(); ++i)
                masses[i]=Real(0);

            Real md=d_massDensity.getValue();
            Real mass=Real(0);
            Real total_mass=Real(0);

            for (Topology::TriangleID i=0; i<m_topology->getNbTriangles(); ++i)
            {
                const Triangle &t=m_topology->getTriangle(i);
                if(triangleGeo)
                {
                    if (d_computeMassOnRest.getValue())
                        mass=(md*triangleGeo->computeRestTriangleArea(i))/(Real(3.0));
                    else
                        mass=(md*triangleGeo->computeTriangleArea(i))/(Real(3.0));
                }
                for (unsigned int j = 0 ; j < t.size(); j++)
                {
                    masses[t[j]] += mass;
                    total_mass += mass;
                }
            }
            d_totalMass.setValue(total_mass);
            d_vertexMass.endEdit();
        }
        else if (m_topology->getNbEdges()>0 && edgeGeo)
        {

            Masvector& masses = *d_vertexMass.beginEdit();
            m_topologyType=TOPOLOGY_EDGESET;

            // resize array
            clear();
            masses.resize(this->mstate->getSize());

            for(unsigned int i=0; i<masses.size(); ++i)
                masses[i]=Real(0);

            Real md=d_massDensity.getValue();
            Real mass=Real(0);
            Real total_mass=Real(0);

            for (Topology::EdgeID i=0; i<m_topology->getNbEdges(); ++i)
            {
                const Edge &e=m_topology->getEdge(i);
                if(edgeGeo)
                {
                    if (d_computeMassOnRest.getValue())
                        mass=(md*edgeGeo->computeRestEdgeLength(i))/(Real(2.0));
                    else
                        mass=(md*edgeGeo->computeEdgeLength(i))/(Real(2.0));
                }
                for (unsigned int j = 0 ; j < e.size(); j++)
                {
                    masses[e[j]] += mass;
                    total_mass += mass;
                }
            }
            d_totalMass.setValue(total_mass);
            d_vertexMass.endEdit();
        }
    }
}

template <class DataTypes, class MassType>
bool DiagonalMass<DataTypes, MassType>::checkTotalMass()
{
    //Check for negative or null value, if wrongly set use the default value totalMass = 1.0
    if(d_totalMass.getValue() <= 0.0)
    {
        msg_warning() << "totalMass data can not have a negative value.\n"
                      << "To remove this warning, you need to set a strictly positive value to the totalMass data";
        return false;
    }
    else
    {
        return true;
    }
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::checkTotalMassInit()
{
    //Check for negative or null value, if wrongly set use the default value totalMass = 1.0
    if(!checkTotalMass())
    {
        d_totalMass.setValue(1.0) ;
        msg_warning() << "Switching back to default values: totalMass = 1.0\n";
        this->d_componentState.setValue(ComponentState::Invalid);
    }
}


template <class DataTypes, class MassType>
bool DiagonalMass<DataTypes, MassType>::checkVertexMass()
{
    const Masvector &vertexMass = d_vertexMass.getValue();

    //Check size of the vector
    if (vertexMass.size() != size_t(m_topology->getNbPoints()))
    {
        msg_warning() << "Inconsistent size of vertexMass vector ("<< vertexMass.size() <<") compared to the DOFs size ("<< m_topology->getNbPoints() <<").";
        return false;
    }
    else
    {
        //Check that the vertexMass vector has only strictly positive values
        for(size_t i=0; i<vertexMass.size(); i++)
        {
            if(vertexMass[i]<=0)
            {
                msg_warning() << "Negative value of vertexMass vector: vertexMass[" << i << "] = " << vertexMass[i];
                return false;
            }
        }
        return true;
    }
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::initFromVertexMass()
{
    msg_info() << "vertexMass information is used";

    const Masvector& vertexMass = d_vertexMass.getValue();
    Real totalMassSave = 0.0;
    for(size_t i=0; i<vertexMass.size(); i++)
    {
        totalMassSave += vertexMass[i];
    }

    d_totalMass.setValue(totalMassSave);
    initFromTotalMass();
    helper::WriteAccessor<Data<Masvector> > vertexMassWrite = d_vertexMass;
    for(size_t i=0; i<vertexMassWrite.size(); i++)
    {
        vertexMassWrite[i] = vertexMass[i];
    }
}


template <class DataTypes, class MassType>
bool DiagonalMass<DataTypes, MassType>::checkMassDensity()
{
    const Real &massDensity = d_massDensity.getValue();

    //Check that the massDensity is strictly positive
    if(massDensity <= 0.0)
    {
        msg_warning() << "Negative value of massDensity: massDensity = " << massDensity;
        return false;
    }
    else
    {
        return true;
    }
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::initFromMassDensity()
{
    msg_info() << "massDensity information is used";

    // Compute Mass per vertex using mesh topology
    computeMass();

    // Sum the mass per vertex to obtain total mass
    const Masvector &vertexMass = d_vertexMass.getValue();    
    Real sumMass = 0.0;
    for (auto vMass : vertexMass)
        sumMass += vMass;

    d_totalMass.setValue(sumMass);
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::initFromTotalMass()
{
    msg_info() << "totalMass information is used";

    const Real totalMassTemp = d_totalMass.getValue();

    Real sumMass = 0.0;
    setMassDensity(1.0);

    // Compute Mass per vertex using mesh topology
    computeMass();

    // Sum the mass per vertex to obtain total mass
    const Masvector &vertexMass = d_vertexMass.getValue();
    for (auto vMass : vertexMass)
        sumMass += vMass;

    setMassDensity(Real(totalMassTemp/sumMass));

    computeMass();
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::setVertexMass(sofa::helper::vector< Real > vertexMass)
{
    const Masvector currentVertexMass = d_vertexMass.getValue();
    helper::WriteAccessor<Data<Masvector> > vertexMassWrite = d_vertexMass;
    vertexMassWrite.resize(vertexMass.size());
    for(size_t i=0; i<vertexMass.size(); i++)
    {
        vertexMassWrite[i] = vertexMass[i];
    }

    if(!checkVertexMass())
    {
        msg_warning() << "Given values to setVertexMass() are not correct.\n"
                      << "Previous values are used.";
        d_vertexMass.setValue(currentVertexMass);
    }
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::setMassDensity(Real massDensityValue)
{
    const Real currentMassDensity = d_massDensity.getValue();
    d_massDensity.setValue(massDensityValue);
    if(!checkMassDensity())
    {
        msg_warning() << "Given values to setMassDensity() are not correct.\n"
                      << "Previous values are used.";
        d_massDensity.setValue(currentMassDensity);
    }
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::setTotalMass(Real totalMass)
{
    const Real currentTotalMass = d_totalMass.getValue();
    d_totalMass.setValue(totalMass);
    if(!checkTotalMass())
    {
        msg_warning() << "Given value to setTotalMass() is not a strictly positive value\n"
                      << "Previous value is used: totalMass = " << currentTotalMass;
        d_totalMass.setValue(currentTotalMass);
    }
}


template <class DataTypes, class MassType>
const typename DiagonalMass<DataTypes, MassType>::Real &DiagonalMass<DataTypes, MassType>::getMassDensity()
{
    return d_massDensity.getValue();
}


template <class DataTypes, class MassType>
const typename DiagonalMass<DataTypes, MassType>::Real &DiagonalMass<DataTypes, MassType>::getTotalMass()
{
    return d_totalMass.getValue();
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::doUpdateInternal()
{
    if (this->hasDataChanged(d_totalMass))
    {
        if(checkTotalMass())
        {
            initFromTotalMass();
            this->d_componentState.setValue(ComponentState::Valid);
        }
        else
        {
            msg_error() << "doUpdateInternal: incorrect update from totalMass";
            this->d_componentState.setValue(ComponentState::Invalid);
        }
    }
    else if(this->hasDataChanged(d_massDensity))
    {
        if(checkMassDensity())
        {
            initFromMassDensity();
            this->d_componentState.setValue(ComponentState::Valid);
        }
        else
        {
            msg_error() << "doUpdateInternal: incorrect update from massDensity";
            this->d_componentState.setValue(ComponentState::Invalid);
        }
    }
    else if(this->hasDataChanged(d_vertexMass))
    {
        if(checkVertexMass())
        {
            initFromVertexMass();
            this->d_componentState.setValue(ComponentState::Valid);
        }
        else
        {
            msg_error() << "doUpdateInternal: incorrect update from vertexMass";
            this->d_componentState.setValue(ComponentState::Invalid);
        }
    }

    //Info post-init
    msg_info() << "mass information updated";
    printMass();
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addGravityToV(const core::MechanicalParams* mparams, DataVecDeriv& d_v)
{
    if(mparams)
    {
        VecDeriv& v = *d_v.beginEdit();
        // gravity
        sofa::defaulttype::Vec3d g ( this->getContext()->getGravity() );
        Deriv theGravity;
        DataTypes::set ( theGravity, g[0], g[1], g[2]);
        Deriv hg = theGravity * typename DataTypes::Real(mparams->dt());

        for (unsigned int i=0; i<v.size(); i++)
        {
            v[i] += hg;
        }
        d_v.endEdit();
    }
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addForce(const core::MechanicalParams* /*mparams*/, DataVecDeriv& f, const DataVecCoord& , const DataVecDeriv& )
{
    //if gravity was added separately (in solver's "solve" method), then nothing to do here
    if(this->m_separateGravity.getValue())
        return;

    const Masvector &masses= d_vertexMass.getValue();
    helper::WriteAccessor< DataVecDeriv > _f = f;

    // gravity
    sofa::defaulttype::Vec3d g ( this->getContext()->getGravity() );
    Deriv theGravity;
    DataTypes::set ( theGravity, g[0], g[1], g[2]);


    // add weight and inertia force
    for (unsigned int i=0; i<masses.size(); i++)
    {
        _f[i] += theGravity*masses[i];
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::draw(const core::visual::VisualParams* vparams)
{
    if (!vparams->displayFlags().getShowBehaviorModels())
        return;

    const Masvector &masses= d_vertexMass.getValue();
    if (masses.empty())
        return;

    const VecCoord& x = this->mstate->read(core::ConstVecCoordId::position())->getValue();
    Coord gravityCenter;
    Real totalMass=0.0;

    std::vector<  sofa::defaulttype::Vector3 > points;

    for (unsigned int i=0; i<x.size(); i++)
    {
        sofa::defaulttype::Vector3 p;
        p = DataTypes::getCPos(x[i]);

        points.push_back(p);
        gravityCenter += x[i]*masses[i];
        totalMass += masses[i];
    }

    if ( d_showCenterOfGravity.getValue() )
    {
        gravityCenter /= totalMass;
        const sofa::defaulttype::Vec4f color(1.0,1.0,0.0,1.0);

        Real axisSize = d_showAxisSize.getValue();
        sofa::defaulttype::Vector3 temp;

        for ( unsigned int i=0 ; i<3 ; i++ )
            if(i < Coord::spatial_dimensions )
                temp[i] = gravityCenter[i];

        vparams->drawTool()->drawCross(temp, axisSize, color);
    }
}

template <class DataTypes, class MassType>
class DiagonalMass<DataTypes, MassType>::Loader : public helper::io::XspLoaderDataHook
{
public:
    DiagonalMass<DataTypes, MassType>* dest;
    Loader(DiagonalMass<DataTypes, MassType>* dest) : dest(dest) {}
    void addMass(SReal /*px*/, SReal /*py*/, SReal /*pz*/, SReal /*vx*/, SReal /*vy*/, SReal /*vz*/, SReal mass, SReal /*elastic*/, bool /*fixed*/, bool /*surface*/) override
    {
        dest->addMass(MassType(Real(mass)));
    }
};

template <class DataTypes, class MassType>
bool DiagonalMass<DataTypes, MassType>::load(const char *filename)
{
    clear();
    if (filename!=nullptr && filename[0]!='\0')
    {
        Loader loader(this);
        return helper::io::XspLoader::Load(filename, loader);
    }
    return false;
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::handleEvent(sofa::core::objectmodel::Event *event)
{
     SOFA_UNUSED(event);
}


} // namespace sofa::component::mass
