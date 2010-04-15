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
#include "OpenCLTypes.h"
#include "OpenCLSpringForceField.inl"
#include <sofa/component/forcefield/BoxStiffSpringForceField.inl>
#include <sofa/core/ObjectFactory.h>

#define DEBUG_TEXT(t) printf("\t%s\t %s %d\n",t,__FILE__,__LINE__);

namespace sofa
{

namespace component
{

namespace forcefield
{

template class SpringForceField<sofa::gpu::opencl::OpenCLVec3fTypes>;
template class StiffSpringForceField<sofa::gpu::opencl::OpenCLVec3fTypes>;
template class MeshSpringForceField<sofa::gpu::opencl::OpenCLVec3fTypes>;
template class BoxStiffSpringForceField<gpu::opencl::OpenCLVec3fTypes>;

template class SpringForceField<sofa::gpu::opencl::OpenCLVec3f1Types>;
template class StiffSpringForceField<sofa::gpu::opencl::OpenCLVec3f1Types>;
template class MeshSpringForceField<sofa::gpu::opencl::OpenCLVec3f1Types>;
template class BoxStiffSpringForceField<gpu::opencl::OpenCLVec3f1Types>;

template class SpringForceField<sofa::gpu::opencl::OpenCLVec3dTypes>;
template class StiffSpringForceField<sofa::gpu::opencl::OpenCLVec3dTypes>;
template class MeshSpringForceField<sofa::gpu::opencl::OpenCLVec3dTypes>;
template class BoxStiffSpringForceField<gpu::opencl::OpenCLVec3dTypes>;

template class SpringForceField<sofa::gpu::opencl::OpenCLVec3d1Types>;
template class StiffSpringForceField<sofa::gpu::opencl::OpenCLVec3d1Types>;
template class MeshSpringForceField<sofa::gpu::opencl::OpenCLVec3d1Types>;
template class BoxStiffSpringForceField<gpu::opencl::OpenCLVec3d1Types>;

} // namespace forcefield

} // namespace component

namespace gpu
{

namespace opencl
{

SOFA_DECL_CLASS(OpenCLSpringForceField)
SOFA_DECL_CLASS(OpenCLBoxStiffSpringForceField)

int SpringForceFieldOpenCLClass = core::RegisterObject("Supports GPU-side computations using OPENCL")
        .add< component::forcefield::SpringForceField<OpenCLVec3fTypes> >()
        .add< component::forcefield::SpringForceField<OpenCLVec3f1Types> >()
        .add< component::forcefield::SpringForceField<OpenCLVec3dTypes> >()
        .add< component::forcefield::SpringForceField<OpenCLVec3d1Types> >()
        ;

int StiffSpringForceFieldOpenCLClass = core::RegisterObject("Supports GPU-side computations using OPENCL")
        .add< component::forcefield::StiffSpringForceField<OpenCLVec3fTypes> >()
        .add< component::forcefield::StiffSpringForceField<OpenCLVec3f1Types> >()
        .add< component::forcefield::StiffSpringForceField<OpenCLVec3dTypes> >()
        .add< component::forcefield::StiffSpringForceField<OpenCLVec3d1Types> >()
        ;

int MeshSpringForceFieldOpenCLClass = core::RegisterObject("Supports GPU-side computations using OPENCL")
        .add< component::forcefield::MeshSpringForceField<OpenCLVec3fTypes> >()
        .add< component::forcefield::MeshSpringForceField<OpenCLVec3f1Types> >()
        .add< component::forcefield::MeshSpringForceField<OpenCLVec3dTypes> >()
        .add< component::forcefield::MeshSpringForceField<OpenCLVec3d1Types> >()
        ;

int TriangleBendingSpringsOpenCLClass = core::RegisterObject("Supports GPU-side computations using OPENCL")
        .add< component::forcefield::TriangleBendingSprings<OpenCLVec3fTypes> >()
        .add< component::forcefield::TriangleBendingSprings<OpenCLVec3f1Types> >()
        .add< component::forcefield::TriangleBendingSprings<OpenCLVec3dTypes> >()
        .add< component::forcefield::TriangleBendingSprings<OpenCLVec3d1Types> >()
        ;

int QuadBendingSpringsOpenCLClass = core::RegisterObject("Supports GPU-side computations using OPENCL")
        .add< component::forcefield::QuadBendingSprings<OpenCLVec3fTypes> >()
        .add< component::forcefield::QuadBendingSprings<OpenCLVec3f1Types> >()
        .add< component::forcefield::QuadBendingSprings<OpenCLVec3dTypes> >()
        .add< component::forcefield::QuadBendingSprings<OpenCLVec3d1Types> >()
        ;

int BoxStiffSpringForceFieldOpenCLClass = core::RegisterObject("Supports GPU-side computations using OPENCL")
        .add< component::forcefield::BoxStiffSpringForceField<OpenCLVec3fTypes> >()
        .add< component::forcefield::BoxStiffSpringForceField<OpenCLVec3f1Types> >()
        .add< component::forcefield::BoxStiffSpringForceField<OpenCLVec3dTypes> >()
        .add< component::forcefield::BoxStiffSpringForceField<OpenCLVec3d1Types> >()
        ;








/////////////////////////////////////////////////////////////////////////
//            kernels

//
//
//void SpringForceFieldOpenCL3f_addForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*x*/, const _device_pointer/*v*/){DEBUG_TEXT("no implemented");}
//void SpringForceFieldOpenCL3f_addExternalForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*x1*/, const _device_pointer/*v1*/, const _device_pointer/*x1*/, const _device_pointer/*v2*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3f_addForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*x*/, const _device_pointer/*v*/, _device_pointer/*dfdx*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3f_addExternalForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*x1*/, const _device_pointer/*v1*/, const _device_pointer/*x1*/, const _device_pointer/*v2*/, _device_pointer/*dfdx*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3f_addDForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*dx*/, const _device_pointer/*x*/, const _device_pointer/*dfdx*/, double/*factor*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3f_addExternalDForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*dx1*/, const _device_pointer/*x1*/, const _device_pointer/*dx2*/, const _device_pointer/*x1*/, const _device_pointer/*dfdx*/, double/*factor*/){DEBUG_TEXT("no implemented");}
//
//
//
//void MultiStiffSpringForceFieldOpenCL3f_addDForce(int /*n*/, SpringDForceOp* /*ops*/, double/*factor*/);
//
//void SpringForceFieldOpenCL3f1_addForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*x*/, const _device_pointer/*v*/){DEBUG_TEXT("no implemented");}
//void SpringForceFieldOpenCL3f1_addExternalForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*x1*/, const _device_pointer/*v1*/, const _device_pointer/*x1*/, const _device_pointer/*v2*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3f1_addForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*x*/, const _device_pointer/*v*/, _device_pointer/*dfdx*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3f1_addExternalForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*x1*/, const _device_pointer/*v1*/, const _device_pointer/*x1*/, const _device_pointer/*v2*/, _device_pointer/*dfdx*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3f1_addDForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*dx*/, const _device_pointer/*x*/, const _device_pointer/*dfdx*/, double/*factor*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3f1_addExternalDForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*dx1*/, const _device_pointer/*x1*/, const _device_pointer/*dx2*/, const _device_pointer/*x1*/, const _device_pointer/*dfdx*/, double/*factor*/){DEBUG_TEXT("no implemented");}
//
//
//
//void SpringForceFieldOpenCL3d_addForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*x*/, const _device_pointer/*v*/){DEBUG_TEXT("no implemented");}
//void SpringForceFieldOpenCL3d_addExternalForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*x1*/, const _device_pointer/*v1*/, const _device_pointer/*x1*/, const _device_pointer/*v2*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3d_addForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*x*/, const _device_pointer/*v*/, _device_pointer/*dfdx*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3d_addExternalForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*x1*/, const _device_pointer/*v1*/, const _device_pointer/*x1*/, const _device_pointer/*v2*/, _device_pointer/*dfdx*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3d_addDForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*dx*/, const _device_pointer/*x*/, const _device_pointer/*dfdx*/, double/*factor*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3d_addExternalDForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*dx1*/, const _device_pointer/*x1*/, const _device_pointer/*dx2*/, const _device_pointer/*x1*/, const _device_pointer/*dfdx*/, double/*factor*/){DEBUG_TEXT("no implemented");}
//
//void SpringForceFieldOpenCL3d1_addForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*x*/, const _device_pointer/*v*/){DEBUG_TEXT("no implemented");}
//void SpringForceFieldOpenCL3d1_addExternalForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*x1*/, const _device_pointer/*v1*/, const _device_pointer/*x1*/, const _device_pointer/*v2*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3d1_addForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*x*/, const _device_pointer/*v*/, _device_pointer/*dfdx*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3d1_addExternalForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*x1*/, const _device_pointer/*v1*/, const _device_pointer/*x1*/, const _device_pointer/*v2*/, _device_pointer/*dfdx*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3d1_addDForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f*/, const _device_pointer/*dx*/, const _device_pointer/*x*/, const _device_pointer/*dfdx*/, double/*factor*/){DEBUG_TEXT("no implemented");}
//void StiffSpringForceFieldOpenCL3d1_addExternalDForce(unsigned int /*nbVertex*/, unsigned int /*nbSpringPerVertex*/, const _device_pointer /*springs*/, _device_pointer/*f1*/, const _device_pointer/*dx1*/, const _device_pointer/*x1*/, const _device_pointer/*dx2*/, const _device_pointer/*x1*/, const _device_pointer/*dfdx*/, double/*factor*/){DEBUG_TEXT("no implemented");}
//
//
//
//
//














} // namespace opencl

} // namespace gpu

} // namespace sofa
