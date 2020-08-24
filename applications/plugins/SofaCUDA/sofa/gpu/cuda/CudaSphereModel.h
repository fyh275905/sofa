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
#ifndef SOFA_GPU_CUDA_CUDASPHEREMODEL_H
#define SOFA_GPU_CUDA_CUDASPHEREMODEL_H

#include "CudaTypes.h"

#include <SofaBaseCollision/SphereModel.h>

namespace sofa
{

namespace gpu
{

namespace cuda
{

using CudaSphereModel [[deprecated("The CudaSphereModel is now deprecated, please use sofa::component::collision::SphereCollisionModel<gpu::cuda::CudaVec3Types> instead. Compatibility stops at v20.06")]] = sofa::component::collision::SphereCollisionModel<gpu::cuda::CudaVec3Types>;
using CudaSphere = sofa::component::collision::TSphere<gpu::cuda::CudaVec3Types>;


} // namespace cuda

} // namespace gpu


namespace component
{
namespace collision
{


#if  !defined(SOFA_BUILD_GPU_CUDA)
extern template class sofa::component::collision::SphereCollisionModel<sofa::gpu::cuda::CudaVec3fTypes>;
extern template class sofa::component::collision::SphereCollisionModel<sofa::gpu::cuda::CudaVec3f1Types>;
#ifdef SOFA_GPU_CUDA_DOUBLE
extern template class sofa::component::collision::SphereCollisionModel<sofa::gpu::cuda::CudaVec3dTypes>;
extern template class sofa::component::collision::SphereCollisionModel<sofa::gpu::cuda::CudaVec3d1Types>;
#endif // SOFA_GPU_CUDA_DOUBLE
#endif

} // namespace collision

} // namespace component


} // namespace sofa

#endif
