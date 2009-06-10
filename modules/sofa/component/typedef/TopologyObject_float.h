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



// File automatically generated by "generateTypedef"


#ifndef SOFA_TYPEDEF_TopologyObject_float_H
#define SOFA_TYPEDEF_TopologyObject_float_H

//Default files containing the declaration of the vector type
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/Mat.h>



#include <sofa/component/topology/DynamicSparseGridGeometryAlgorithms.h>
#include <sofa/component/topology/DynamicSparseGridTopologyAlgorithms.h>
#include <sofa/component/topology/EdgeSetGeometryAlgorithms.h>
#include <sofa/component/topology/EdgeSetTopologyAlgorithms.h>
#include <sofa/component/topology/HexahedronSetGeometryAlgorithms.h>
#include <sofa/component/topology/HexahedronSetTopologyAlgorithms.h>
#include <sofa/component/topology/ManifoldEdgeSetGeometryAlgorithms.h>
#include <sofa/component/topology/ManifoldEdgeSetTopologyAlgorithms.h>
#include <sofa/component/topology/ManifoldTriangleSetTopologyAlgorithms.h>
#include <sofa/component/topology/PointSetGeometryAlgorithms.h>
#include <sofa/component/topology/PointSetTopologyAlgorithms.h>
#include <sofa/component/topology/QuadSetGeometryAlgorithms.h>
#include <sofa/component/topology/QuadSetTopologyAlgorithms.h>
#include <sofa/component/topology/TetrahedronSetGeometryAlgorithms.h>
#include <sofa/component/topology/TetrahedronSetTopologyAlgorithms.h>
#include <sofa/component/topology/TriangleSetGeometryAlgorithms.h>
#include <sofa/component/topology/TriangleSetTopologyAlgorithms.h>



//---------------------------------------------------------------------------------------------
//Typedef for DynamicSparseGridGeometryAlgorithms
typedef sofa::component::topology::DynamicSparseGridGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > DynamicSparseGridGeometryAlgorithms3f;
typedef sofa::component::topology::DynamicSparseGridGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > DynamicSparseGridGeometryAlgorithms1f;
typedef sofa::component::topology::DynamicSparseGridGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > DynamicSparseGridGeometryAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for DynamicSparseGridTopologyAlgorithms
typedef sofa::component::topology::DynamicSparseGridTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > DynamicSparseGridTopologyAlgorithms3f;
typedef sofa::component::topology::DynamicSparseGridTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > DynamicSparseGridTopologyAlgorithms1f;
typedef sofa::component::topology::DynamicSparseGridTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > DynamicSparseGridTopologyAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for EdgeSetGeometryAlgorithms
typedef sofa::component::topology::EdgeSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > EdgeSetGeometryAlgorithms3f;
typedef sofa::component::topology::EdgeSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > EdgeSetGeometryAlgorithms1f;
typedef sofa::component::topology::EdgeSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > EdgeSetGeometryAlgorithms2f;
typedef sofa::component::topology::EdgeSetGeometryAlgorithms<sofa::defaulttype::StdRigidTypes<3, float> > EdgeSetGeometryAlgorithmsRigid3f;
typedef sofa::component::topology::EdgeSetGeometryAlgorithms<sofa::defaulttype::StdRigidTypes<2, float> > EdgeSetGeometryAlgorithmsRigid2f;



//---------------------------------------------------------------------------------------------
//Typedef for EdgeSetTopologyAlgorithms
typedef sofa::component::topology::EdgeSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > EdgeSetTopologyAlgorithms3f;
typedef sofa::component::topology::EdgeSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > EdgeSetTopologyAlgorithms1f;
typedef sofa::component::topology::EdgeSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > EdgeSetTopologyAlgorithms2f;
typedef sofa::component::topology::EdgeSetTopologyAlgorithms<sofa::defaulttype::StdRigidTypes<3, float> > EdgeSetTopologyAlgorithmsRigid3f;
typedef sofa::component::topology::EdgeSetTopologyAlgorithms<sofa::defaulttype::StdRigidTypes<2, float> > EdgeSetTopologyAlgorithmsRigid2f;



//---------------------------------------------------------------------------------------------
//Typedef for HexahedronSetGeometryAlgorithms
typedef sofa::component::topology::HexahedronSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > HexahedronSetGeometryAlgorithms3f;
typedef sofa::component::topology::HexahedronSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > HexahedronSetGeometryAlgorithms1f;
typedef sofa::component::topology::HexahedronSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > HexahedronSetGeometryAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for HexahedronSetTopologyAlgorithms
typedef sofa::component::topology::HexahedronSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > HexahedronSetTopologyAlgorithms3f;
typedef sofa::component::topology::HexahedronSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > HexahedronSetTopologyAlgorithms1f;
typedef sofa::component::topology::HexahedronSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > HexahedronSetTopologyAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for ManifoldEdgeSetGeometryAlgorithms
typedef sofa::component::topology::ManifoldEdgeSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > ManifoldEdgeSetGeometryAlgorithms3f;
typedef sofa::component::topology::ManifoldEdgeSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > ManifoldEdgeSetGeometryAlgorithms1f;
typedef sofa::component::topology::ManifoldEdgeSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > ManifoldEdgeSetGeometryAlgorithms2f;
typedef sofa::component::topology::ManifoldEdgeSetGeometryAlgorithms<sofa::defaulttype::StdRigidTypes<3, float> > ManifoldEdgeSetGeometryAlgorithmsRigid3f;
typedef sofa::component::topology::ManifoldEdgeSetGeometryAlgorithms<sofa::defaulttype::StdRigidTypes<2, float> > ManifoldEdgeSetGeometryAlgorithmsRigid2f;



//---------------------------------------------------------------------------------------------
//Typedef for ManifoldEdgeSetTopologyAlgorithms
typedef sofa::component::topology::ManifoldEdgeSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > ManifoldEdgeSetTopologyAlgorithms3f;
typedef sofa::component::topology::ManifoldEdgeSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > ManifoldEdgeSetTopologyAlgorithms1f;
typedef sofa::component::topology::ManifoldEdgeSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > ManifoldEdgeSetTopologyAlgorithms2f;
typedef sofa::component::topology::ManifoldEdgeSetTopologyAlgorithms<sofa::defaulttype::StdRigidTypes<3, float> > ManifoldEdgeSetTopologyAlgorithmsRigid3f;
typedef sofa::component::topology::ManifoldEdgeSetTopologyAlgorithms<sofa::defaulttype::StdRigidTypes<2, float> > ManifoldEdgeSetTopologyAlgorithmsRigid2f;



//---------------------------------------------------------------------------------------------
//Typedef for ManifoldTriangleSetTopologyAlgorithms
typedef sofa::component::topology::ManifoldTriangleSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > ManifoldTriangleSetTopologyAlgorithms3f;
typedef sofa::component::topology::ManifoldTriangleSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > ManifoldTriangleSetTopologyAlgorithms1f;
typedef sofa::component::topology::ManifoldTriangleSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > ManifoldTriangleSetTopologyAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for PointSetGeometryAlgorithms
typedef sofa::component::topology::PointSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > PointSetGeometryAlgorithms3f;
typedef sofa::component::topology::PointSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > PointSetGeometryAlgorithms1f;
typedef sofa::component::topology::PointSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > PointSetGeometryAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for PointSetTopologyAlgorithms
typedef sofa::component::topology::PointSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > PointSetTopologyAlgorithms3f;
typedef sofa::component::topology::PointSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > PointSetTopologyAlgorithms1f;
typedef sofa::component::topology::PointSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > PointSetTopologyAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for QuadSetGeometryAlgorithms
typedef sofa::component::topology::QuadSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > QuadSetGeometryAlgorithms3f;
typedef sofa::component::topology::QuadSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > QuadSetGeometryAlgorithms1f;
typedef sofa::component::topology::QuadSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > QuadSetGeometryAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for QuadSetTopologyAlgorithms
typedef sofa::component::topology::QuadSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > QuadSetTopologyAlgorithms3f;
typedef sofa::component::topology::QuadSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > QuadSetTopologyAlgorithms1f;
typedef sofa::component::topology::QuadSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > QuadSetTopologyAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for TetrahedronSetGeometryAlgorithms
typedef sofa::component::topology::TetrahedronSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > TetrahedronSetGeometryAlgorithms3f;
typedef sofa::component::topology::TetrahedronSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > TetrahedronSetGeometryAlgorithms1f;
typedef sofa::component::topology::TetrahedronSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > TetrahedronSetGeometryAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for TetrahedronSetTopologyAlgorithms
typedef sofa::component::topology::TetrahedronSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > TetrahedronSetTopologyAlgorithms3f;
typedef sofa::component::topology::TetrahedronSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > TetrahedronSetTopologyAlgorithms1f;
typedef sofa::component::topology::TetrahedronSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > TetrahedronSetTopologyAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for TriangleSetGeometryAlgorithms
typedef sofa::component::topology::TriangleSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > TriangleSetGeometryAlgorithms3f;
typedef sofa::component::topology::TriangleSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > TriangleSetGeometryAlgorithms1f;
typedef sofa::component::topology::TriangleSetGeometryAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > TriangleSetGeometryAlgorithms2f;



//---------------------------------------------------------------------------------------------
//Typedef for TriangleSetTopologyAlgorithms
typedef sofa::component::topology::TriangleSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > TriangleSetTopologyAlgorithms3f;
typedef sofa::component::topology::TriangleSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, float>, sofa::defaulttype::Vec<2, float>, float> > TriangleSetTopologyAlgorithms1f;
typedef sofa::component::topology::TriangleSetTopologyAlgorithms<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, float>, sofa::defaulttype::Vec<1, float>, float> > TriangleSetTopologyAlgorithms2f;





#ifdef SOFA_FLOAT
typedef DynamicSparseGridGeometryAlgorithms3f DynamicSparseGridGeometryAlgorithms3;
typedef DynamicSparseGridGeometryAlgorithms1f DynamicSparseGridGeometryAlgorithms1;
typedef DynamicSparseGridGeometryAlgorithms2f DynamicSparseGridGeometryAlgorithms2;
typedef DynamicSparseGridTopologyAlgorithms3f DynamicSparseGridTopologyAlgorithms3;
typedef DynamicSparseGridTopologyAlgorithms1f DynamicSparseGridTopologyAlgorithms1;
typedef DynamicSparseGridTopologyAlgorithms2f DynamicSparseGridTopologyAlgorithms2;
typedef EdgeSetGeometryAlgorithms3f EdgeSetGeometryAlgorithms3;
typedef EdgeSetGeometryAlgorithms1f EdgeSetGeometryAlgorithms1;
typedef EdgeSetGeometryAlgorithms2f EdgeSetGeometryAlgorithms2;
typedef EdgeSetGeometryAlgorithmsRigid3f EdgeSetGeometryAlgorithmsRigid3;
typedef EdgeSetGeometryAlgorithmsRigid2f EdgeSetGeometryAlgorithmsRigid2;
typedef EdgeSetTopologyAlgorithms3f EdgeSetTopologyAlgorithms3;
typedef EdgeSetTopologyAlgorithms1f EdgeSetTopologyAlgorithms1;
typedef EdgeSetTopologyAlgorithms2f EdgeSetTopologyAlgorithms2;
typedef EdgeSetTopologyAlgorithmsRigid3f EdgeSetTopologyAlgorithmsRigid3;
typedef EdgeSetTopologyAlgorithmsRigid2f EdgeSetTopologyAlgorithmsRigid2;
typedef HexahedronSetGeometryAlgorithms3f HexahedronSetGeometryAlgorithms3;
typedef HexahedronSetGeometryAlgorithms1f HexahedronSetGeometryAlgorithms1;
typedef HexahedronSetGeometryAlgorithms2f HexahedronSetGeometryAlgorithms2;
typedef HexahedronSetTopologyAlgorithms3f HexahedronSetTopologyAlgorithms3;
typedef HexahedronSetTopologyAlgorithms1f HexahedronSetTopologyAlgorithms1;
typedef HexahedronSetTopologyAlgorithms2f HexahedronSetTopologyAlgorithms2;
typedef ManifoldEdgeSetGeometryAlgorithms3f ManifoldEdgeSetGeometryAlgorithms3;
typedef ManifoldEdgeSetGeometryAlgorithms1f ManifoldEdgeSetGeometryAlgorithms1;
typedef ManifoldEdgeSetGeometryAlgorithms2f ManifoldEdgeSetGeometryAlgorithms2;
typedef ManifoldEdgeSetGeometryAlgorithmsRigid3f ManifoldEdgeSetGeometryAlgorithmsRigid3;
typedef ManifoldEdgeSetGeometryAlgorithmsRigid2f ManifoldEdgeSetGeometryAlgorithmsRigid2;
typedef ManifoldEdgeSetTopologyAlgorithms3f ManifoldEdgeSetTopologyAlgorithms3;
typedef ManifoldEdgeSetTopologyAlgorithms1f ManifoldEdgeSetTopologyAlgorithms1;
typedef ManifoldEdgeSetTopologyAlgorithms2f ManifoldEdgeSetTopologyAlgorithms2;
typedef ManifoldEdgeSetTopologyAlgorithmsRigid3f ManifoldEdgeSetTopologyAlgorithmsRigid3;
typedef ManifoldEdgeSetTopologyAlgorithmsRigid2f ManifoldEdgeSetTopologyAlgorithmsRigid2;
typedef ManifoldTriangleSetTopologyAlgorithms3f ManifoldTriangleSetTopologyAlgorithms3;
typedef ManifoldTriangleSetTopologyAlgorithms1f ManifoldTriangleSetTopologyAlgorithms1;
typedef ManifoldTriangleSetTopologyAlgorithms2f ManifoldTriangleSetTopologyAlgorithms2;
typedef PointSetGeometryAlgorithms3f PointSetGeometryAlgorithms3;
typedef PointSetGeometryAlgorithms1f PointSetGeometryAlgorithms1;
typedef PointSetGeometryAlgorithms2f PointSetGeometryAlgorithms2;
typedef PointSetTopologyAlgorithms3f PointSetTopologyAlgorithms3;
typedef PointSetTopologyAlgorithms1f PointSetTopologyAlgorithms1;
typedef PointSetTopologyAlgorithms2f PointSetTopologyAlgorithms2;
typedef QuadSetGeometryAlgorithms3f QuadSetGeometryAlgorithms3;
typedef QuadSetGeometryAlgorithms1f QuadSetGeometryAlgorithms1;
typedef QuadSetGeometryAlgorithms2f QuadSetGeometryAlgorithms2;
typedef QuadSetTopologyAlgorithms3f QuadSetTopologyAlgorithms3;
typedef QuadSetTopologyAlgorithms1f QuadSetTopologyAlgorithms1;
typedef QuadSetTopologyAlgorithms2f QuadSetTopologyAlgorithms2;
typedef TetrahedronSetGeometryAlgorithms3f TetrahedronSetGeometryAlgorithms3;
typedef TetrahedronSetGeometryAlgorithms1f TetrahedronSetGeometryAlgorithms1;
typedef TetrahedronSetGeometryAlgorithms2f TetrahedronSetGeometryAlgorithms2;
typedef TetrahedronSetTopologyAlgorithms3f TetrahedronSetTopologyAlgorithms3;
typedef TetrahedronSetTopologyAlgorithms1f TetrahedronSetTopologyAlgorithms1;
typedef TetrahedronSetTopologyAlgorithms2f TetrahedronSetTopologyAlgorithms2;
typedef TriangleSetGeometryAlgorithms3f TriangleSetGeometryAlgorithms3;
typedef TriangleSetGeometryAlgorithms1f TriangleSetGeometryAlgorithms1;
typedef TriangleSetGeometryAlgorithms2f TriangleSetGeometryAlgorithms2;
typedef TriangleSetTopologyAlgorithms3f TriangleSetTopologyAlgorithms3;
typedef TriangleSetTopologyAlgorithms1f TriangleSetTopologyAlgorithms1;
typedef TriangleSetTopologyAlgorithms2f TriangleSetTopologyAlgorithms2;
#endif

#endif
