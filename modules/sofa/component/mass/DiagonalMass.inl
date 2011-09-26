/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_MASS_DIAGONALMASS_INL
#define SOFA_COMPONENT_MASS_DIAGONALMASS_INL

#include <sofa/component/mass/DiagonalMass.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/behavior/Mass.inl>
#include <sofa/helper/io/MassSpringLoader.h>
#include <sofa/helper/gl/template.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/DataTypeInfo.h>
#include <sofa/component/topology/TopologyChangedEvent.h>
#include <sofa/component/topology/PointData.inl>
#include <sofa/component/topology/RegularGridTopology.h>
#include <sofa/component/mass/AddMToMatrixFunctor.h>
#include <sofa/simulation/common/Simulation.h>

#ifdef SOFA_SUPPORT_MOVING_FRAMES
#include <sofa/core/behavior/InertiaForce.h>
#endif

namespace sofa
{

namespace component
{

namespace mass
{


using namespace	sofa::component::topology;
using namespace core::topology;

template<class MassType>
void MassPointCreationFunction(int ,
        void* , MassType & t,
        const sofa::helper::vector< unsigned int > &,
        const sofa::helper::vector< double >&)
{
    t=0;
}

template< class DataTypes, class MassType, class MassVector>
inline void MassEdgeCreationFunction(const sofa::helper::vector<unsigned int> &edgeAdded,
        void* param, MassVector &masses)
{
    DiagonalMass<DataTypes, MassType> *dm= (DiagonalMass<DataTypes, MassType> *)param;
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_EDGESET)
    {

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=(typename DataTypes::Real) 0;
        unsigned int i;

        for (i=0; i<edgeAdded.size(); ++i)
        {
            /// get the edge to be added
            const Edge &e=dm->_topology->getEdge(edgeAdded[i]);
            // compute its mass based on the mass density and the edge length
            if(dm->edgeGeo)
            {
                mass=(md*dm->edgeGeo->computeRestEdgeLength(edgeAdded[i]))/(typename DataTypes::Real)2.0;
            }
            // added mass on its two vertices
            masses[e[0]]+=mass;
            masses[e[1]]+=mass;
        }

    }
}

template< class DataTypes, class MassType, class MassVector>
inline void MassEdgeDestroyFunction(const sofa::helper::vector<unsigned int> &edgeRemoved,
        void* param, MassVector &masses)
{
    DiagonalMass<DataTypes, MassType> *dm= (DiagonalMass<DataTypes, MassType> *)param;
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_EDGESET)
    {

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=(typename DataTypes::Real) 0;
        unsigned int i;

        for (i=0; i<edgeRemoved.size(); ++i)
        {
            /// get the edge to be added
            const Edge &e=dm->_topology->getEdge(edgeRemoved[i]);
            // compute its mass based on the mass density and the edge length
            if(dm->edgeGeo)
            {
                mass=(md*dm->edgeGeo->computeRestEdgeLength(edgeRemoved[i]))/(typename DataTypes::Real)2.0;
            }
            // added mass on its two vertices
            masses[e[0]]-=mass;
            masses[e[1]]-=mass;
        }

    }
}

template< class DataTypes, class MassType, class MassVector>
inline void MassTriangleCreationFunction(const sofa::helper::vector<unsigned int> &triangleAdded,
        void* param, MassVector &masses)
{
    DiagonalMass<DataTypes, MassType> *dm= (DiagonalMass<DataTypes, MassType> *)param;
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_TRIANGLESET)
    {

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=(typename DataTypes::Real) 0;
        unsigned int i;

        for (i=0; i<triangleAdded.size(); ++i)
        {
            /// get the triangle to be added
            const Triangle &t=dm->_topology->getTriangle(triangleAdded[i]);
            // compute its mass based on the mass density and the triangle area
            if(dm->triangleGeo)
            {
                mass=(md*dm->triangleGeo->computeRestTriangleArea(triangleAdded[i]))/(typename DataTypes::Real)3.0;
            }
            // removed  mass on its three vertices
            masses[t[0]]+=mass;
            masses[t[1]]+=mass;
            masses[t[2]]+=mass;
        }

    }
}

template< class DataTypes, class MassType, class MassVector>
inline void MassTriangleDestroyFunction(const sofa::helper::vector<unsigned int> &triangleRemoved,
        void* param, MassVector &masses)
{
    DiagonalMass<DataTypes, MassType> *dm= (DiagonalMass<DataTypes, MassType> *)param;
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_TRIANGLESET)
    {

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=(typename DataTypes::Real) 0;
        unsigned int i;

        for (i=0; i<triangleRemoved.size(); ++i)
        {
            /// get the triangle to be added
            const Triangle &t=dm->_topology->getTriangle(triangleRemoved[i]);
            // compute its mass based on the mass density and the triangle area
            if(dm->triangleGeo)
            {
                mass=(md*dm->triangleGeo->computeRestTriangleArea(triangleRemoved[i]))/(typename DataTypes::Real)3.0;
            }
            // removed  mass on its three vertices
            masses[t[0]]-=mass;
            masses[t[1]]-=mass;
            masses[t[2]]-=mass;
            // Commented to prevent from printing in case of triangle removal
            //serr<< "mass vertex " << t[0]<< " = " << masses[t[0]]<<sendl;
            //serr<< "mass vertex " << t[1]<< " = " << masses[t[1]]<<sendl;
            //serr<< "mass vertex " << t[2]<< " = " << masses[t[2]]<<sendl;
        }

    }
}

template< class DataTypes, class MassType, class MassVector>
inline void MassTetrahedronCreationFunction(const sofa::helper::vector<unsigned int> &tetrahedronAdded,
        void* param, MassVector &masses)
{
    DiagonalMass<DataTypes, MassType> *dm= (DiagonalMass<DataTypes, MassType> *)param;
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_TETRAHEDRONSET)
    {

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=(typename DataTypes::Real) 0;
        unsigned int i;

        for (i=0; i<tetrahedronAdded.size(); ++i)
        {
            /// get the tetrahedron to be added
            const Tetrahedron &t=dm->_topology->getTetrahedron(tetrahedronAdded[i]);
            // compute its mass based on the mass density and the tetrahedron volume
            if(dm->tetraGeo)
            {
                mass=(md*dm->tetraGeo->computeRestTetrahedronVolume(tetrahedronAdded[i]))/(typename DataTypes::Real)4.0;
            }
            // removed  mass on its four vertices
            masses[t[0]]+=mass;
            masses[t[1]]+=mass;
            masses[t[2]]+=mass;
            masses[t[3]]+=mass;

        }

    }
}

template< class DataTypes, class MassType, class MassVector>
inline void MassTetrahedronDestroyFunction(const sofa::helper::vector<unsigned int> &tetrahedronRemoved,
        void* param, MassVector &masses)
{
    DiagonalMass<DataTypes, MassType> *dm= (DiagonalMass<DataTypes, MassType> *)param;
    if (dm->getMassTopologyType()==DiagonalMass<DataTypes, MassType>::TOPOLOGY_TETRAHEDRONSET)
    {

        typename DataTypes::Real md=dm->getMassDensity();
        typename DataTypes::Real mass=(typename DataTypes::Real) 0;
        unsigned int i;

        for (i=0; i<tetrahedronRemoved.size(); ++i)
        {
            /// get the tetrahedron to be added
            const Tetrahedron &t=dm->_topology->getTetrahedron(tetrahedronRemoved[i]);
            if(dm->tetraGeo)
            {
                // compute its mass based on the mass density and the tetrahedron volume
                mass=(md*dm->tetraGeo->computeRestTetrahedronVolume(tetrahedronRemoved[i]))/(typename DataTypes::Real)4.0;
            }
            // removed  mass on its four vertices
            masses[t[0]]-=mass;
            masses[t[1]]-=mass;
            masses[t[2]]-=mass;
            masses[t[3]]-=mass;
        }

    }
}


using namespace sofa::defaulttype;
using namespace sofa::core::behavior;


template <class DataTypes, class MassType>
DiagonalMass<DataTypes, MassType>::DiagonalMass()
    : f_mass( initData(&f_mass, "mass", "values of the particles masses") )
    , m_massDensity( initData(&m_massDensity, (Real)1.0,"massDensity", "mass density that allows to compute the  particles masses from a mesh topology and geometry.\nOnly used if > 0") )
    , showCenterOfGravity( initData(&showCenterOfGravity, false, "showGravityCenter", "display the center of gravity of the system" ) )
    , showAxisSize( initData(&showAxisSize, 1.0f, "showAxisSizeFactor", "factor length of the axis displayed (only used for rigids)" ) )
    , fileMass( initData(&fileMass,  "fileMass", "File to specify the mass" ) )
    , topologyType(TOPOLOGY_UNKNOWN)
{
    this->addAlias(&fileMass,"filename");
}




template <class DataTypes, class MassType>
DiagonalMass<DataTypes, MassType>::~DiagonalMass()
{
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::clear()
{
    MassVector& masses = *f_mass.beginEdit();
    masses.clear();
    f_mass.endEdit();
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addMass(const MassType& m)
{
    MassVector& masses = *f_mass.beginEdit();
    masses.push_back(m);
    f_mass.endEdit();
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::resize(int vsize)
{
    MassVector& masses = *f_mass.beginEdit();
    masses.resize(vsize);
    f_mass.endEdit();
}

// -- Mass interface
template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addMDx(const core::MechanicalParams* /*mparams*/ /* PARAMS FIRST */, DataVecDeriv& res, const DataVecDeriv& dx, double factor)
{
    const MassVector &masses= f_mass.getValue();
    //std::cout << "DIAGONALMASS: dx size = " << dx.size() << " res size = " << res.size() << " masses size = " << masses.size() << std::endl;
    helper::WriteAccessor< DataVecDeriv > _res = res;
    helper::ReadAccessor< DataVecDeriv > _dx = dx;

    unsigned int n = masses.size();
    if (_dx.size() < n) n = _dx.size();
    if (_res.size() < n) n = _res.size();
    if (factor == 1.0)
    {
        for (unsigned int i=0; i<n; i++)
        {
            _res[i] += _dx[i] * masses[i];
        }
    }
    else
    {
        for (unsigned int i=0; i<n; i++)
        {
            _res[i] += (_dx[i] * masses[i]) * (Real)factor;
        }
    }
}



template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::accFromF(const core::MechanicalParams* /*mparams*/ /* PARAMS FIRST */, DataVecDeriv& a, const DataVecDeriv& f)
{

    const MassVector &masses= f_mass.getValue();
    helper::WriteAccessor< DataVecDeriv > _a = a;
    const VecDeriv& _f = f.getValue();

    for (unsigned int i=0; i<masses.size(); i++)
    {
        _a[i] = _f[i] / masses[i];
    }
}

template <class DataTypes, class MassType>
double DiagonalMass<DataTypes, MassType>::getKineticEnergy( const core::MechanicalParams* /*mparams*/ /* PARAMS FIRST */, const DataVecDeriv& v ) const
{

    const MassVector &masses= f_mass.getValue();
    helper::ReadAccessor< DataVecDeriv > _v = v;
    double e = 0;
    for (unsigned int i=0; i<masses.size(); i++)
    {
        e += _v[i]*masses[i]*_v[i]; // v[i]*v[i]*masses[i] would be more efficient but less generic
    }
    return e/2;
}

template <class DataTypes, class MassType>
double DiagonalMass<DataTypes, MassType>::getPotentialEnergy( const core::MechanicalParams* /*mparams*/ /* PARAMS FIRST */, const DataVecCoord& x ) const
{

    const MassVector &masses= f_mass.getValue();
    helper::ReadAccessor< DataVecCoord > _x = x;
    SReal e = 0;
    // gravity
    Vec3d g ( this->getContext()->getGravity() );
    Deriv theGravity;
    DataTypes::set ( theGravity, g[0], g[1], g[2]);
    for (unsigned int i=0; i<masses.size(); i++)
    {
        e -= theGravity*masses[i]*_x[i];
    }
    return e;
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addMToMatrix(const core::MechanicalParams *mparams /* PARAMS FIRST */, const sofa::core::behavior::MultiMatrixAccessor* matrix)
{
    const MassVector &masses= f_mass.getValue();
    const int N = defaulttype::DataTypeInfo<Deriv>::size();
    AddMToMatrixFunctor<Deriv,MassType> calc;
    sofa::core::behavior::MultiMatrixAccessor::MatrixRef r = matrix->getMatrix(this->mstate);
    Real mFactor = (Real)mparams->mFactor();
    for (unsigned int i=0; i<masses.size(); i++)
        calc(r.matrix, masses[i], r.offset + N*i, mFactor);
}


template <class DataTypes, class MassType>
double DiagonalMass<DataTypes, MassType>::getElementMass(unsigned int index) const
{
    return (SReal)(f_mass.getValue()[index]);
}


//TODO: special case for Rigid Mass
template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::getElementMass(unsigned int index, defaulttype::BaseMatrix *m) const
{
    const unsigned int dimension = defaulttype::DataTypeInfo<Deriv>::size();
    if (m->rowSize() != dimension || m->colSize() != dimension) m->resize(dimension,dimension);

    m->clear();
    AddMToMatrixFunctor<Deriv,MassType>()(m, f_mass.getValue()[index], 0, 1);
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=_topology->beginChange();
    std::list<const TopologyChange *>::const_iterator itEnd=_topology->endChange();

    f_mass.handleTopologyEvents(itBegin,itEnd);
}


template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::reinit()
{
    if (_topology && (m_massDensity.getValue() > 0 || f_mass.getValue().size() == 0))
    {
        if (_topology->getNbTetrahedra()>0 && tetraGeo)
        {

            MassVector& masses = *f_mass.beginEdit();
            topologyType=TOPOLOGY_TETRAHEDRONSET;

            // resize array
            clear();
            masses.resize(this->mstate->getSize());

            for(unsigned int i=0; i<masses.size(); ++i)
                masses[i]=(Real)0;

            Real md=m_massDensity.getValue();
            Real mass=(Real)0;

            for (int i=0; i<_topology->getNbTetrahedra(); ++i)
            {

                const Tetrahedron &t=_topology->getTetrahedron(i);
                if(tetraGeo)
                {
                    mass=(md*tetraGeo->computeRestTetrahedronVolume(i))/(Real)4.0;
                }
                masses[t[0]]+=mass;
                masses[t[1]]+=mass;
                masses[t[2]]+=mass;
                masses[t[3]]+=mass;
            }
            f_mass.endEdit();
        }
        else if (_topology->getNbTriangles()>0 && triangleGeo)
        {
            MassVector& masses = *f_mass.beginEdit();
            topologyType=TOPOLOGY_TRIANGLESET;

            // resize array
            clear();
            masses.resize(this->mstate->getSize());

            for(unsigned int i=0; i<masses.size(); ++i)
                masses[i]=(Real)0;

            Real md=m_massDensity.getValue();
            Real mass=(Real)0;

            for (int i=0; i<_topology->getNbTriangles(); ++i)
            {
                const Triangle &t=_topology->getTriangle(i);
                if(triangleGeo)
                {
                    mass=(md*triangleGeo->computeRestTriangleArea(i))/(Real)3.0;
                }
                masses[t[0]]+=mass;
                masses[t[1]]+=mass;
                masses[t[2]]+=mass;
            }
            f_mass.endEdit();
        }
        /*
          else if (_topology->getNbHexahedra()>0) {

          // TODO : Hexahedra
          topologyType=TOPOLOGY_HEXAHEDRONSET;
          }
          else if (_topology->getNbQuads()>0) {

          // TODO : Quads
          topologyType=TOPOLOGY_QUADSET;
          }
        */
        else if (_topology->getNbEdges()>0 && edgeGeo)
        {

            MassVector& masses = *f_mass.beginEdit();
            topologyType=TOPOLOGY_EDGESET;

            // resize array
            clear();
            masses.resize(this->mstate->getSize());

            for(unsigned int i=0; i<masses.size(); ++i)
                masses[i]=(Real)0;

            Real md=m_massDensity.getValue();
            Real mass=(Real)0;

            for (int i=0; i<_topology->getNbEdges(); ++i)
            {
                const Edge &e=_topology->getEdge(i);
                if(edgeGeo)
                {
                    mass=(md*edgeGeo->computeEdgeLength(i))/(Real)2.0;
                }
                masses[e[0]]+=mass;
                masses[e[1]]+=mass;
            }
            f_mass.endEdit();
        }
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::init()
{
    if (!fileMass.getValue().empty())
        load(fileMass.getFullPath().c_str());

    _topology = this->getContext()->getMeshTopology();

    this->getContext()->get(edgeGeo);
    this->getContext()->get(triangleGeo);
    this->getContext()->get(quadGeo);
    this->getContext()->get(tetraGeo);
    this->getContext()->get(hexaGeo);

    Inherited::init();

    // add the functions to handle topology changes.

    //	VecMass& masses = *f_mass.beginEdit();
    f_mass.createTopologicalEngine(_topology);
    f_mass.setCreateFunction(MassPointCreationFunction<MassType>);
    f_mass.setCreateEdgeFunction(MassEdgeCreationFunction<DataTypes,MassType,MassVector>);
    f_mass.setDestroyEdgeFunction(MassEdgeDestroyFunction<DataTypes,MassType,MassVector>);
    f_mass.setCreateTriangleFunction(MassTriangleCreationFunction<DataTypes,MassType,MassVector>);
    f_mass.setDestroyTriangleFunction(MassTriangleDestroyFunction<DataTypes,MassType,MassVector>);
    f_mass.setCreateTetrahedronFunction(MassTetrahedronCreationFunction<DataTypes,MassType,MassVector>);
    f_mass.setDestroyTetrahedronFunction(MassTetrahedronDestroyFunction<DataTypes,MassType,MassVector>);
    f_mass.setCreateParameter( (void *) this );
    f_mass.setDestroyParameter( (void *) this );
    f_mass.registerTopologicalData();

    //    f_mass.endEdit();

    if (this->mstate && f_mass.getValue().size() > 0 && f_mass.getValue().size() < (unsigned)this->mstate->getSize())
    {
        MassVector &masses= *f_mass.beginEdit();
        unsigned int i = masses.size()-1;
        unsigned int n = (unsigned)this->mstate->getSize();
        while (masses.size() < n)
            masses.push_back(masses[i]);
        f_mass.endEdit();
    }

    if ((f_mass.getValue().size()==0) && (_topology!=0))
    {
        reinit();
    }
}

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addGravityToV(const core::MechanicalParams* mparams /* PARAMS FIRST */, DataVecDeriv& d_v)
{
    if(mparams)
    {
        VecDeriv& v = *d_v.beginEdit();
        // gravity
        Vec3d g ( this->getContext()->getGravity() );
        Deriv theGravity;
        DataTypes::set ( theGravity, g[0], g[1], g[2]);
        Deriv hg = theGravity * (typename DataTypes::Real)mparams->dt();

        for (unsigned int i=0; i<v.size(); i++)
        {
            v[i] += hg;
        }
        d_v.endEdit();
    }
}


#ifdef SOFA_SUPPORT_MOVING_FRAMES
template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addForce(const core::MechanicalParams* /*mparams*/ /* PARAMS FIRST */, DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v)
{

    const MassVector &masses= f_mass.getValue();
    helper::WriteAccessor< DataVecDeriv > _f = f;
    helper::ReadAccessor< DataVecCoord > _x = x;
    helper::ReadAccessor< DataVecDeriv > _v = v;

    // gravity
    Vec3d g ( this->getContext()->getGravity() );
    Deriv theGravity;
    DataTypes::set ( theGravity, g[0], g[1], g[2]);

    // velocity-based stuff
    core::objectmodel::BaseContext::SpatialVector vframe = this->getContext()->getVelocityInWorld();
    core::objectmodel::BaseContext::Vec3 aframe = this->getContext()->getVelocityBasedLinearAccelerationInWorld() ;

    // project back to local frame
    vframe = this->getContext()->getPositionInWorld() / vframe;
    aframe = this->getContext()->getPositionInWorld().backProjectVector( aframe );

    // add weight and inertia force
    if(this->m_separateGravity.getValue()) for (unsigned int i=0; i<masses.size(); i++)
        {
            _f[i] += core::behavior::inertiaForce(vframe,aframe,masses[i],_x[i],_v[i]);
        }
    else for (unsigned int i=0; i<masses.size(); i++)
        {
            _f[i] += theGravity*masses[i] + core::behavior::inertiaForce(vframe,aframe,masses[i],_x[i],_v[i]);
        }
}
#else
template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::addForce(const core::MechanicalParams* /*mparams*/ /* PARAMS FIRST */, DataVecDeriv& f, const DataVecCoord& , const DataVecDeriv& )
{
    //if gravity was added separately (in solver's "solve" method), then nothing to do here
    if(this->m_separateGravity.getValue())
        return;

    const MassVector &masses= f_mass.getValue();
    helper::WriteAccessor< DataVecDeriv > _f = f;

    // gravity
    Vec3d g ( this->getContext()->getGravity() );
    Deriv theGravity;
    DataTypes::set ( theGravity, g[0], g[1], g[2]);


    // add weight and inertia force
    for (unsigned int i=0; i<masses.size(); i++)
    {
        _f[i] += theGravity*masses[i];
    }
}
#endif

template <class DataTypes, class MassType>
void DiagonalMass<DataTypes, MassType>::draw(const core::visual::VisualParams* vparams)
{
    if (!vparams->displayFlags().getShowBehaviorModels()) return;
    const MassVector &masses= f_mass.getValue();
    if (masses.empty()) return;

    const VecCoord& x = *this->mstate->getX();
    Coord gravityCenter;
    Real totalMass=0.0;

    std::vector<  Vector3 > points;
    std::vector< Vec<2,int> > indices;

    for (unsigned int i=0; i<x.size(); i++)
    {
        Vector3 p;
        p = DataTypes::getCPos(x[i]);

        points.push_back(p);
        gravityCenter += x[i]*masses[i];
        totalMass += masses[i];
    }

    vparams->drawTool()->drawPoints(points, 2, Vec<4,float>(1,1,1,1));

    if(showCenterOfGravity.getValue())
    {
        glBegin (GL_LINES);
        glColor4f (1,1,0,1);
        glPointSize(5);
        gravityCenter /= totalMass;
        for(unsigned int i=0 ; i<Coord::spatial_dimensions ; i++)
        {
            Coord v;
            v[i] = showAxisSize.getValue();
            helper::gl::glVertexT(gravityCenter-v);
            helper::gl::glVertexT(gravityCenter+v);
        }
        glEnd();
    }
}

template <class DataTypes, class MassType>
bool DiagonalMass<DataTypes, MassType>::addBBox(double* minBBox, double* maxBBox)
{
    const VecCoord& x = *this->mstate->getX();
    for (unsigned int i=0; i<x.size(); i++)
    {
        //const Coord& p = x[i];
        Real p[3] = {0.0, 0.0, 0.0};
        DataTypes::get(p[0],p[1],p[2],x[i]);
        for (int c=0; c<3; c++)
        {
            if (p[c] > maxBBox[c]) maxBBox[c] = p[c];
            if (p[c] < minBBox[c]) minBBox[c] = p[c];
        }
    }
    return true;
}

template <class DataTypes, class MassType>
class DiagonalMass<DataTypes, MassType>::Loader : public helper::io::MassSpringLoader
{
public:
    DiagonalMass<DataTypes, MassType>* dest;
    Loader(DiagonalMass<DataTypes, MassType>* dest) : dest(dest) {}
    virtual void addMass(SReal /*px*/, SReal /*py*/, SReal /*pz*/, SReal /*vx*/, SReal /*vy*/, SReal /*vz*/, SReal mass, SReal /*elastic*/, bool /*fixed*/, bool /*surface*/)
    {
        dest->addMass(MassType((Real)mass));
    }
};

template <class DataTypes, class MassType>
bool DiagonalMass<DataTypes, MassType>::load(const char *filename)
{
    clear();
    if (filename!=NULL && filename[0]!='\0')
    {
        Loader loader(this);
        return loader.load(filename);
    }
    else return false;
}

// Specialization for rigids
#ifndef SOFA_FLOAT
/*template <>
    inline void MassEdgeDestroyFunction<Rigid3dTypes, Rigid3dMass>(const sofa::helper::vector<unsigned int> &,
    void* , vector<Rigid3dMass> &);

template <>
    inline void MassEdgeCreationFunction<Rigid3dTypes, Rigid3dMass>(const sofa::helper::vector<unsigned int> &,
    void* , vector<Rigid3dMass> &);*/

template <>
double DiagonalMass<Rigid3dTypes, Rigid3dMass>::getPotentialEnergy( const core::MechanicalParams* mparams /* PARAMS FIRST */, const DataVecCoord& x) const;
template <>
double DiagonalMass<Rigid2dTypes, Rigid2dMass>::getPotentialEnergy( const core::MechanicalParams* mparams /* PARAMS FIRST */, const DataVecCoord& x) const;
template <>
void DiagonalMass<Rigid3dTypes, Rigid3dMass>::draw(const core::visual::VisualParams* vparams);
template <>
void DiagonalMass<Rigid2dTypes, Rigid2dMass>::draw(const core::visual::VisualParams* vparams);
#endif
#ifndef SOFA_DOUBLE
template <>
double DiagonalMass<Rigid3fTypes, Rigid3fMass>::getPotentialEnergy( const core::MechanicalParams* mparams /* PARAMS FIRST */, const DataVecCoord& x) const;
template <>
double DiagonalMass<Rigid2fTypes, Rigid2fMass>::getPotentialEnergy( const core::MechanicalParams* mparams /* PARAMS FIRST */, const DataVecCoord& x) const;

template <>
void DiagonalMass<Rigid3fTypes, Rigid3fMass>::draw(const core::visual::VisualParams* vparams);
template <>
void DiagonalMass<Rigid2fTypes, Rigid2fMass>::draw(const core::visual::VisualParams* vparams);
#endif




} // namespace mass

} // namespace component

} // namespace sofa

#endif
