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


#ifndef SOFA_TYPEDEF_VisualModel_float_H
#define SOFA_TYPEDEF_VisualModel_float_H

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


#include <sofa/component/visualmodel/DrawV.h>
#include <sofa/component/visualmodel/OglModel.h>
#include <sofa/component/visualmodel/OglShaderVisualModel.h>
#include <sofa/component/visualmodel/OglTetrahedralModel.h>
#include <sofa/component/visualmodel/VisualModelImpl.h>



//---------------------------------------------------------------------------------------------
//Typedef for DrawV
typedef sofa::component::visualmodel::DrawV DrawVExt3f;



//---------------------------------------------------------------------------------------------
//Typedef for OglModel
typedef sofa::component::visualmodel::OglModel OglModelExt3f;



//---------------------------------------------------------------------------------------------
//Typedef for OglShaderVisualModel
typedef sofa::component::visualmodel::OglShaderVisualModel OglShaderVisualModelExt3f;



//---------------------------------------------------------------------------------------------
//Typedef for OglTetrahedralModel
typedef sofa::component::visualmodel::OglTetrahedralModel<sofa::defaulttype::StdVectorTypes<sofa::defaulttype::Vec<3, float>, sofa::defaulttype::Vec<3, float>, float> > OglTetrahedralModel3f;



//---------------------------------------------------------------------------------------------
//Typedef for VisualModelImpl
typedef sofa::component::visualmodel::VisualModelImpl VisualModelImplExt3f;





#ifdef SOFA_FLOAT
typedef DrawVExt3f DrawVExt3;
typedef OglModelExt3f OglModelExt3;
typedef OglShaderVisualModelExt3f OglShaderVisualModelExt3;
typedef OglTetrahedralModel3f OglTetrahedralModel3;
typedef VisualModelImplExt3f VisualModelImplExt3;
#endif

#endif
