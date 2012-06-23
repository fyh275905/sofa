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


#ifndef SOFA_TYPEDEF_CollisionAlgorithm_float_H
#define SOFA_TYPEDEF_CollisionAlgorithm_float_H

//Default files containing the declaration of the vector type
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/Mat.h>


#ifdef SOFA_GPU_CUDA
#include <sofa/gpu/cuda/CudaTypes.h>
#endif
#ifdef SOFA_GPU_OPENCL
#include <sofa/gpu/opencl/OpenCLTypes.h>
#endif


#include <sofa/gpu/cuda/CudaRasterizer.h>
#include <sofa/gpu/cuda/PairwiseCudaRasterizer.h>
#include <sofa/gpu/cuda/ProximityRasterizer.h>



//---------------------------------------------------------------------------------------------
//Typedef for CudaRasterizer
typedef sofa::gpu::cuda::CudaRasterizer<sofa::gpu::cuda::CudaVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > CudaRasterizerCuda3f;
typedef sofa::gpu::cuda::CudaRasterizer<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > CudaRasterizer3f;



//---------------------------------------------------------------------------------------------
//Typedef for PairwiseCudaRasterizer
typedef sofa::gpu::cuda::PairwiseCudaRasterizer<sofa::gpu::cuda::CudaVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > PairwiseCudaRasterizerCuda3f;
typedef sofa::gpu::cuda::PairwiseCudaRasterizer<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > PairwiseCudaRasterizer3f;



//---------------------------------------------------------------------------------------------
//Typedef for ProximityRasterizer
typedef sofa::gpu::cuda::ProximityRasterizer<sofa::gpu::cuda::CudaVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > ProximityRasterizerCuda3f;
typedef sofa::gpu::cuda::ProximityRasterizer<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > ProximityRasterizer3f;





#ifdef SOFA_FLOAT
typedef CudaRasterizerCuda3f CudaRasterizerCuda3;
typedef CudaRasterizer3f CudaRasterizer3;
typedef PairwiseCudaRasterizerCuda3f PairwiseCudaRasterizerCuda3;
typedef PairwiseCudaRasterizer3f PairwiseCudaRasterizer3;
typedef ProximityRasterizerCuda3f ProximityRasterizerCuda3;
typedef ProximityRasterizer3f ProximityRasterizer3;
#endif

#endif
