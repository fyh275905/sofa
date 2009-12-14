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
#ifndef SOFA_COMPONENT_MASS_MESHMATRIXMASS_INL
#define SOFA_COMPONENT_MASS_MESHMATRIXMASS_INL

#include <sofa/component/mass/MeshMatrixMass.h>
//#include <sofa/helper/io/MassSpringLoader.h>
#include <sofa/helper/gl/template.h>
//#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/DataTypeInfo.h>
#include <sofa/component/topology/TopologyChangedEvent.h>
#include <sofa/component/topology/PointData.inl>
#include <sofa/component/topology/EdgeData.inl>
#include <sofa/component/topology/RegularGridTopology.h>
#include <sofa/component/mass/AddMToMatrixFunctor.h>
#include <sofa/simulation/common/Simulation.h>

namespace sofa
{

namespace component
{

namespace mass
{


using namespace	sofa::component::topology;
using namespace core::componentmodel::topology;


template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::VertexMassCreationFunction(int, void* , MassType & VertexMass,
        const sofa::helper::vector< unsigned int > &,
        const sofa::helper::vector< double >&)
{
    VertexMass = 0;
}


template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::EdgeMassCreationFunction(int, void* , MassType & EdgeMass,
        const Edge&,
        const sofa::helper::vector< unsigned int > &,
        const sofa::helper::vector< double >&)
{
    EdgeMass = 0;
}

// -------------------------------------------------------
// ------- Triangle Creation/Destruction functions -------
// -------------------------------------------------------
//{

/// Creation fonction for mass stored on vertices
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::VertexMassTriangleCreationFunction(const sofa::helper::vector<unsigned int> &triangleAdded,
        void* param, vector<MassType> &VertexMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_TRIANGLESET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<triangleAdded.size(); ++i)
        {
            // Get the triangle to be added
            const Triangle &t = MMM->_topology->getTriangle(triangleAdded[i]);

            // Compute rest mass of conserne triangle = density * triangle surface.
            if(MMM->triangleGeo)
            {
                mass=(densityM * MMM->triangleGeo->computeRestTriangleArea(triangleAdded[i]))/(typename DataTypes::Real)6.0;
            }

            // Adding mass
            for (unsigned int j=0; j<3; ++j)
                VertexMasses[ t[j] ] += mass;
        }
    }
}


/// Creation fonction for mass stored on edges
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::EdgeMassTriangleCreationFunction(const sofa::helper::vector<unsigned int> &triangleAdded,
        void* param, vector<MassType> &EdgeMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_TRIANGLESET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<triangleAdded.size(); ++i)
        {
            // Get the edgesInTriangle to be added
            const EdgesInTriangle &te = MMM->_topology->getEdgesInTriangle(triangleAdded[i]);

            // Compute rest mass of conserne triangle = density * triangle surface.
            if(MMM->triangleGeo)
            {
                mass=(densityM * MMM->triangleGeo->computeRestTriangleArea(triangleAdded[i]))/(typename DataTypes::Real)12.0;
            }

            // Adding mass edges of concerne triangle
            for (unsigned int j=0; j<3; ++j)
                EdgeMasses[ te[j] ] += mass;
        }
    }
}


/// Destruction fonction for mass stored on vertices
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::VertexMassTriangleDestroyFunction(const sofa::helper::vector<unsigned int> &triangleRemoved,
        void* param, vector<MassType> &VertexMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_TRIANGLESET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<triangleRemoved.size(); ++i)
        {
            // Get the triangle to be removed
            const Triangle &t = MMM->_topology->getTriangle(triangleRemoved[i]);

            // Compute rest mass of conserne triangle = density * triangle surface.
            if(MMM->triangleGeo)
            {
                mass=(densityM * MMM->triangleGeo->computeRestTriangleArea(triangleRemoved[i]))/(typename DataTypes::Real)6.0;
            }

            // Removing mass
            for (unsigned int j=0; j<3; ++j)
                VertexMasses[ t[j] ] -= mass;
        }
    }
}


/// Destruction fonction for mass stored on edges
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::EdgeMassTriangleDestroyFunction(const sofa::helper::vector<unsigned int> &triangleRemoved,
        void* param, vector<MassType> &EdgeMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_TRIANGLESET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<triangleRemoved.size(); ++i)
        {
            // Get the triangle to be removed
            const EdgesInTriangle &te = MMM->_topology->getEdgesInTriangle(triangleRemoved[i]);

            // Compute rest mass of conserne triangle = density * triangle surface.
            if(MMM->triangleGeo)
            {
                mass=(densityM * MMM->triangleGeo->computeRestTriangleArea(triangleRemoved[i]))/(typename DataTypes::Real)12.0;
            }

            // Removing mass edges of concerne triangle
            for (unsigned int j=0; j<3; ++j)
                EdgeMasses[ te[j] ] -= mass;
        }
    }
}

// }



// ---------------------------------------------------
// ------- Quad Creation/Destruction functions -------
// ---------------------------------------------------
//{

/// Creation fonction for mass stored on vertices
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::VertexMassQuadCreationFunction(const sofa::helper::vector<unsigned int> &quadAdded,
        void* param, vector<MassType> &VertexMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_QUADSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<quadAdded.size(); ++i)
        {
            // Get the quad to be added
            const Quad &q = MMM->_topology->getQuad(quadAdded[i]);

            // Compute rest mass of conserne quad = density * quad surface.
            if(MMM->quadGeo)
            {
                mass=(densityM * MMM->quadGeo->computeRestQuadArea(quadAdded[i]))/(typename DataTypes::Real)8.0;
            }

            // Adding mass
            for (unsigned int j=0; j<4; ++j)
                VertexMasses[ q[j] ] += mass;
        }
    }
}


/// Creation fonction for mass stored on edges
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::EdgeMassQuadCreationFunction(const sofa::helper::vector<unsigned int> &quadAdded,
        void* param, vector<MassType> &EdgeMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_QUADSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<quadAdded.size(); ++i)
        {
            // Get the EdgesInQuad to be added
            const EdgesInQuad &qe = MMM->_topology->getEdgesInQuad(quadAdded[i]);

            // Compute rest mass of conserne quad = density * quad surface.
            if(MMM->quadGeo)
            {
                mass=(densityM * MMM->quadGeo->computeRestQuadArea(quadAdded[i]))/(typename DataTypes::Real)16.0;
            }

            // Adding mass edges of concerne quad
            for (unsigned int j=0; j<4; ++j)
                EdgeMasses[ qe[j] ] += mass;
        }
    }
}


/// Destruction fonction for mass stored on vertices
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::VertexMassQuadDestroyFunction(const sofa::helper::vector<unsigned int> &quadRemoved,
        void* param, vector<MassType> &VertexMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_QUADSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<quadRemoved.size(); ++i)
        {
            // Get the quad to be removed
            const Quad &q = MMM->_topology->getQuad(quadRemoved[i]);

            // Compute rest mass of conserne quad = density * quad surface.
            if(MMM->quadGeo)
            {
                mass=(densityM * MMM->quadGeo->computeRestQuadArea(quadRemoved[i]))/(typename DataTypes::Real)8.0;
            }

            // Removing mass
            for (unsigned int j=0; j<4; ++j)
                VertexMasses[ q[j] ] -= mass;
        }
    }
}


/// Destruction fonction for mass stored on edges
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::EdgeMassQuadDestroyFunction(const sofa::helper::vector<unsigned int> &quadRemoved,
        void* param, vector<MassType> &EdgeMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_QUADSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<quadRemoved.size(); ++i)
        {
            // Get the EdgesInQuad to be removed
            const EdgesInQuad &qe = MMM->_topology->getEdgesInQuad(quadRemoved[i]);

            // Compute rest mass of conserne quad = density * quad surface.
            if(MMM->quadGeo)
            {
                mass=(densityM * MMM->quadGeo->computeRestQuadArea(quadRemoved[i]))/(typename DataTypes::Real)16.0;
            }

            // Removing mass edges of concerne quad
            for (unsigned int j=0; j<4; ++j)
                EdgeMasses[ qe[j] ] -= mass/2;
        }
    }
}

// }



// ----------------------------------------------------------
// ------- Tetrahedron Creation/Destruction functions -------
// ----------------------------------------------------------
//{

/// Creation fonction for mass stored on vertices
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::VertexMassTetrahedronCreationFunction(const sofa::helper::vector<unsigned int> &tetrahedronAdded,
        void* param, vector<MassType> &VertexMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_TETRAHEDRONSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<tetrahedronAdded.size(); ++i)
        {
            // Get the tetrahedron to be added
            const Tetrahedron &t = MMM->_topology->getTetrahedron(tetrahedronAdded[i]);

            // Compute rest mass of conserne tetrahedron = density * tetrahedron volume.
            if(MMM->tetraGeo)
            {
                mass=(densityM * MMM->tetraGeo->computeRestTetrahedronVolume(tetrahedronAdded[i]))/(typename DataTypes::Real)10.0;
            }

            // Adding mass
            for (unsigned int j=0; j<4; ++j)
                VertexMasses[ t[j] ] += mass;
        }
    }
}


/// Creation fonction for mass stored on edges
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::EdgeMassTetrahedronCreationFunction(const sofa::helper::vector<unsigned int> &tetrahedronAdded,
        void* param, vector<MassType> &EdgeMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_TETRAHEDRONSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<tetrahedronAdded.size(); ++i)
        {
            // Get the edgesInTetrahedron to be added
            const EdgesInTetrahedron &te = MMM->_topology->getEdgesInTetrahedron(tetrahedronAdded[i]);

            // Compute rest mass of conserne triangle = density * tetrahedron volume.
            if(MMM->tetraGeo)
            {
                mass=(densityM * MMM->tetraGeo->computeRestTetrahedronVolume(tetrahedronAdded[i]))/(typename DataTypes::Real)20.0;
            }

            // Adding mass edges of concerne triangle
            for (unsigned int j=0; j<6; ++j)
                EdgeMasses[ te[j] ] += mass;
        }
    }
}


/// Destruction fonction for mass stored on vertices
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::VertexMassTetrahedronDestroyFunction(const sofa::helper::vector<unsigned int> &tetrahedronRemoved,
        void* param, vector<MassType> &VertexMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_TETRAHEDRONSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<tetrahedronRemoved.size(); ++i)
        {
            // Get the tetrahedron to be removed
            const Tetrahedron &t = MMM->_topology->getTetrahedron(tetrahedronRemoved[i]);

            // Compute rest mass of conserne tetrahedron = density * tetrahedron volume.
            if(MMM->tetraGeo)
            {
                mass=(densityM * MMM->tetraGeo->computeRestTetrahedronVolume(tetrahedronRemoved[i]))/(typename DataTypes::Real)10.0;
            }

            // Removing mass
            for (unsigned int j=0; j<4; ++j)
                VertexMasses[ t[j] ] -= mass;
        }
    }
}


/// Destruction fonction for mass stored on edges
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::EdgeMassTetrahedronDestroyFunction(const sofa::helper::vector<unsigned int> &tetrahedronRemoved,
        void* param, vector<MassType> &EdgeMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_TETRAHEDRONSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<tetrahedronRemoved.size(); ++i)
        {
            // Get the edgesInTetrahedron to be removed
            const EdgesInTetrahedron &te = MMM->_topology->getEdgesInTetrahedron(tetrahedronRemoved[i]);

            // Compute rest mass of conserne triangle = density * tetrahedron volume.
            if(MMM->tetraGeo)
            {
                mass=(densityM * MMM->tetraGeo->computeRestTetrahedronVolume(tetrahedronRemoved[i]))/(typename DataTypes::Real)20.0;
            }

            // Removing mass edges of concerne triangle
            for (unsigned int j=0; j<6; ++j)
                EdgeMasses[ te[j] ] -= mass; //?
        }
    }
}

// }


// ---------------------------------------------------------
// ------- Hexahedron Creation/Destruction functions -------
// ---------------------------------------------------------
//{

/// Creation fonction for mass stored on vertices
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::VertexMassHexahedronCreationFunction(const sofa::helper::vector<unsigned int> &hexahedronAdded,
        void* param, vector<MassType> &VertexMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_HEXAHEDRONSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<hexahedronAdded.size(); ++i)
        {
            // Get the hexahedron to be added
            const Hexahedron &h = MMM->_topology->getHexahedron(hexahedronAdded[i]);

            // Compute rest mass of conserne hexahedron = density * hexahedron volume.
            if(MMM->hexaGeo)
            {
                mass=(densityM * MMM->hexaGeo->computeRestHexahedronVolume(hexahedronAdded[i]))/(typename DataTypes::Real)20.0;
            }

            // Adding mass
            for (unsigned int j=0; j<8; ++j)
                VertexMasses[ h[j] ] += mass;
        }
    }
}


/// Creation fonction for mass stored on edges
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::EdgeMassHexahedronCreationFunction(const sofa::helper::vector<unsigned int> &hexahedronAdded,
        void* param, vector<MassType> &EdgeMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_HEXAHEDRONSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<hexahedronAdded.size(); ++i)
        {
            // Get the EdgesInHexahedron to be added
            const EdgesInHexahedron &he = MMM->_topology->getEdgesInHexahedron(hexahedronAdded[i]);

            // Compute rest mass of conserne hexahedron = density * hexahedron volume.
            if(MMM->hexaGeo)
            {
                mass=(densityM * MMM->hexaGeo->computeRestHexahedronVolume(hexahedronAdded[i]))/(typename DataTypes::Real)40.0;
            }

            // Adding mass edges of concerne triangle
            for (unsigned int j=0; j<12; ++j)
                EdgeMasses[ he[j] ] += mass;
        }
    }
}


/// Destruction fonction for mass stored on vertices
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::VertexMassHexahedronDestroyFunction(const sofa::helper::vector<unsigned int> &hexahedronRemoved,
        void* param, vector<MassType> &VertexMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_HEXAHEDRONSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<hexahedronRemoved.size(); ++i)
        {
            // Get the hexahedron to be removed
            const Hexahedron &h = MMM->_topology->getHexahedron(hexahedronRemoved[i]);

            // Compute rest mass of conserne hexahedron = density * hexahedron volume.
            if(MMM->hexaGeo)
            {
                mass=(densityM * MMM->hexaGeo->computeRestHexahedronVolume(hexahedronRemoved[i]))/(typename DataTypes::Real)20.0;
            }

            // Removing mass
            for (unsigned int j=0; j<8; ++j)
                VertexMasses[ h[j] ] -= mass;
        }
    }
}


/// Destruction fonction for mass stored on edges
template< class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::EdgeMassHexahedronDestroyFunction(const sofa::helper::vector<unsigned int> &hexahedronRemoved,
        void* param, vector<MassType> &EdgeMasses)
{
    MeshMatrixMass<DataTypes, MassType> *MMM = (MeshMatrixMass<DataTypes, MassType> *)param;

    if (MMM && MMM->getMassTopologyType()==MeshMatrixMass<DataTypes, MassType>::TOPOLOGY_HEXAHEDRONSET)
    {
        // Initialisation
        typename DataTypes::Real densityM = MMM->getMassDensity();
        typename DataTypes::Real mass = (typename DataTypes::Real) 0;

        for (unsigned int i = 0; i<hexahedronRemoved.size(); ++i)
        {
            // Get the EdgesInHexahedron to be removed
            const EdgesInHexahedron &he = MMM->_topology->getEdgesInHexahedron(hexahedronRemoved[i]);

            // Compute rest mass of conserne hexahedron = density * hexahedron volume.
            if(MMM->hexaGeo)
            {
                mass=(densityM * MMM->hexaGeo->computeRestHexahedronVolume(hexahedronRemoved[i]))/(typename DataTypes::Real)40.0;
            }

            // Removing mass edges of concerne triangle
            for (unsigned int j=0; j<12; ++j)
                EdgeMasses[ he[j] ] -= mass;
        }
    }
}

// }




using namespace sofa::defaulttype;
using namespace sofa::core::componentmodel::behavior;


template <class DataTypes, class MassType>
MeshMatrixMass<DataTypes, MassType>::MeshMatrixMass()
    : vertexMassInfo( initData(&vertexMassInfo, "vertexMass", "values of the particles masses on vertices") )
    , edgeMassInfo( initData(&edgeMassInfo, "edgeMass", "values of the particles masses on edges") )
    , m_massDensity( initData(&m_massDensity, (Real)1.0,"massDensity", "mass density that allows to compute the  particles masses from a mesh topology and geometry.\nOnly used if > 0") )
    , showCenterOfGravity( initData(&showCenterOfGravity, false, "showGravityCenter", "display the center of gravity of the system" ) )
    , showAxisSize( initData(&showAxisSize, 1.0f, "showAxisSizeFactor", "factor length of the axis displayed (only used for rigids)" ) )
    , topologyType(TOPOLOGY_UNKNOWN)
{

}


template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::init()
{
    /*  using sofa::component::topology::RegularGridTopology;
    RegularGridTopology* reg = dynamic_cast<RegularGridTopology*>( this->getContext()->getMeshTopology() );
    if( reg != NULL )
    {
    Real weight = reg->getDx().norm() * reg->getDy().norm() * reg->getDz().norm() * m_massDensity.getValue()/8;
    VecMass& m = *f_mass.beginEdit();
    for( int i=0; i<reg->getNx()-1; i++ )
    {
    for( int j=0; j<reg->getNy()-1; j++ )
    {
        for( int k=0; k<reg->getNz()-1; k++ )
        {
    m[reg->point(i,j,k)] += weight;
    m[reg->point(i,j,k+1)] += weight;
    m[reg->point(i,j+1,k)] += weight;
    m[reg->point(i,j+1,k+1)] += weight;
    m[reg->point(i+1,j,k)] += weight;
    m[reg->point(i+1,j,k+1)] += weight;
    m[reg->point(i+1,j+1,k)] += weight;
    m[reg->point(i+1,j+1,k+1)] += weight;
        }
    }
    }
    f_mass.endEdit();
    }*/

    this->Inherited::init();

    _topology = this->getContext()->getMeshTopology();
    //    sofa::core::objectmodel::Tag mechanicalTag(m_tagMeshMechanics.getValue());
    //    this->getContext()->get(triangleGeo, mechanicalTag,sofa::core::objectmodel::BaseContext::SearchUp);

    this->getContext()->get(edgeGeo);
    this->getContext()->get(triangleGeo);
    this->getContext()->get(quadGeo);
    this->getContext()->get(tetraGeo);
    this->getContext()->get(hexaGeo);

    // add the functions to handle topology changes for Vertex informations
    vertexMassInfo.setCreateFunction(VertexMassCreationFunction);
    vertexMassInfo.setCreateParameter ((void *) this );
    // Triangle
    vertexMassInfo.setCreateTriangleFunction(VertexMassTriangleCreationFunction);
    vertexMassInfo.setDestroyTriangleFunction(VertexMassTriangleDestroyFunction);
    // Quad
    vertexMassInfo.setCreateQuadFunction(VertexMassQuadCreationFunction);
    vertexMassInfo.setDestroyQuadFunction(VertexMassQuadDestroyFunction);
    // Tetrahedron
    vertexMassInfo.setCreateTetrahedronFunction(VertexMassTetrahedronCreationFunction);
    vertexMassInfo.setDestroyTetrahedronFunction(VertexMassTetrahedronDestroyFunction);
    // Hexahedron (NOT HANDLE YET)
    //vertexMassInfo.setCreateHexahedronFunction(VertexMassHexahedronCreationFunction);
    //vertexMassInfo.setDestroyHexahedronFunction(VertexMassHexahedronDestroyFunction);


    // add the functions to handle topology changes for Edge informations
    edgeMassInfo.setCreateFunction(EdgeMassCreationFunction);
    edgeMassInfo.setCreateParameter ((void *) this );
    // Triangle
    edgeMassInfo.setCreateTriangleFunction(EdgeMassTriangleCreationFunction);
    edgeMassInfo.setDestroyTriangleFunction(EdgeMassTriangleDestroyFunction);
    // Quad
    edgeMassInfo.setCreateQuadFunction(EdgeMassQuadCreationFunction);
    edgeMassInfo.setDestroyQuadFunction(EdgeMassQuadDestroyFunction);
    // Tetrahedron
    edgeMassInfo.setCreateTetrahedronFunction(EdgeMassTetrahedronCreationFunction);
    edgeMassInfo.setDestroyTetrahedronFunction(EdgeMassTetrahedronDestroyFunction);
    // Hexahedron (NOT HANDLE YET)
    //edgeMassInfo.setCreateHexahedronFunction(EdgeMassHexahedronCreationFunction);
    //edgeMassInfo.setDestroyHexahedronFunction(EdgeMassHexahedronDestroyFunction);

    if ((vertexMassInfo.getValue().size()==0 || edgeMassInfo.getValue().size()==0) && (_topology!=0))
    {
        reinit();
    }
}

template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::reinit()
{

    if (_topology && (m_massDensity.getValue() > 0 && (vertexMassInfo.getValue().size() == 0 || edgeMassInfo.getValue().size() == 0)))
    {
        // resize array
        clear();

        /// prepare to store info in the vertex array
        vector<MassType>& my_vertexMassInfo = *vertexMassInfo.beginEdit();
        vector<MassType>& my_edgeMassInfo = *edgeMassInfo.beginEdit();

        unsigned int ndof = this->mstate->getSize();
        unsigned int nbEdges=_topology->getNbEdges();

        my_vertexMassInfo.resize(ndof);
        my_edgeMassInfo.resize(nbEdges);

        // set vertex tensor to 0
        for (unsigned int i = 0; i<ndof; ++i)
            VertexMassCreationFunction (i, (void*) this, my_vertexMassInfo[i],
                    (const sofa::helper::vector< unsigned int > )0,
                    (const sofa::helper::vector< double >)0);

        // set edge tensor to 0
        for (unsigned int i = 0; i<nbEdges; ++i)
            EdgeMassCreationFunction (i, (void*) this, my_edgeMassInfo[i], _topology->getEdge(i),
                    (const sofa::helper::vector< unsigned int > )0,
                    (const sofa::helper::vector< double >)0);


        // Create mass matrix depending on current Topology:
        if (_topology->getNbHexahedra()>0 && hexaGeo)  // Hexahedron topology
        {
            // create vector tensor by calling the hexahedron creation function on the entire mesh
            sofa::helper::vector<unsigned int> hexahedraAdded;
            setMassTopologyType(TOPOLOGY_HEXAHEDRONSET);

            for (int i = 0; i<_topology->getNbHexahedra(); ++i)
                hexahedraAdded.push_back(i);

            VertexMassHexahedronCreationFunction(hexahedraAdded, (void*) this, my_vertexMassInfo);
            EdgeMassHexahedronCreationFunction(hexahedraAdded, (void*) this, my_edgeMassInfo);
        }
        else if (_topology->getNbTetrahedra()>0 && tetraGeo)  // Tetrahedron topology
        {
            // create vector tensor by calling the tetrahedron creation function on the entire mesh
            sofa::helper::vector<unsigned int> tetrahedraAdded;
            setMassTopologyType(TOPOLOGY_TETRAHEDRONSET);

            for (int i = 0; i<_topology->getNbTetrahedra(); ++i)
                tetrahedraAdded.push_back(i);

            VertexMassTetrahedronCreationFunction(tetrahedraAdded, (void*) this, my_vertexMassInfo);
            EdgeMassTetrahedronCreationFunction(tetrahedraAdded, (void*) this, my_edgeMassInfo);
        }
        else if (_topology->getNbQuads()>0 && quadGeo)  // Quad topology
        {
            // create vector tensor by calling the quad creation function on the entire mesh
            sofa::helper::vector<unsigned int> quadsAdded;
            setMassTopologyType(TOPOLOGY_QUADSET);

            for (int i = 0; i<_topology->getNbQuads(); ++i)
                quadsAdded.push_back(i);

            VertexMassQuadCreationFunction(quadsAdded, (void*) this, my_vertexMassInfo);
            EdgeMassQuadCreationFunction(quadsAdded, (void*) this, my_edgeMassInfo);
        }
        else if (_topology->getNbTriangles()>0 && triangleGeo) // Triangle topology
        {
            // create vector tensor by calling the triangle creation function on the entire mesh
            sofa::helper::vector<unsigned int> trianglesAdded;
            setMassTopologyType(TOPOLOGY_TRIANGLESET);

            for (int i = 0; i<_topology->getNbTriangles(); ++i)
                trianglesAdded.push_back(i);

            VertexMassTriangleCreationFunction(trianglesAdded, (void*) this, my_vertexMassInfo);
            EdgeMassTriangleCreationFunction(trianglesAdded, (void*) this, my_edgeMassInfo);
        }
    }
}


template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=_topology->firstChange();
    std::list<const TopologyChange *>::const_iterator itEnd=_topology->lastChange();

    vertexMassInfo.handleTopologyEvents(itBegin,itEnd);
    edgeMassInfo.handleTopologyEvents(itBegin,itEnd);
}



template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::clear()
{
    MassVector& vertexMass = *vertexMassInfo.beginEdit();
    MassVector& edgeMass = *edgeMassInfo.beginEdit();
    vertexMass.clear();
    edgeMass.clear();
    vertexMassInfo.endEdit();
    edgeMassInfo.endEdit();
}


// -- Mass interface
template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::addMDx(VecDeriv& res, const VecDeriv& dx, double factor)
{

    const MassVector &vertexMass= vertexMassInfo.getValue();
    const MassVector &edgeMass= edgeMassInfo.getValue();

    unsigned int nbEdges=_topology->getNbEdges();
    unsigned int v0,v1;

    if (factor == 1.0)
    {
        for (unsigned int i=0; i<dx.size(); i++)
        {
            res[i] += dx[i] * vertexMass[i];
        }

        for (unsigned int i=0; i<nbEdges; ++i)
        {
            v0=_topology->getEdge(i)[0];
            v1=_topology->getEdge(i)[1];

            res[v0] += dx[v0] * edgeMass[i];
            res[v1] += dx[v1] * edgeMass[i];
        }
    }
    else
    {
        for (unsigned int i=0; i<dx.size(); i++)
        {
            res[i] += (dx[i] * vertexMass[i]) * (Real)factor;
        }

        for (unsigned int i=0; i<nbEdges; ++i)
        {
            v0=_topology->getEdge(i)[0];
            v1=_topology->getEdge(i)[1];

            res[v0] += (dx[v0] * edgeMass[i]) * (Real)factor;
            res[v1] += (dx[v1] * edgeMass[i]) * (Real)factor;
        }
    }
}



template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::accFromF(VecDeriv& a, const VecDeriv& f)
{
    (void)a;
    (void)f;
    serr << "WARNING: the methode 'accFromF' can't be used with MeshMatrixMass as this mass matrix can't be inversed easily." << sendl;
    return;
}


template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& v)
{
    //if gravity was added separately (in solver's "solve" method), then nothing to do here
    if(this->m_separateGravity.getValue())
        return;

    const MassVector &vertexMass= vertexMassInfo.getValue();
    const MassVector &edgeMass= edgeMassInfo.getValue();

    unsigned int nbEdges=_topology->getNbEdges();
    unsigned int v0,v1;


    // gravity
    Vec3d g ( this->getContext()->getLocalGravity() );
    Deriv theGravity;
    DataTypes::set ( theGravity, g[0], g[1], g[2]);

    // velocity-based stuff
    core::objectmodel::BaseContext::SpatialVector vframe = this->getContext()->getVelocityInWorld();
    core::objectmodel::BaseContext::Vec3 aframe = this->getContext()->getVelocityBasedLinearAccelerationInWorld() ;

    // project back to local frame
    vframe = this->getContext()->getPositionInWorld() / vframe;
    aframe = this->getContext()->getPositionInWorld().backProjectVector( aframe );

    // add weight and inertia force
    for (unsigned int i=0; i<x.size(); ++i)
    {
        f[i] += theGravity * vertexMass[i] + core::componentmodel::behavior::inertiaForce(vframe,aframe,vertexMass[i],x[i],v[i]);
    }

    for (unsigned int i=0; i<nbEdges; ++i)
    {
        v0=_topology->getEdge(i)[0];
        v1=_topology->getEdge(i)[1];

        f[v0] += theGravity*edgeMass[i] + core::componentmodel::behavior::inertiaForce(vframe,aframe,edgeMass[i],x[v0],v[v0]);
        f[v1] += theGravity*edgeMass[i] + core::componentmodel::behavior::inertiaForce(vframe,aframe,edgeMass[i],x[v1],v[v1]);
    }
}



template <class DataTypes, class MassType>
double MeshMatrixMass<DataTypes, MassType>::getKineticEnergy( const VecDeriv& v )
{

    const MassVector &vertexMass= vertexMassInfo.getValue();
    const MassVector &edgeMass= edgeMassInfo.getValue();

    unsigned int nbEdges=_topology->getNbEdges();
    unsigned int v0,v1;

    double e = 0;

    for (unsigned int i=0; i<v.size(); i++)
    {
        e += v[i]*vertexMass[i]*v[i]; // v[i]*v[i]*masses[i] would be more efficient but less generic
    }

    for (unsigned int i=0; i<nbEdges; ++i)
    {
        v0=_topology->getEdge(i)[0];
        v1=_topology->getEdge(i)[1];

        e += v[v0]*edgeMass[i]*v[v0];
        e += v[v1]*edgeMass[i]*v[v1];
    }

    return e/2;
}


template <class DataTypes, class MassType>
double MeshMatrixMass<DataTypes, MassType>::getPotentialEnergy( const VecCoord& x )
{

    const MassVector &vertexMass= vertexMassInfo.getValue();
    const MassVector &edgeMass= edgeMassInfo.getValue();

    unsigned int nbEdges=_topology->getNbEdges();
    unsigned int v0,v1;

    SReal e = 0;
    // gravity
    Vec3d g ( this->getContext()->getLocalGravity() );
    Deriv theGravity;
    DataTypes::set ( theGravity, g[0], g[1], g[2]);

    for (unsigned int i=0; i<x.size(); i++)
    {
        e -= theGravity*vertexMass[i]*x[i];
    }

    for (unsigned int i=0; i<nbEdges; ++i)
    {
        v0=_topology->getEdge(i)[0];
        v1=_topology->getEdge(i)[1];

        e -= theGravity*edgeMass[i]*x[v0];
        e -= theGravity*edgeMass[i]*x[v1];
    }

    return e;
}



template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::addGravityToV(double dt)
{
    if(this->mstate)
    {
        VecDeriv& v = *this->mstate->getV();

        // gravity
        Vec3d g ( this->getContext()->getLocalGravity() );
        Deriv theGravity;
        DataTypes::set ( theGravity, g[0], g[1], g[2]);
        Deriv hg = theGravity * (typename DataTypes::Real)dt;

        for (unsigned int i=0; i<v.size(); i++)
        {
            v[i] += hg;
        }
    }
}




template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::addMToMatrix(defaulttype::BaseMatrix * mat, double mFact, unsigned int &offset)
{
    const MassVector &vertexMass= vertexMassInfo.getValue();
    const MassVector &edgeMass= edgeMassInfo.getValue();

    unsigned int nbEdges=_topology->getNbEdges();
    unsigned int v0,v1;

    const int N = defaulttype::DataTypeInfo<Deriv>::size();
    AddMToMatrixFunctor<Deriv,MassType> calc;

    for (unsigned int i=0; i<vertexMass.size(); i++)
        calc(mat, vertexMass[i], offset + N*i, mFact);

    for (unsigned int i=0; i<nbEdges; ++i)
    {
        v0=_topology->getEdge(i)[0];
        v1=_topology->getEdge(i)[1];

        calc(mat, edgeMass[i], offset + N*v0, mFact);
        calc(mat, edgeMass[i], offset + N*v1, mFact);
    }
}




template <class DataTypes, class MassType>
double MeshMatrixMass<DataTypes, MassType>::getElementMass(unsigned int index) const
{
    //return (SReal)(f_mass.getValue()[index]);
    (void)index;
    serr << "WARNING: the methode 'getElementMass' can't be used with MeshMatrixMass as this mass matrix is stored on two different Data." << sendl;
    return 0.0;
}


//TODO: special case for Rigid Mass
template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::getElementMass(unsigned int index, defaulttype::BaseMatrix *m) const
{
    /*
    const unsigned int dimension = defaulttype::DataTypeInfo<Deriv>::size();
    if (m->rowSize() != dimension || m->colSize() != dimension) m->resize(dimension,dimension);

    m->clear();
    AddMToMatrixFunctor<Deriv,MassType>()(m, f_mass.getValue()[index], 0, 1);
    */
    (void)index;
    (void)m;
    serr << "WARNING: the methode 'getElementMass' can't be used with MeshMatrixMass as this mass matrix is stored on two different Data." << sendl;
}




template <class DataTypes, class MassType>
void MeshMatrixMass<DataTypes, MassType>::draw()
{
    if (!this->getContext()->getShowBehaviorModels()) return;

    const MassVector &vertexMass= vertexMassInfo.getValue();
    const MassVector &edgeMass= edgeMassInfo.getValue();

    unsigned int nbEdges=_topology->getNbEdges();
    unsigned int v0,v1;

    const VecCoord& x = *this->mstate->getX();
    Coord gravityCenter;
    Real totalMass=0.0;

    std::vector<  Vector3 > points;
    std::vector< Vec<2,int> > indices;

    for (unsigned int i=0; i<x.size(); i++)
    {
        Vector3 p;
        for (unsigned int j=0; j< Coord::static_size; ++j)
            p[j] = x[i][j];

        points.push_back(p);
        gravityCenter += x[i]*vertexMass[i];
        totalMass += vertexMass[i];
    }

    for (unsigned int i=0; i<nbEdges; ++i)
    {
        v0=_topology->getEdge(i)[0];
        v1=_topology->getEdge(i)[1];

        gravityCenter += x[v0]*edgeMass[v0];
        gravityCenter += x[v1]*edgeMass[v1];
        totalMass += edgeMass[v0];
        totalMass += edgeMass[v1];
    }


    simulation::getSimulation()->DrawUtility.drawPoints(points, 2, Vec<4,float>(1,1,1,1));

    if(showCenterOfGravity.getValue())
    {
        glBegin (GL_LINES);
        glColor4f (1,1,0,1);
        glPointSize(5);
        gravityCenter /= totalMass;
        for(unsigned int i=0 ; i<Coord::static_size ; i++)
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
bool MeshMatrixMass<DataTypes, MassType>::addBBox(double* minBBox, double* maxBBox)
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


} // namespace mass

} // namespace component

} // namespace sofa

#endif
