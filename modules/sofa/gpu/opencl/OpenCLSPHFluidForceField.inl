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
#ifndef SOFA_GPU_OPENCL_OPENCLSPHFLUIDFORCEFIELD_INL
#define SOFA_GPU_OPENCL_OPENCLSPHFLUIDFORCEFIELD_INL

#include "OpenCLSPHFluidForceField.h"
#include <sofa/component/forcefield/SPHFluidForceField.inl>
//#include <sofa/gpu/opencl/OpenCLSpatialGridContainer.inl>

namespace sofa
{

namespace gpu
{

namespace opencl
{

//extern "C"
//{
//
//
//extern void SPHFluidForceFieldOpenCL3f_computeDensity(unsigned int size, const void* cells, const void* cellGhost, GPUSPHFluid3f* params, void* pos4, const void* x);
//extern void SPHFluidForceFieldOpenCL3f_addForce (unsigned int size, const void* cells, const void* cellGhost, GPUSPHFluid3f* params, void* f, const void* pos4, const void* v);
//extern void SPHFluidForceFieldOpenCL3f_addDForce(unsigned int size, const void* cells, const void* cellGhost, GPUSPHFluid3f* params, void* f, const void* pos4, const void* v, const void* dx);
//
//
//
//extern void SPHFluidForceFieldOpenCL3d_computeDensity(unsigned int size, const void* cells, const void* cellGhost, GPUSPHFluid3d* params, void* pos4, const void* x);
//extern void SPHFluidForceFieldOpenCL3d_addForce (unsigned int size, const void* cells, const void* cellGhost, GPUSPHFluid3d* params, void* f, const void* pos4, const void* v);
//extern void SPHFluidForceFieldOpenCL3d_addDForce(unsigned int size, const void* cells, const void* cellGhost, GPUSPHFluid3d* params, void* f, const void* pos4, const void* v, const void* dx);
//
//
//}

} // namespace OpenCL

} // namespace gpu

namespace component
{

namespace forcefield
{

using namespace gpu::opencl;

//template<>
//void SPHFluidForceFieldInternalData<gpu::opencl::OpenCLVec3fTypes>::Kernels_computeDensity(int gsize, const void* cells, const void* cellGhost, void* pos4, const void* x)
//{
//	SPHFluidForceFieldOpenCL3f_computeDensity(gsize, cells, cellGhost, &params, pos4, x);
//}
//
//template<>
//void SPHFluidForceFieldInternalData<gpu::opencl::OpenCLVec3fTypes>::Kernels_addForce(int gsize, const void* cells, const void* cellGhost, void* f, const void* pos4, const void* v)
//{
//	SPHFluidForceFieldOpenCL3f_addForce (gsize, cells, cellGhost, &params, f, pos4, v);
//}
//
//template<>
//void SPHFluidForceFieldInternalData<gpu::opencl::OpenCLVec3fTypes>::Kernels_addDForce(int gsize, const void* cells, const void* cellGhost, void* f, const void* pos4, const void* v, const void* dx)
//{
//	SPHFluidForceFieldOpenCL3f_addDForce(gsize, cells, cellGhost, &params, f, pos4, v, dx);
//}
//
//template <>
//void SPHFluidForceField<gpu::opencl::OpenCLVec3fTypes>::addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& v)
//{
//	if (grid == NULL) return;
//	grid->updateGrid(x);
//	data.fillParams(this);
//	f.resize(x.size());
//	Grid::Grid* g = grid->getGrid();
//	data.pos4.recreate(x.size());
//	data.Kernels_computeDensity(
//		g->getNbCells(), g->getCellsVector().deviceRead(), g->getCellGhostVector().deviceRead(),
//		data.pos4.deviceWrite(), x.deviceRead());
//	data.Kernels_addForce(
//		g->getNbCells(), g->getCellsVector().deviceRead(), g->getCellGhostVector().deviceRead(),
//		f.deviceWrite(), data.pos4.deviceRead(), v.deviceRead());
//}
//
//template <>
//void SPHFluidForceField<gpu::opencl::OpenCLVec3fTypes>::addDForce(VecDeriv& df, const VecCoord& dx, double kFactor, double bFactor)
//{
//	return;
//	if (grid == NULL) return;
//	sout << "addDForce(" << kFactor << "," << bFactor << ")" << sendl;
//	//const VecCoord& x = *this->mstate->getX();
//	const VecDeriv& v = *this->mstate->getV();
//	data.fillParams(this, kFactor, bFactor);
//	df.resize(dx.size());
//	Grid::Grid* g = grid->getGrid();
//	data.Kernels_addDForce(
//		g->getNbCells(), g->getCellsVector().deviceRead(), g->getCellGhostVector().deviceRead(),
//		df.deviceWrite(), data.pos4.deviceRead(), v.deviceRead(), dx.deviceRead());
//}
//
//
//template<>
//void SPHFluidForceFieldInternalData<gpu::opencl::OpenCLVec3dTypes>::Kernels_computeDensity(int gsize, const void* cells, const void* cellGhost, void* pos4, const void* x)
//{
//	SPHFluidForceFieldOpenCL3d_computeDensity(gsize, cells, cellGhost, &params, pos4, x);
//}
//
//template<>
//void SPHFluidForceFieldInternalData<gpu::opencl::OpenCLVec3dTypes>::Kernels_addForce(int gsize, const void* cells, const void* cellGhost, void* f, const void* pos4, const void* v)
//{
//	SPHFluidForceFieldOpenCL3d_addForce (gsize, cells, cellGhost, &params, f, pos4, v);
//}
//
//template<>
//void SPHFluidForceFieldInternalData<gpu::opencl::OpenCLVec3dTypes>::Kernels_addDForce(int gsize, const void* cells, const void* cellGhost, void* f, const void* pos4, const void* v, const void* dx)
//{
//	SPHFluidForceFieldOpenCL3d_addDForce(gsize, cells, cellGhost, &params, f, pos4, v, dx);
//}
//
//template <>
//void SPHFluidForceField<gpu::opencl::OpenCLVec3dTypes>::addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& v)
//{
//	if (grid == NULL) return;
//	grid->updateGrid(x);
//	data.fillParams(this);
//	f.resize(x.size());
//	Grid::Grid* g = grid->getGrid();
//	data.pos4.recreate(x.size());
//	data.Kernels_computeDensity(
//		g->getNbCells(), g->getCellsVector().deviceRead(), g->getCellGhostVector().deviceRead(),
//		data.pos4.deviceWrite(), x.deviceRead());
//	data.Kernels_addForce(
//		g->getNbCells(), g->getCellsVector().deviceRead(), g->getCellGhostVector().deviceRead(),
//		f.deviceWrite(), data.pos4.deviceRead(), v.deviceRead());
//}
//
//template <>
//void SPHFluidForceField<gpu::opencl::OpenCLVec3dTypes>::addDForce(VecDeriv& df, const VecCoord& dx, double kFactor, double bFactor)
//{
//	if (grid == NULL) return;
//	//const VecCoord& x = *this->mstate->getX();
//	const VecDeriv& v = *this->mstate->getV();
//	data.fillParams(this, kFactor, bFactor);
//	df.resize(dx.size());
//	Grid::Grid* g = grid->getGrid();
//	data.Kernels_addDForce(
//		g->getNbCells(), g->getCellsVector().deviceRead(), g->getCellGhostVector().deviceRead(),
//		df.deviceWrite(), data.pos4.deviceRead(), v.deviceRead(), dx.deviceRead());
//}
//
//
//
//
//
//template <>
//void SPHFluidForceField<gpu::opencl::OpenCLVec3fTypes>::draw()
//{
//	if (!getContext()->getShowForceFields()) return;
//	//if (grid != NULL)
//	//	grid->draw();
//	const VecCoord& x = *this->mstate->getX();
//	const gpu::opencl::OpenCLVector<defaulttype::Vec4f> pos4 = this->data.pos4;
//	if (pos4.empty()) return;
//	glDisable(GL_LIGHTING);
//	glColor3f(0,1,1);
//	glDisable(GL_BLEND);
//	glDepthMask(1);
//	glPointSize(5);
//	glBegin(GL_POINTS);
//	for (unsigned int i=0;i<pos4.size();i++)
//	{
//		float density = pos4[i][3];
//		float f = (float)(density / density0.getValue());
//		f = 1+10*(f-1);
//		if (f < 1)
//		{
//			glColor3f(0,1-f,f);
//		}
//		else
//		{
//			glColor3f(f-1,0,2-f);
//		}
//		helper::gl::glVertexT(x[i]);
//	}
//	glEnd();
//	glPointSize(1);
//}


} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
