﻿/******************************************************************************
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
#include <SofaBaseMechanics/SubsetMultiMapping.inl>
#include <sofa/core/ObjectFactory.h>
#include "CudaTypes.h"

namespace sofa::gpu::cuda
{
    using namespace sofa::component::mapping;

    // Register in the Factory
    int SubsetMultiMappingCudaClass = core::RegisterObject("Compute a subset of the input MechanicalObjects according to a dof index list")
        .add< SubsetMultiMapping< CudaVec6Types, CudaVec6Types > >()
        .add< SubsetMultiMapping< CudaVec3Types, CudaVec3Types > >()
        .add< SubsetMultiMapping< CudaVec3f1Types, CudaVec3f1Types > >()
        .add< SubsetMultiMapping< CudaVec2Types, CudaVec2Types > >()
        .add< SubsetMultiMapping< CudaVec1Types, CudaVec1Types > >()
        .add< SubsetMultiMapping< CudaRigid3Types, CudaRigid3Types > >()
        .add< SubsetMultiMapping< CudaRigid3Types, CudaVec3Types > >()
    ;
}

namespace sofa::component::mapping
{
    using namespace sofa::gpu::cuda;

    template class SOFA_GPU_CUDA_API SubsetMultiMapping< CudaVec6Types, CudaVec6Types >;
    template class SOFA_GPU_CUDA_API SubsetMultiMapping< CudaVec3Types, CudaVec3Types >;
    template class SOFA_GPU_CUDA_API SubsetMultiMapping< CudaVec3f1Types, CudaVec3f1Types >;
    template class SOFA_GPU_CUDA_API SubsetMultiMapping< CudaVec2Types, CudaVec2Types >;
    template class SOFA_GPU_CUDA_API SubsetMultiMapping< CudaVec1Types, CudaVec1Types >;
    template class SOFA_GPU_CUDA_API SubsetMultiMapping< CudaRigid3Types, CudaRigid3Types >;
    template class SOFA_GPU_CUDA_API SubsetMultiMapping< CudaRigid3Types, CudaVec3Types >;


}//namespace sofa::component::mapping
