/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
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


#ifndef SOFA_TYPEDEF_InteractionForceField_double_H
#define SOFA_TYPEDEF_InteractionForceField_double_H

//Default files containing the declaration of the vector type
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/Mat.h>


#ifdef SOFA_GPU_CUDA
#include <sofa/gpu/cuda/CudaTypesBase.h>
#include <sofa/gpu/cuda/CudaTypes.h>
#endif
#ifdef SOFA_GPU_OPENCL
#include <sofa/gpu/opencl/OpenCLTypes.h>
#endif


#include <sofa/component/interactionforcefield/BoxStiffSpringForceField.h>
#include <sofa/gpu/cuda/CudaLDIPenalityContactForceField.h>
#include <sofa/component/interactionforcefield/FrameSpringForceField.h>
#include <sofa/component/interactionforcefield/InteractionEllipsoidForceField.h>
#include <sofa/component/interactionforcefield/JointSpringForceField.h>
#include <sofa/component/interactionforcefield/MeshSpringForceField.h>
#include <sofa/component/interactionforcefield/PenalityContactForceField.h>
#include <sofa/component/interactionforcefield/QuadBendingSprings.h>
#include <sofa/component/interactionforcefield/RegularGridSpringForceField.h>
#include <sofa/component/interactionforcefield/RepulsiveSpringForceField.h>
#include <sofa/component/interactionforcefield/SpringForceField.h>
#include <sofa/component/interactionforcefield/StiffSpringForceField.h>
#include <sofa/component/interactionforcefield/TriangleBendingSprings.h>
#include <sofa/component/interactionforcefield/VectorSpringForceField.h>



//---------------------------------------------------------------------------------------------
//Typedef for BoxStiffSpringForceField
typedef sofa::component::interactionforcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > BoxStiffSpringForceField1d;
typedef sofa::component::interactionforcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > BoxStiffSpringForceField2d;
typedef sofa::component::interactionforcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > BoxStiffSpringForceField3d;
typedef sofa::component::interactionforcefield::BoxStiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > BoxStiffSpringForceField6d;



//---------------------------------------------------------------------------------------------
//Typedef for CudaLDIPenalityContactForceField
typedef sofa::gpu::cuda::CudaLDIPenalityContactForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double>, sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double>, sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > CudaLDIPenalityContactForceField3d_3d;



//---------------------------------------------------------------------------------------------
//Typedef for FrameSpringForceField
typedef sofa::component::interactionforcefield::FrameSpringForceField<sofa::defaulttype::StdRigidTypes<3, double> > FrameSpringForceFieldRigid3d;



//---------------------------------------------------------------------------------------------
//Typedef for InteractionEllipsoidForceField
typedef sofa::component::interactionforcefield::InteractionEllipsoidForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double>, sofa::defaulttype::StdRigidTypes<3, double> > InteractionEllipsoidForceField3d_Rigid3d;



//---------------------------------------------------------------------------------------------
//Typedef for JointSpringForceField
typedef sofa::component::interactionforcefield::JointSpringForceField<sofa::defaulttype::StdRigidTypes<3, double> > JointSpringForceFieldRigid3d;



//---------------------------------------------------------------------------------------------
//Typedef for MeshSpringForceField
typedef sofa::component::interactionforcefield::MeshSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > MeshSpringForceField1d;
typedef sofa::component::interactionforcefield::MeshSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > MeshSpringForceField2d;
typedef sofa::component::interactionforcefield::MeshSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > MeshSpringForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for PenalityContactForceField
typedef sofa::component::interactionforcefield::PenalityContactForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > PenalityContactForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for QuadBendingSprings
typedef sofa::component::interactionforcefield::QuadBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > QuadBendingSprings2d;
typedef sofa::component::interactionforcefield::QuadBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > QuadBendingSprings3d;



//---------------------------------------------------------------------------------------------
//Typedef for RegularGridSpringForceField
typedef sofa::component::interactionforcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > RegularGridSpringForceField1d;
typedef sofa::component::interactionforcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > RegularGridSpringForceField2d;
typedef sofa::component::interactionforcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > RegularGridSpringForceField3d;
typedef sofa::component::interactionforcefield::RegularGridSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > RegularGridSpringForceField6d;



//---------------------------------------------------------------------------------------------
//Typedef for RepulsiveSpringForceField
typedef sofa::component::interactionforcefield::RepulsiveSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > RepulsiveSpringForceField1d;
typedef sofa::component::interactionforcefield::RepulsiveSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > RepulsiveSpringForceField2d;
typedef sofa::component::interactionforcefield::RepulsiveSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > RepulsiveSpringForceField3d;



//---------------------------------------------------------------------------------------------
//Typedef for SpringForceField
typedef sofa::component::interactionforcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > SpringForceField1d;
typedef sofa::component::interactionforcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > SpringForceField2d;
typedef sofa::component::interactionforcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > SpringForceField3d;
typedef sofa::component::interactionforcefield::SpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > SpringForceField6d;



//---------------------------------------------------------------------------------------------
//Typedef for StiffSpringForceField
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<1, double>, sofa::defaulttype::Vec<1, double>, double> > StiffSpringForceField1d;
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > StiffSpringForceField2d;
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > StiffSpringForceField3d;
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<6, double>, sofa::defaulttype::Vec<6, double>, double> > StiffSpringForceField6d;



//---------------------------------------------------------------------------------------------
//Typedef for TriangleBendingSprings
typedef sofa::component::interactionforcefield::TriangleBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<2, double>, sofa::defaulttype::Vec<2, double>, double> > TriangleBendingSprings2d;
typedef sofa::component::interactionforcefield::TriangleBendingSprings<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > TriangleBendingSprings3d;



//---------------------------------------------------------------------------------------------
//Typedef for VectorSpringForceField
typedef sofa::component::interactionforcefield::VectorSpringForceField<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, double>, sofa::defaulttype::Vec<3, double>, double> > VectorSpringForceField3d;





#ifndef SOFA_FLOAT
typedef BoxStiffSpringForceField1d BoxStiffSpringForceField1;
typedef BoxStiffSpringForceField2d BoxStiffSpringForceField2;
typedef BoxStiffSpringForceField3d BoxStiffSpringForceField3;
typedef BoxStiffSpringForceField6d BoxStiffSpringForceField6;
typedef CudaLDIPenalityContactForceField3d_3d CudaLDIPenalityContactForceField3_3;
typedef FrameSpringForceFieldRigid3d FrameSpringForceFieldRigid3;
typedef InteractionEllipsoidForceField3d_Rigid3d InteractionEllipsoidForceField3_Rigid3;
typedef JointSpringForceFieldRigid3d JointSpringForceFieldRigid3;
typedef MeshSpringForceField1d MeshSpringForceField1;
typedef MeshSpringForceField2d MeshSpringForceField2;
typedef MeshSpringForceField3d MeshSpringForceField3;
typedef PenalityContactForceField3d PenalityContactForceField3;
typedef QuadBendingSprings2d QuadBendingSprings2;
typedef QuadBendingSprings3d QuadBendingSprings3;
typedef RegularGridSpringForceField1d RegularGridSpringForceField1;
typedef RegularGridSpringForceField2d RegularGridSpringForceField2;
typedef RegularGridSpringForceField3d RegularGridSpringForceField3;
typedef RegularGridSpringForceField6d RegularGridSpringForceField6;
typedef RepulsiveSpringForceField1d RepulsiveSpringForceField1;
typedef RepulsiveSpringForceField2d RepulsiveSpringForceField2;
typedef RepulsiveSpringForceField3d RepulsiveSpringForceField3;
typedef SpringForceField1d SpringForceField1;
typedef SpringForceField2d SpringForceField2;
typedef SpringForceField3d SpringForceField3;
typedef SpringForceField6d SpringForceField6;
typedef StiffSpringForceField1d StiffSpringForceField1;
typedef StiffSpringForceField2d StiffSpringForceField2;
typedef StiffSpringForceField3d StiffSpringForceField3;
typedef StiffSpringForceField6d StiffSpringForceField6;
typedef TriangleBendingSprings2d TriangleBendingSprings2;
typedef TriangleBendingSprings3d TriangleBendingSprings3;
typedef VectorSpringForceField3d VectorSpringForceField3;
#endif

#endif
