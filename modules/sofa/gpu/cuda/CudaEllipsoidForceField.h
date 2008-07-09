/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_GPU_CUDA_CUDAELLIPSOIDFORCEFIELD_H
#define SOFA_GPU_CUDA_CUDAELLIPSOIDFORCEFIELD_H

#include "CudaTypes.h"
#include <sofa/component/forcefield/EllipsoidForceField.h>

namespace sofa
{

namespace gpu
{

namespace cuda
{

struct GPUEllipsoid
{
    defaulttype::Vec3f center;
    defaulttype::Vec3f inv_r2;
    float stiffness;
    float damping;
};

} // namespace cuda

} // namespace gpu

namespace component
{

namespace forcefield
{

template <>
class EllipsoidForceFieldInternalData<gpu::cuda::CudaVec3fTypes>
{
public:
    gpu::cuda::GPUEllipsoid ellipsoid;
    gpu::cuda::CudaVector<float> tmp;
};

template <>
void EllipsoidForceField<gpu::cuda::CudaVec3fTypes>::addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

template <>
void EllipsoidForceField<gpu::cuda::CudaVec3fTypes>::addDForce (VecDeriv& df, const VecDeriv& dx, double kFactor, double bFactor);

template <>
class EllipsoidForceFieldInternalData<gpu::cuda::CudaVec3f1Types>
{
public:
    gpu::cuda::GPUEllipsoid ellipsoid;
    gpu::cuda::CudaVector<float> tmp;
};

template <>
void EllipsoidForceField<gpu::cuda::CudaVec3f1Types>::addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

template <>
void EllipsoidForceField<gpu::cuda::CudaVec3f1Types>::addDForce (VecDeriv& df, const VecDeriv& dx, double kFactor, double bFactor);

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
