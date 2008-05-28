/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#include <sofa/gpu/cuda/CudaSpatialGridContainer.inl>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace container
{

using namespace sofa::defaulttype;
using namespace sofa::gpu::cuda;
using namespace core::componentmodel::behavior;


SOFA_DECL_CLASS(CudaSpatialGridContainer)

int SpatialGridContainerCudaClass = core::RegisterObject("GPU support using CUDA.")
        .add< SpatialGridContainer<CudaVec3fTypes> >()
        ;

template class SpatialGridContainer< CudaVec3fTypes >;
template class SpatialGrid< SpatialGridTypes< CudaVec3fTypes > >;

} // namespace container

} // namespace component

} // namespace sofa
