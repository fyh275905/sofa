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
#ifndef SOFA_COMPONENT_MASS_CUDAMESHMATRIXMASS_H
#define SOFA_COMPONENT_MASS_CUDAMESHMATRIXMASS_H

#include "CudaTypes.h"
#include <SofaMiscForceField/MeshMatrixMass.h>

namespace sofa
{
namespace component
{
namespace mass
{

using namespace sofa::gpu::cuda;
using namespace sofa::component::mass;

template<>
class MeshMatrixMassInternalData<CudaVec3fTypes,float>
{
public:
    /// Cuda vector copying the vertex mass (enabling deviceRead)
    CudaVector<float> vMass;

    /// assumes the geometry object type is 3D
    typedef CudaVec3fTypes GeometricalTypes ;
};

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec3fTypes, float>::copyVertexMass();

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec3fTypes, float>::addMDx(const core::MechanicalParams*, DataVecDeriv& f, const DataVecDeriv& dx, SReal factor);

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec3fTypes, float>::addForce(const core::MechanicalParams*, DataVecDeriv& /*vf*/, const DataVecCoord& /* */, const DataVecDeriv& /* */);

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec3fTypes, float>::accFromF(const core::MechanicalParams*, DataVecDeriv& a, const DataVecDeriv& f);




template<>
class MeshMatrixMassInternalData<CudaVec2fTypes,float>
{
public:
    /// Cuda vector copying the vertex mass (enabling deviceRead)
    CudaVector<float> vMass;

    /// assumes the geometry object type is 3D
    typedef CudaVec3fTypes GeometricalTypes ;
};

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec2fTypes, float>::copyVertexMass();

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec2fTypes, float>::addMDx(const core::MechanicalParams*, DataVecDeriv& f, const DataVecDeriv& dx, SReal factor);

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec2fTypes, float>::addForce(const core::MechanicalParams*, DataVecDeriv& /*vf*/, const DataVecCoord& /* */, const DataVecDeriv& /* */);

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec2fTypes, float>::accFromF(const core::MechanicalParams*, DataVecDeriv& a, const DataVecDeriv& f);




template<>
class MeshMatrixMassInternalData<CudaVec1fTypes,float>
{
public:
    /// Cuda vector copying the vertex mass (enabling deviceRead)
    CudaVector<float> vMass;

    /// assumes the geometry object type is 3D
    typedef CudaVec3fTypes GeometricalTypes ;
};

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec1fTypes, float>::copyVertexMass();

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec1fTypes, float>::addMDx(const core::MechanicalParams*, DataVecDeriv& f, const DataVecDeriv& dx, SReal factor);

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec1fTypes, float>::addForce(const core::MechanicalParams*, DataVecDeriv& /*vf*/, const DataVecCoord& /* */, const DataVecDeriv& /* */);

template<>
void MeshMatrixMass<sofa::gpu::cuda::CudaVec1fTypes, float>::accFromF(const core::MechanicalParams*, DataVecDeriv& a, const DataVecDeriv& f);


#ifndef SOFA_GPU_CUDA_CUDAMESHMATRIXMASS_CPP
template class MeshMatrixMass<sofa::gpu::cuda::CudaVec3fTypes, float>;
template class MeshMatrixMass<sofa::gpu::cuda::CudaVec2fTypes, float>;
template class MeshMatrixMass<sofa::gpu::cuda::CudaVec1fTypes, float>;

#ifdef SOFA_GPU_CUDA_DOUBLE
template class MeshMatrixMass<sofa::gpu::cuda::CudaVec3dTypes, double>;
template class MeshMatrixMass<sofa::gpu::cuda::CudaVec2dTypes, double>;
template class MeshMatrixMass<sofa::gpu::cuda::CudaVec1dTypes, double>;
#endif // SOFA_GPU_CUDA_DOUBLE

#endif //SOFA_GPU_CUDA_CUDAMESHMATRIXMASS_CPP

} // namespace mass

} // namespace component

} // namespace sofa

#endif //SOFA_COMPONENT_MASS_CUDAMESHMATRIXMASS_H
