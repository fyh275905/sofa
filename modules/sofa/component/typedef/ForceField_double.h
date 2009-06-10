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


#ifndef SOFA_TYPEDEF_ForceField_double_H
#define SOFA_TYPEDEF_ForceField_double_H

//Default files containing the declaration of the vector type
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/Mat.h>



#include <sofa/component/forcefield/BeamFEMForceField.h>
#include <sofa/component/forcefield/BoxStiffSpringForceField.h>
#include <sofa/component/forcefield/ConicalForceField.h>
#include <sofa/component/forcefield/ConstantForceField.h>
#include <sofa/component/forcefield/DistanceGridForceField.h>
#include <sofa/component/forcefield/EdgePressureForceField.h>
#include <sofa/component/forcefield/EllipsoidForceField.h>
#include <sofa/component/forcefield/FrameSpringForceField.h>
#include <sofa/component/forcefield/HexahedralFEMForceField.h>
#include <sofa/component/forcefield/HexahedronFEMForceField.h>
#include <sofa/component/interactionforcefield/InteractionEllipsoidForceField.h>
#include <sofa/component/forcefield/JointSpringForceField.h>
#include <sofa/component/interactionforcefield/LagrangeMultiplierInteraction.h>
#include <sofa/component/forcefield/LennardJonesForceField.h>
#include <sofa/component/forcefield/MeshSpringForceField.h>
#include <sofa/component/forcefield/PenalityContactForceField.h>
#include <sofa/component/forcefield/PlaneForceField.h>
#include <sofa/component/forcefield/QuadBendingSprings.h>
#include <sofa/component/forcefield/QuadularBendingSprings.h>
#include <sofa/component/forcefield/RegularGridSpringForceField.h>
#include <sofa/component/interactionforcefield/RepulsiveSpringForceField.h>
#include <sofa/component/forcefield/RestShapeSpringsForceField.h>
#include <sofa/component/forcefield/SPHFluidForceField.h>
#include <sofa/component/forcefield/ShapeMatchingForceField.h>
#include <sofa/component/forcefield/SphereForceField.h>
#include <sofa/component/forcefield/SpringForceField.h>
#include <sofa/component/forcefield/StiffSpringForceField.h>
#include <sofa/component/forcefield/SurfacePressureForceField.h>
#include <sofa/component/forcefield/TetrahedralBiquadraticSpringsForceField.h>
#include <sofa/component/forcefield/TetrahedralCorotationalFEMForceField.h>
#include <sofa/component/forcefield/TetrahedralQuadraticSpringsForceField.h>
#include <sofa/component/forcefield/TetrahedralTensorMassForceField.h>
#include <sofa/component/forcefield/TetrahedronFEMForceField.h>
#include <sofa/component/fem/forcefield/Triangle2DFEMForceField.h>
#include <sofa/component/fem/forcefield/TriangleBendingFEMForceField.h>
#include <sofa/component/forcefield/TriangleBendingSprings.h>
#include <sofa/component/forcefield/TriangleFEMForceField.h>
#include <sofa/component/forcefield/TrianglePressureForceField.h>
#include <sofa/component/forcefield/TriangularAnisotropicFEMForceField.h>
#include <sofa/component/forcefield/TriangularBendingSprings.h>
#include <sofa/component/forcefield/TriangularBiquadraticSpringsForceField.h>
#include <sofa/component/forcefield/TriangularFEMForceField.h>
#include <sofa/component/forcefield/TriangularQuadraticSpringsForceField.h>
#include <sofa/component/forcefield/TriangularTensorMassForceField.h>
#include <sofa/component/forcefield/VaccumSphereForceField.h>
#include <sofa/component/forcefield/VectorSpringForceField.h>
#include <sofa/component/forcefield/WashingMachineForceField.h>



//---------------------------------------------------------------------------------------------
//Typedef for BeamFEMForceField
typedef sofa::component::forcefield::BeamFEMForceField<sofa::defaulttype::StdRigidTypes<3, double> > BeamFEMForceFieldRigid3d;



//---------------------------------------------------------------------------------------------
//Typedef for BoxStiffSpringForceField
typedef sofa::component::forcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > BoxStiffSpringForceField3d;
typedef sofa::component::forcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > BoxStiffSpringForceField1d;
typedef sofa::component::forcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > BoxStiffSpringForceField2d;
typedef sofa::component::forcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > BoxStiffSpringForceField6d;



//---------------------------------------------------------------------------------------------
//Typedef for ConicalForceField
typedef sofa::component::forcefield::ConicalForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > ConicalForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for ConstantForceField
typedef sofa::component::forcefield::ConstantForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > ConstantForceField3d;
typedef sofa::component::forcefield::ConstantForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > ConstantForceField1d;
typedef sofa::component::forcefield::ConstantForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > ConstantForceField2d;
typedef sofa::component::forcefield::ConstantForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > ConstantForceField6d;
typedef sofa::component::forcefield::ConstantForceField<sofa::defaulttype::StdRigidTypes<3, double> > ConstantForceFieldRigid3d;
typedef sofa::component::forcefield::ConstantForceField<sofa::defaulttype::StdRigidTypes<2, double> > ConstantForceFieldRigid2d;



//---------------------------------------------------------------------------------------------
//Typedef for DistanceGridForceField
typedef sofa::component::forcefield::DistanceGridForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > DistanceGridForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for EdgePressureForceField
typedef sofa::component::forcefield::EdgePressureForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > EdgePressureForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for EllipsoidForceField
typedef sofa::component::forcefield::EllipsoidForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > EllipsoidForceField3d;
typedef sofa::component::forcefield::EllipsoidForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > EllipsoidForceField1d;
typedef sofa::component::forcefield::EllipsoidForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > EllipsoidForceField2d;



//---------------------------------------------------------------------------------------------
//Typedef for FrameSpringForceField
typedef sofa::component::forcefield::FrameSpringForceField<sofa::defaulttype::StdRigidTypes<3, double> > FrameSpringForceFieldRigid3d;



//---------------------------------------------------------------------------------------------
//Typedef for HexahedralFEMForceField
typedef sofa::component::forcefield::HexahedralFEMForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > HexahedralFEMForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for HexahedronFEMForceField
typedef sofa::component::forcefield::HexahedronFEMForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > HexahedronFEMForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for InteractionEllipsoidForceField
typedef sofa::component::interactionforcefield::InteractionEllipsoidForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double>, sofa::defaulttype::StdRigidTypes<3, double> > InteractionEllipsoidForceField3d_Rigid3d;



//---------------------------------------------------------------------------------------------
//Typedef for JointSpringForceField
typedef sofa::component::forcefield::JointSpringForceField<sofa::defaulttype::StdRigidTypes<3, double> > JointSpringForceFieldRigid3d;



//---------------------------------------------------------------------------------------------
//Typedef for LagrangeMultiplierInteraction
typedef sofa::component::interactionforcefield::LagrangeMultiplierInteraction<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double>, sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > LagrangeMultiplierInteraction2d_3d;



//---------------------------------------------------------------------------------------------
//Typedef for LennardJonesForceField
typedef sofa::component::forcefield::LennardJonesForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > LennardJonesForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for MeshSpringForceField
typedef sofa::component::forcefield::MeshSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > MeshSpringForceField3d;
typedef sofa::component::forcefield::MeshSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > MeshSpringForceField1d;
typedef sofa::component::forcefield::MeshSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > MeshSpringForceField2d;



//---------------------------------------------------------------------------------------------
//Typedef for PenalityContactForceField
typedef sofa::component::forcefield::PenalityContactForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > PenalityContactForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for PlaneForceField
typedef sofa::component::forcefield::PlaneForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > PlaneForceField3d;
typedef sofa::component::forcefield::PlaneForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > PlaneForceField1d;
typedef sofa::component::forcefield::PlaneForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > PlaneForceField2d;
typedef sofa::component::forcefield::PlaneForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > PlaneForceField6d;



//---------------------------------------------------------------------------------------------
//Typedef for QuadBendingSprings
typedef sofa::component::forcefield::QuadBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > QuadBendingSprings3d;
typedef sofa::component::forcefield::QuadBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > QuadBendingSprings1d;



//---------------------------------------------------------------------------------------------
//Typedef for QuadularBendingSprings
typedef sofa::component::forcefield::QuadularBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > QuadularBendingSprings3d;



//---------------------------------------------------------------------------------------------
//Typedef for RegularGridSpringForceField
typedef sofa::component::forcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > RegularGridSpringForceField3d;
typedef sofa::component::forcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > RegularGridSpringForceField1d;
typedef sofa::component::forcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > RegularGridSpringForceField2d;
typedef sofa::component::forcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > RegularGridSpringForceField6d;



//---------------------------------------------------------------------------------------------
//Typedef for RepulsiveSpringForceField
typedef sofa::component::interactionforcefield::RepulsiveSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > RepulsiveSpringForceField3d;
typedef sofa::component::interactionforcefield::RepulsiveSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > RepulsiveSpringForceField1d;
typedef sofa::component::interactionforcefield::RepulsiveSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > RepulsiveSpringForceField2d;



//---------------------------------------------------------------------------------------------
//Typedef for RestShapeSpringsForceField
typedef sofa::component::forcefield::RestShapeSpringsForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > RestShapeSpringsForceField3d;
typedef sofa::component::forcefield::RestShapeSpringsForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > RestShapeSpringsForceField2d;



//---------------------------------------------------------------------------------------------
//Typedef for SPHFluidForceField
typedef sofa::component::forcefield::SPHFluidForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > SPHFluidForceField3d;
typedef sofa::component::forcefield::SPHFluidForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > SPHFluidForceField1d;



//---------------------------------------------------------------------------------------------
//Typedef for ShapeMatchingForceField
typedef sofa::component::forcefield::ShapeMatchingForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > ShapeMatchingForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for SphereForceField
typedef sofa::component::forcefield::SphereForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > SphereForceField3d;
typedef sofa::component::forcefield::SphereForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > SphereForceField1d;
typedef sofa::component::forcefield::SphereForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > SphereForceField2d;



//---------------------------------------------------------------------------------------------
//Typedef for SpringForceField
typedef sofa::component::forcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > SpringForceField3d;
typedef sofa::component::forcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > SpringForceField1d;
typedef sofa::component::forcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > SpringForceField2d;
typedef sofa::component::forcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > SpringForceField6d;



//---------------------------------------------------------------------------------------------
//Typedef for StiffSpringForceField
typedef sofa::component::forcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > StiffSpringForceField3d;
typedef sofa::component::forcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > StiffSpringForceField1d;
typedef sofa::component::forcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > StiffSpringForceField2d;
typedef sofa::component::forcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > StiffSpringForceField6d;



//---------------------------------------------------------------------------------------------
//Typedef for SurfacePressureForceField
typedef sofa::component::forcefield::SurfacePressureForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > SurfacePressureForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TetrahedralBiquadraticSpringsForceField
typedef sofa::component::forcefield::TetrahedralBiquadraticSpringsForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TetrahedralBiquadraticSpringsForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TetrahedralCorotationalFEMForceField
typedef sofa::component::forcefield::TetrahedralCorotationalFEMForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TetrahedralCorotationalFEMForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TetrahedralQuadraticSpringsForceField
typedef sofa::component::forcefield::TetrahedralQuadraticSpringsForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TetrahedralQuadraticSpringsForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TetrahedralTensorMassForceField
typedef sofa::component::forcefield::TetrahedralTensorMassForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TetrahedralTensorMassForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TetrahedronFEMForceField
typedef sofa::component::forcefield::TetrahedronFEMForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TetrahedronFEMForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for Triangle2DFEMForceField
typedef sofa::component::fem::forcefield::Triangle2DFEMForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > Triangle2DFEMForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TriangleBendingFEMForceField
typedef sofa::component::fem::forcefield::TriangleBendingFEMForceField<sofa::defaulttype::StdRigidTypes<3, double> > TriangleBendingFEMForceFieldRigid3d;



//---------------------------------------------------------------------------------------------
//Typedef for TriangleBendingSprings
typedef sofa::component::forcefield::TriangleBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TriangleBendingSprings3d;
typedef sofa::component::forcefield::TriangleBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > TriangleBendingSprings1d;



//---------------------------------------------------------------------------------------------
//Typedef for TriangleFEMForceField
typedef sofa::component::forcefield::TriangleFEMForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TriangleFEMForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TrianglePressureForceField
typedef sofa::component::forcefield::TrianglePressureForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TrianglePressureForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TriangularAnisotropicFEMForceField
typedef sofa::component::forcefield::TriangularAnisotropicFEMForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TriangularAnisotropicFEMForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TriangularBendingSprings
typedef sofa::component::forcefield::TriangularBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TriangularBendingSprings3d;



//---------------------------------------------------------------------------------------------
//Typedef for TriangularBiquadraticSpringsForceField
typedef sofa::component::forcefield::TriangularBiquadraticSpringsForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TriangularBiquadraticSpringsForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TriangularFEMForceField
typedef sofa::component::forcefield::TriangularFEMForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TriangularFEMForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TriangularQuadraticSpringsForceField
typedef sofa::component::forcefield::TriangularQuadraticSpringsForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TriangularQuadraticSpringsForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for TriangularTensorMassForceField
typedef sofa::component::forcefield::TriangularTensorMassForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TriangularTensorMassForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for VaccumSphereForceField
typedef sofa::component::forcefield::VaccumSphereForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > VaccumSphereForceField3d;
typedef sofa::component::forcefield::VaccumSphereForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > VaccumSphereForceField1d;
typedef sofa::component::forcefield::VaccumSphereForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > VaccumSphereForceField2d;



//---------------------------------------------------------------------------------------------
//Typedef for VectorSpringForceField
typedef sofa::component::forcefield::VectorSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > VectorSpringForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for WashingMachineForceField
typedef sofa::component::forcefield::WashingMachineForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > WashingMachineForceField3d;





#ifndef SOFA_FLOAT
typedef BeamFEMForceFieldRigid3d BeamFEMForceFieldRigid3;
typedef BoxStiffSpringForceField3d BoxStiffSpringForceField3;
typedef BoxStiffSpringForceField1d BoxStiffSpringForceField1;
typedef BoxStiffSpringForceField2d BoxStiffSpringForceField2;
typedef BoxStiffSpringForceField6d BoxStiffSpringForceField6;
typedef ConicalForceField3d ConicalForceField3;
typedef ConstantForceField3d ConstantForceField3;
typedef ConstantForceField1d ConstantForceField1;
typedef ConstantForceField2d ConstantForceField2;
typedef ConstantForceField6d ConstantForceField6;
typedef ConstantForceFieldRigid3d ConstantForceFieldRigid3;
typedef ConstantForceFieldRigid2d ConstantForceFieldRigid2;
typedef DistanceGridForceField3d DistanceGridForceField3;
typedef EdgePressureForceField3d EdgePressureForceField3;
typedef EllipsoidForceField3d EllipsoidForceField3;
typedef EllipsoidForceField1d EllipsoidForceField1;
typedef EllipsoidForceField2d EllipsoidForceField2;
typedef FrameSpringForceFieldRigid3d FrameSpringForceFieldRigid3;
typedef HexahedralFEMForceField3d HexahedralFEMForceField3;
typedef HexahedronFEMForceField3d HexahedronFEMForceField3;
typedef InteractionEllipsoidForceField3d_Rigid3d InteractionEllipsoidForceField3_Rigid3;
typedef JointSpringForceFieldRigid3d JointSpringForceFieldRigid3;
typedef LagrangeMultiplierInteraction2d_3d LagrangeMultiplierInteraction2_3;
typedef LennardJonesForceField3d LennardJonesForceField3;
typedef MeshSpringForceField3d MeshSpringForceField3;
typedef MeshSpringForceField1d MeshSpringForceField1;
typedef MeshSpringForceField2d MeshSpringForceField2;
typedef PenalityContactForceField3d PenalityContactForceField3;
typedef PlaneForceField3d PlaneForceField3;
typedef PlaneForceField1d PlaneForceField1;
typedef PlaneForceField2d PlaneForceField2;
typedef PlaneForceField6d PlaneForceField6;
typedef QuadBendingSprings3d QuadBendingSprings3;
typedef QuadBendingSprings1d QuadBendingSprings1;
typedef QuadularBendingSprings3d QuadularBendingSprings3;
typedef RegularGridSpringForceField3d RegularGridSpringForceField3;
typedef RegularGridSpringForceField1d RegularGridSpringForceField1;
typedef RegularGridSpringForceField2d RegularGridSpringForceField2;
typedef RegularGridSpringForceField6d RegularGridSpringForceField6;
typedef RepulsiveSpringForceField3d RepulsiveSpringForceField3;
typedef RepulsiveSpringForceField1d RepulsiveSpringForceField1;
typedef RepulsiveSpringForceField2d RepulsiveSpringForceField2;
typedef RestShapeSpringsForceField3d RestShapeSpringsForceField3;
typedef RestShapeSpringsForceField2d RestShapeSpringsForceField2;
typedef SPHFluidForceField3d SPHFluidForceField3;
typedef SPHFluidForceField1d SPHFluidForceField1;
typedef ShapeMatchingForceField3d ShapeMatchingForceField3;
typedef SphereForceField3d SphereForceField3;
typedef SphereForceField1d SphereForceField1;
typedef SphereForceField2d SphereForceField2;
typedef SpringForceField3d SpringForceField3;
typedef SpringForceField1d SpringForceField1;
typedef SpringForceField2d SpringForceField2;
typedef SpringForceField6d SpringForceField6;
typedef StiffSpringForceField3d StiffSpringForceField3;
typedef StiffSpringForceField1d StiffSpringForceField1;
typedef StiffSpringForceField2d StiffSpringForceField2;
typedef StiffSpringForceField6d StiffSpringForceField6;
typedef SurfacePressureForceField3d SurfacePressureForceField3;
typedef TetrahedralBiquadraticSpringsForceField3d TetrahedralBiquadraticSpringsForceField3;
typedef TetrahedralCorotationalFEMForceField3d TetrahedralCorotationalFEMForceField3;
typedef TetrahedralQuadraticSpringsForceField3d TetrahedralQuadraticSpringsForceField3;
typedef TetrahedralTensorMassForceField3d TetrahedralTensorMassForceField3;
typedef TetrahedronFEMForceField3d TetrahedronFEMForceField3;
typedef Triangle2DFEMForceField3d Triangle2DFEMForceField3;
typedef TriangleBendingFEMForceFieldRigid3d TriangleBendingFEMForceFieldRigid3;
typedef TriangleBendingSprings3d TriangleBendingSprings3;
typedef TriangleBendingSprings1d TriangleBendingSprings1;
typedef TriangleFEMForceField3d TriangleFEMForceField3;
typedef TrianglePressureForceField3d TrianglePressureForceField3;
typedef TriangularAnisotropicFEMForceField3d TriangularAnisotropicFEMForceField3;
typedef TriangularBendingSprings3d TriangularBendingSprings3;
typedef TriangularBiquadraticSpringsForceField3d TriangularBiquadraticSpringsForceField3;
typedef TriangularFEMForceField3d TriangularFEMForceField3;
typedef TriangularQuadraticSpringsForceField3d TriangularQuadraticSpringsForceField3;
typedef TriangularTensorMassForceField3d TriangularTensorMassForceField3;
typedef VaccumSphereForceField3d VaccumSphereForceField3;
typedef VaccumSphereForceField1d VaccumSphereForceField1;
typedef VaccumSphereForceField2d VaccumSphereForceField2;
typedef VectorSpringForceField3d VectorSpringForceField3;
typedef WashingMachineForceField3d WashingMachineForceField3;
#endif

#endif
