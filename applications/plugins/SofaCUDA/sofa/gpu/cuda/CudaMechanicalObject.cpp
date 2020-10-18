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
#ifndef SOFA_GPU_CUDA_CUDAMECHANICALOBJECT_CPP
#define SOFA_GPU_CUDA_CUDAMECHANICALOBJECT_CPP


#include "CudaTypes.h"
#include "CudaMechanicalObject.inl"
#include <sofa/core/ObjectFactory.h>
#include <SofaBaseMechanics/MappedObject.inl>
#include <sofa/core/State.inl>

namespace sofa
{

namespace gpu
{

namespace cuda
{

int MechanicalObjectCudaClass = core::RegisterObject("Supports GPU-side computations using CUDA")
        .add< component::container::MechanicalObject<CudaVec1fTypes> >()
        .add< component::container::MechanicalObject<CudaVec2fTypes> >()
        .add< component::container::MechanicalObject<CudaVec3fTypes> >()
        .add< component::container::MechanicalObject<CudaVec3f1Types> >()
        .add< component::container::MechanicalObject<CudaVec6fTypes> >()
        .add< component::container::MechanicalObject<CudaRigid3fTypes> >()
#ifdef SOFA_GPU_CUDA_DOUBLE
        .add< component::container::MechanicalObject<CudaVec3dTypes> >()
        .add< component::container::MechanicalObject<CudaVec3d1Types> >()
        .add< component::container::MechanicalObject<CudaVec6dTypes> >()
        .add< component::container::MechanicalObject<CudaRigid3dTypes> >()
#endif // SOFA_GPU_CUDA_DOUBLE
        ;

int MappedObjectCudaClass = core::RegisterObject("Supports GPU-side computations using CUDA")
        .add< component::container::MappedObject<CudaVec1fTypes> >()
        .add< component::container::MappedObject<CudaVec2fTypes> >()
        .add< component::container::MappedObject<CudaVec3fTypes> >()
        .add< component::container::MappedObject<CudaVec3f1Types> >()
        .add< component::container::MappedObject<CudaVec6fTypes> >()
        .add< component::container::MappedObject<CudaRigid3fTypes> >()
#ifdef SOFA_GPU_CUDA_DOUBLE
        .add< component::container::MappedObject<CudaVec3dTypes> >()
        .add< component::container::MappedObject<CudaVec3d1Types> >()
        .add< component::container::MappedObject<CudaVec6dTypes> >()
        .add< component::container::MappedObject<CudaRigid3dTypes> >()
#endif // SOFA_GPU_CUDA_DOUBLE
        ;

} // namespace cuda

} // namespace gpu

} // namespace sofa

#endif // SOFA_GPU_CUDA_CUDAMECHANICALOBJECT_CPP
