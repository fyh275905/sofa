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
#ifndef SOFA_GPU_CUDA_CUDAMECHANICALOBJECT_INL
#define SOFA_GPU_CUDA_CUDAMECHANICALOBJECT_INL

#include "CudaMechanicalObject.h"
#include <sofa/component/container/MechanicalObject.inl>


namespace sofa
{

namespace gpu
{

namespace cuda
{

extern "C"
{
    void MechanicalObjectCudaVec3f_vAssign(unsigned int size, void* res, const void* a);
    void MechanicalObjectCudaVec3f_vClear(unsigned int size, void* res);
    void MechanicalObjectCudaVec3f_vMEq(unsigned int size, void* res, float f);
    void MechanicalObjectCudaVec3f_vEqBF(unsigned int size, void* res, const void* b, float f);
    void MechanicalObjectCudaVec3f_vPEq(unsigned int size, void* res, const void* a);
    void MechanicalObjectCudaVec3f_vPEqBF(unsigned int size, void* res, const void* b, float f);
    void MechanicalObjectCudaVec3f_vAdd(unsigned int size, void* res, const void* a, const void* b);
    void MechanicalObjectCudaVec3f_vOp(unsigned int size, void* res, const void* a, const void* b, float f);
    void MechanicalObjectCudaVec3f_vIntegrate(unsigned int size, const void* a, void* v, void* x, float f_v_v, float f_v_a, float f_x_x, float f_x_v);
    void MechanicalObjectCudaVec3f_vPEqBF2(unsigned int size, void* res1, const void* b1, float f1, void* res2, const void* b2, float f2);
    void MechanicalObjectCudaVec3f_vPEq4BF2(unsigned int size, void* res1, const void* b11, float f11, const void* b12, float f12, const void* b13, float f13, const void* b14, float f14,
            void* res2, const void* b21, float f21, const void* b22, float f22, const void* b23, float f23, const void* b24, float f24);
    void MechanicalObjectCudaVec3f_vOp2(unsigned int size, void* res1, const void* a1, const void* b1, float f1, void* res2, const void* a2, const void* b2, float f2);
    int MechanicalObjectCudaVec3f_vDotTmpSize(unsigned int size);
    void MechanicalObjectCudaVec3f_vDot(unsigned int size, float* res, const void* a, const void* b, void* tmp, float* cputmp);
    void MechanicalObjectCudaVec3f1_vAssign(unsigned int size, void* res, const void* a);
    void MechanicalObjectCudaVec3f1_vClear(unsigned int size, void* res);
    void MechanicalObjectCudaVec3f1_vMEq(unsigned int size, void* res, float f);
    void MechanicalObjectCudaVec3f1_vEqBF(unsigned int size, void* res, const void* b, float f);
    void MechanicalObjectCudaVec3f1_vPEq(unsigned int size, void* res, const void* a);
    void MechanicalObjectCudaVec3f1_vPEqBF(unsigned int size, void* res, const void* b, float f);
    void MechanicalObjectCudaVec3f1_vAdd(unsigned int size, void* res, const void* a, const void* b);
    void MechanicalObjectCudaVec3f1_vOp(unsigned int size, void* res, const void* a, const void* b, float f);
    void MechanicalObjectCudaVec3f1_vIntegrate(unsigned int size, const void* a, void* v, void* x, float f_v_v, float f_v_a, float f_x_x, float f_x_v);
    void MechanicalObjectCudaVec3f1_vPEqBF2(unsigned int size, void* res1, const void* b1, float f1, void* res2, const void* b2, float f2);
    void MechanicalObjectCudaVec3f1_vPEq4BF2(unsigned int size, void* res1, const void* b11, float f11, const void* b12, float f12, const void* b13, float f13, const void* b14, float f14,
            void* res2, const void* b21, float f21, const void* b22, float f22, const void* b23, float f23, const void* b24, float f24);
    void MechanicalObjectCudaVec3f1_vOp2(unsigned int size, void* res1, const void* a1, const void* b1, float f1, void* res2, const void* a2, const void* b2, float f2);
    int MechanicalObjectCudaVec3f1_vDotTmpSize(unsigned int size);
    void MechanicalObjectCudaVec3f1_vDot(unsigned int size, float* res, const void* a, const void* b, void* tmp, float* cputmp);

#ifdef SOFA_GPU_CUDA_DOUBLE

    void MechanicalObjectCudaVec3d_vAssign(unsigned int size, void* res, const void* a);
    void MechanicalObjectCudaVec3d_vClear(unsigned int size, void* res);
    void MechanicalObjectCudaVec3d_vMEq(unsigned int size, void* res, double f);
    void MechanicalObjectCudaVec3d_vEqBF(unsigned int size, void* res, const void* b, double f);
    void MechanicalObjectCudaVec3d_vPEq(unsigned int size, void* res, const void* a);
    void MechanicalObjectCudaVec3d_vPEqBF(unsigned int size, void* res, const void* b, double f);
    void MechanicalObjectCudaVec3d_vAdd(unsigned int size, void* res, const void* a, const void* b);
    void MechanicalObjectCudaVec3d_vOp(unsigned int size, void* res, const void* a, const void* b, double f);
    void MechanicalObjectCudaVec3d_vIntegrate(unsigned int size, const void* a, void* v, void* x, double f_v_v, double f_v_a, double f_x_x, double f_x_v);
    void MechanicalObjectCudaVec3d_vPEqBF2(unsigned int size, void* res1, const void* b1, double f1, void* res2, const void* b2, double f2);
    void MechanicalObjectCudaVec3d_vPEq4BF2(unsigned int size, void* res1, const void* b11, double f11, const void* b12, double f12, const void* b13, double f13, const void* b14, double f14,
            void* res2, const void* b21, double f21, const void* b22, double f22, const void* b23, double f23, const void* b24, double f24);
    void MechanicalObjectCudaVec3d_vOp2(unsigned int size, void* res1, const void* a1, const void* b1, double f1, void* res2, const void* a2, const void* b2, double f2);
    int MechanicalObjectCudaVec3d_vDotTmpSize(unsigned int size);
    void MechanicalObjectCudaVec3d_vDot(unsigned int size, double* res, const void* a, const void* b, void* tmp, double* cputmp);
    void MechanicalObjectCudaVec3d1_vAssign(unsigned int size, void* res, const void* a);
    void MechanicalObjectCudaVec3d1_vClear(unsigned int size, void* res);
    void MechanicalObjectCudaVec3d1_vMEq(unsigned int size, void* res, double f);
    void MechanicalObjectCudaVec3d1_vEqBF(unsigned int size, void* res, const void* b, double f);
    void MechanicalObjectCudaVec3d1_vPEq(unsigned int size, void* res, const void* a);
    void MechanicalObjectCudaVec3d1_vPEqBF(unsigned int size, void* res, const void* b, double f);
    void MechanicalObjectCudaVec3d1_vAdd(unsigned int size, void* res, const void* a, const void* b);
    void MechanicalObjectCudaVec3d1_vOp(unsigned int size, void* res, const void* a, const void* b, double f);
    void MechanicalObjectCudaVec3d1_vIntegrate(unsigned int size, const void* a, void* v, void* x, double f_v_v, double f_v_a, double f_x_x, double f_x_v);
    void MechanicalObjectCudaVec3d1_vPEqBF2(unsigned int size, void* res1, const void* b1, double f1, void* res2, const void* b2, double f2);
    void MechanicalObjectCudaVec3d1_vPEq4BF2(unsigned int size, void* res1, const void* b11, double f11, const void* b12, double f12, const void* b13, double f13, const void* b14, double f14,
            void* res2, const void* b21, double f21, const void* b22, double f22, const void* b23, double f23, const void* b24, double f24);
    void MechanicalObjectCudaVec3d1_vOp2(unsigned int size, void* res1, const void* a1, const void* b1, double f1, void* res2, const void* a2, const void* b2, double f2);
    int MechanicalObjectCudaVec3d1_vDotTmpSize(unsigned int size);
    void MechanicalObjectCudaVec3d1_vDot(unsigned int size, double* res, const void* a, const void* b, void* tmp, double* cputmp);

#endif // SOFA_GPU_CUDA_DOUBLE

} // extern "C"

template<>
class CudaKernelsMechanicalObject<CudaVec3fTypes>
{
public:
    static void vAssign(unsigned int size, void* res, const void* a)
    {   MechanicalObjectCudaVec3f_vAssign(size, res, a); }
    static void vClear(unsigned int size, void* res)
    {   MechanicalObjectCudaVec3f_vClear(size, res); }
    static void vMEq(unsigned int size, void* res, float f)
    {   MechanicalObjectCudaVec3f_vMEq(size, res, f); }
    static void vEqBF(unsigned int size, void* res, const void* b, float f)
    {   MechanicalObjectCudaVec3f_vEqBF(size, res, b, f); }
    static void vPEq(unsigned int size, void* res, const void* a)
    {   MechanicalObjectCudaVec3f_vPEq(size, res, a); }
    static void vPEqBF(unsigned int size, void* res, const void* b, float f)
    {   MechanicalObjectCudaVec3f_vPEqBF(size, res, b, f); }
    static void vAdd(unsigned int size, void* res, const void* a, const void* b)
    {   MechanicalObjectCudaVec3f_vAdd(size, res, a, b); }
    static void vOp(unsigned int size, void* res, const void* a, const void* b, float f)
    {   MechanicalObjectCudaVec3f_vOp(size, res, a, b, f); }
    static void vIntegrate(unsigned int size, const void* a, void* v, void* x, float f_v_v, float f_v_a, float f_x_x, float f_x_v)
    {   MechanicalObjectCudaVec3f_vIntegrate(size, a, v, x, f_v_v, f_v_a, f_x_x, f_x_v); }
    static void vPEqBF2(unsigned int size, void* res1, const void* b1, float f1, void* res2, const void* b2, float f2)
    {   MechanicalObjectCudaVec3f_vPEqBF2(size, res1, b1, f1, res2, b2, f2); }
    static void vPEq4BF2(unsigned int size, void* res1, const void* b11, float f11, const void* b12, float f12, const void* b13, float f13, const void* b14, float f14,
            void* res2, const void* b21, float f21, const void* b22, float f22, const void* b23, float f23, const void* b24, float f24)
    {
        MechanicalObjectCudaVec3f_vPEq4BF2(size, res1, b11, f11, b12, f12, b13, f13, b14, f14,
                res2, b21, f21, b22, f22, b23, f23, b24, f24);
    }
    static void vOp2(unsigned int size, void* res1, const void* a1, const void* b1, float f1, void* res2, const void* a2, const void* b2, float f2)
    {   MechanicalObjectCudaVec3f_vOp2(size, res1, a1, b1, f1, res2, a2, b2, f2); }
    static int vDotTmpSize(unsigned int size)
    {   return MechanicalObjectCudaVec3f_vDotTmpSize(size); }
    static void vDot(unsigned int size, float* res, const void* a, const void* b, void* tmp, float* cputmp)
    {   MechanicalObjectCudaVec3f_vDot(size, res, a, b, tmp, cputmp); }
};

template<>
class CudaKernelsMechanicalObject<CudaVec3f1Types>
{
public:
    static void vAssign(unsigned int size, void* res, const void* a)
    {   MechanicalObjectCudaVec3f1_vAssign(size, res, a); }
    static void vClear(unsigned int size, void* res)
    {   MechanicalObjectCudaVec3f1_vClear(size, res); }
    static void vMEq(unsigned int size, void* res, float f)
    {   MechanicalObjectCudaVec3f1_vMEq(size, res, f); }
    static void vEqBF(unsigned int size, void* res, const void* b, float f)
    {   MechanicalObjectCudaVec3f1_vEqBF(size, res, b, f); }
    static void vPEq(unsigned int size, void* res, const void* a)
    {   MechanicalObjectCudaVec3f1_vPEq(size, res, a); }
    static void vPEqBF(unsigned int size, void* res, const void* b, float f)
    {   MechanicalObjectCudaVec3f1_vPEqBF(size, res, b, f); }
    static void vAdd(unsigned int size, void* res, const void* a, const void* b)
    {   MechanicalObjectCudaVec3f1_vAdd(size, res, a, b); }
    static void vOp(unsigned int size, void* res, const void* a, const void* b, float f)
    {   MechanicalObjectCudaVec3f1_vOp(size, res, a, b, f); }
    static void vIntegrate(unsigned int size, const void* a, void* v, void* x, float f_v_v, float f_v_a, float f_x_x, float f_x_v)
    {   MechanicalObjectCudaVec3f1_vIntegrate(size, a, v, x, f_v_v, f_v_a, f_x_x, f_x_v); }
    static void vPEqBF2(unsigned int size, void* res1, const void* b1, float f1, void* res2, const void* b2, float f2)
    {   MechanicalObjectCudaVec3f1_vPEqBF2(size, res1, b1, f1, res2, b2, f2); }
    static void vPEq4BF2(unsigned int size, void* res1, const void* b11, float f11, const void* b12, float f12, const void* b13, float f13, const void* b14, float f14,
            void* res2, const void* b21, float f21, const void* b22, float f22, const void* b23, float f23, const void* b24, float f24)
    {
        MechanicalObjectCudaVec3f1_vPEq4BF2(size, res1, b11, f11, b12, f12, b13, f13, b14, f14,
                res2, b21, f21, b22, f22, b23, f23, b24, f24);
    }
    static void vOp2(unsigned int size, void* res1, const void* a1, const void* b1, float f1, void* res2, const void* a2, const void* b2, float f2)
    {   MechanicalObjectCudaVec3f1_vOp2(size, res1, a1, b1, f1, res2, a2, b2, f2); }
    static int vDotTmpSize(unsigned int size)
    {   return MechanicalObjectCudaVec3f1_vDotTmpSize(size); }
    static void vDot(unsigned int size, float* res, const void* a, const void* b, void* tmp, float* cputmp)
    {   MechanicalObjectCudaVec3f1_vDot(size, res, a, b, tmp, cputmp); }
};

#ifdef SOFA_GPU_CUDA_DOUBLE

template<>
class CudaKernelsMechanicalObject<CudaVec3dTypes>
{
public:
    static void vAssign(unsigned int size, void* res, const void* a)
    {   MechanicalObjectCudaVec3d_vAssign(size, res, a); }
    static void vClear(unsigned int size, void* res)
    {   MechanicalObjectCudaVec3d_vClear(size, res); }
    static void vMEq(unsigned int size, void* res, double f)
    {   MechanicalObjectCudaVec3d_vMEq(size, res, f); }
    static void vEqBF(unsigned int size, void* res, const void* b, double f)
    {   MechanicalObjectCudaVec3d_vEqBF(size, res, b, f); }
    static void vPEq(unsigned int size, void* res, const void* a)
    {   MechanicalObjectCudaVec3d_vPEq(size, res, a); }
    static void vPEqBF(unsigned int size, void* res, const void* b, double f)
    {   MechanicalObjectCudaVec3d_vPEqBF(size, res, b, f); }
    static void vAdd(unsigned int size, void* res, const void* a, const void* b)
    {   MechanicalObjectCudaVec3d_vAdd(size, res, a, b); }
    static void vOp(unsigned int size, void* res, const void* a, const void* b, double f)
    {   MechanicalObjectCudaVec3d_vOp(size, res, a, b, f); }
    static void vIntegrate(unsigned int size, const void* a, void* v, void* x, double f_v_v, double f_v_a, double f_x_x, double f_x_v)
    {   MechanicalObjectCudaVec3d_vIntegrate(size, a, v, x, f_v_v, f_v_a, f_x_x, f_x_v); }
    static void vPEqBF2(unsigned int size, void* res1, const void* b1, double f1, void* res2, const void* b2, double f2)
    {   MechanicalObjectCudaVec3d_vPEqBF2(size, res1, b1, f1, res2, b2, f2); }
    static void vPEq4BF2(unsigned int size, void* res1, const void* b11, double f11, const void* b12, double f12, const void* b13, double f13, const void* b14, double f14,
            void* res2, const void* b21, double f21, const void* b22, double f22, const void* b23, double f23, const void* b24, double f24)
    {
        MechanicalObjectCudaVec3d_vPEq4BF2(size, res1, b11, f11, b12, f12, b13, f13, b14, f14,
                res2, b21, f21, b22, f22, b23, f23, b24, f24);
    }
    static void vOp2(unsigned int size, void* res1, const void* a1, const void* b1, double f1, void* res2, const void* a2, const void* b2, double f2)
    {   MechanicalObjectCudaVec3d_vOp2(size, res1, a1, b1, f1, res2, a2, b2, f2); }
    static int vDotTmpSize(unsigned int size)
    {   return MechanicalObjectCudaVec3d_vDotTmpSize(size); }
    static void vDot(unsigned int size, double* res, const void* a, const void* b, void* tmp, double* cputmp)
    {   MechanicalObjectCudaVec3d_vDot(size, res, a, b, tmp, cputmp); }
};

template<>
class CudaKernelsMechanicalObject<CudaVec3d1Types>
{
public:
    static void vAssign(unsigned int size, void* res, const void* a)
    {   MechanicalObjectCudaVec3d1_vAssign(size, res, a); }
    static void vClear(unsigned int size, void* res)
    {   MechanicalObjectCudaVec3d1_vClear(size, res); }
    static void vMEq(unsigned int size, void* res, double f)
    {   MechanicalObjectCudaVec3d1_vMEq(size, res, f); }
    static void vEqBF(unsigned int size, void* res, const void* b, double f)
    {   MechanicalObjectCudaVec3d1_vEqBF(size, res, b, f); }
    static void vPEq(unsigned int size, void* res, const void* a)
    {   MechanicalObjectCudaVec3d1_vPEq(size, res, a); }
    static void vPEqBF(unsigned int size, void* res, const void* b, double f)
    {   MechanicalObjectCudaVec3d1_vPEqBF(size, res, b, f); }
    static void vAdd(unsigned int size, void* res, const void* a, const void* b)
    {   MechanicalObjectCudaVec3d1_vAdd(size, res, a, b); }
    static void vOp(unsigned int size, void* res, const void* a, const void* b, double f)
    {   MechanicalObjectCudaVec3d1_vOp(size, res, a, b, f); }
    static void vIntegrate(unsigned int size, const void* a, void* v, void* x, double f_v_v, double f_v_a, double f_x_x, double f_x_v)
    {   MechanicalObjectCudaVec3d1_vIntegrate(size, a, v, x, f_v_v, f_v_a, f_x_x, f_x_v); }
    static void vPEqBF2(unsigned int size, void* res1, const void* b1, double f1, void* res2, const void* b2, double f2)
    {   MechanicalObjectCudaVec3d1_vPEqBF2(size, res1, b1, f1, res2, b2, f2); }
    static void vPEq4BF2(unsigned int size, void* res1, const void* b11, double f11, const void* b12, double f12, const void* b13, double f13, const void* b14, double f14,
            void* res2, const void* b21, double f21, const void* b22, double f22, const void* b23, double f23, const void* b24, double f24)
    {
        MechanicalObjectCudaVec3d1_vPEq4BF2(size, res1, b11, f11, b12, f12, b13, f13, b14, f14,
                res2, b21, f21, b22, f22, b23, f23, b24, f24);
    }
    static void vOp2(unsigned int size, void* res1, const void* a1, const void* b1, double f1, void* res2, const void* a2, const void* b2, double f2)
    {   MechanicalObjectCudaVec3d1_vOp2(size, res1, a1, b1, f1, res2, a2, b2, f2); }
    static int vDotTmpSize(unsigned int size)
    {   return MechanicalObjectCudaVec3d1_vDotTmpSize(size); }
    static void vDot(unsigned int size, double* res, const void* a, const void* b, void* tmp, double* cputmp)
    {   MechanicalObjectCudaVec3d1_vDot(size, res, a, b, tmp, cputmp); }
};

#endif // SOFA_GPU_CUDA_DOUBLE

} // namespace cuda

} // namespace gpu

namespace component
{

using namespace gpu::cuda;

template<class TCoord, class TDeriv, class TReal>
void MechanicalObjectInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::accumulateForce(Main* m)
{
    if (!m->externalForces->empty())
    {
        Kernels::vAssign(m->externalForces->size(), m->f->deviceWrite(), m->externalForces->deviceRead());
    }
}

template<class TCoord, class TDeriv, class TReal>
void MechanicalObjectInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::vAlloc(Main* m, VecId v)
{
    if (v.type == VecId::V_COORD && v.index >= VecId::V_FIRST_DYNAMIC_INDEX)
    {
        VecCoord* vec = m->getVecCoord(v.index);
        vec->fastResize(m->vsize);
    }
    else if (v.type == VecId::V_DERIV && v.index >= VecId::V_FIRST_DYNAMIC_INDEX)
    {
        VecDeriv* vec = m->getVecDeriv(v.index);
        vec->fastResize(m->vsize);
    }
    else
    {
        std::cerr << "Invalid alloc operation ("<<v<<")\n";
        return;
    }
    //vOp(v); // clear vector
}

template<class TCoord, class TDeriv, class TReal>
void MechanicalObjectInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::vOp(Main* m, VecId v, VecId a, VecId b, double f)
{
    if(v.isNull())
    {
        // ERROR
        std::cerr << "Invalid vOp operation ("<<v<<','<<a<<','<<b<<','<<f<<")\n";
        return;
    }
    //std::cout << "> vOp operation ("<<v<<','<<a<<','<<b<<','<<f<<")\n";
    if (a.isNull())
    {
        if (b.isNull())
        {
            // v = 0
            if (v.type == VecId::V_COORD)
            {
                VecCoord* vv = m->getVecCoord(v.index);
                vv->fastResize(m->vsize);
                Kernels::vClear(vv->size(), vv->deviceWrite());
            }
            else
            {
                VecDeriv* vv = m->getVecDeriv(v.index);
                vv->fastResize(m->vsize);
                Kernels::vClear(vv->size(), vv->deviceWrite());
            }
        }
        else
        {
            if (b.type != v.type)
            {
                // ERROR
                std::cerr << "Invalid vOp operation ("<<v<<','<<a<<','<<b<<','<<f<<")\n";
                return;
            }
            if (v == b)
            {
                // v *= f
                if (v.type == VecId::V_COORD)
                {
                    VecCoord* vv = m->getVecCoord(v.index);
                    Kernels::vMEq(vv->size(), vv->deviceWrite(), (Real) f);
                }
                else
                {
                    VecDeriv* vv = m->getVecDeriv(v.index);
                    Kernels::vMEq(vv->size(), vv->deviceWrite(), (Real) f);
                }
            }
            else
            {
                // v = b*f
                if (v.type == VecId::V_COORD)
                {
                    VecCoord* vv = m->getVecCoord(v.index);
                    VecCoord* vb = m->getVecCoord(b.index);
                    vv->fastResize(vb->size());
                    Kernels::vEqBF(vv->size(), vv->deviceWrite(), vb->deviceRead(), (Real) f);
                }
                else
                {
                    VecDeriv* vv = m->getVecDeriv(v.index);
                    VecDeriv* vb = m->getVecDeriv(b.index);
                    vv->fastResize(vb->size());
                    Kernels::vEqBF(vv->size(), vv->deviceWrite(), vb->deviceRead(), (Real) f);
                }
            }
        }
    }
    else
    {
        if (a.type != v.type)
        {
            // ERROR
            std::cerr << "Invalid vOp operation ("<<v<<','<<a<<','<<b<<','<<f<<")\n";
            return;
        }
        if (b.isNull())
        {
            // v = a
            if (v.type == VecId::V_COORD)
            {
                VecCoord* vv = m->getVecCoord(v.index);
                VecCoord* va = m->getVecCoord(a.index);
                vv->fastResize(va->size());
                Kernels::vAssign(vv->size(), vv->deviceWrite(), va->deviceRead());
            }
            else
            {
                VecDeriv* vv = m->getVecDeriv(v.index);
                VecDeriv* va = m->getVecDeriv(a.index);
                vv->fastResize(va->size());
                Kernels::vAssign(vv->size(), vv->deviceWrite(), va->deviceRead());
            }
        }
        else
        {
            if (v == a)
            {
                if (f==1.0)
                {
                    // v += b
                    if (v.type == VecId::V_COORD)
                    {
                        VecCoord* vv = m->getVecCoord(v.index);
                        if (b.type == VecId::V_COORD)
                        {
                            VecCoord* vb = m->getVecCoord(b.index);
                            vv->resize(vb->size());
                            Kernels::vPEq(vv->size(), vv->deviceWrite(), vb->deviceRead());
                        }
                        else
                        {
                            VecDeriv* vb = m->getVecDeriv(b.index);
                            vv->resize(vb->size());
                            Kernels::vPEq(vv->size(), vv->deviceWrite(), vb->deviceRead());
                        }
                    }
                    else if (b.type == VecId::V_DERIV)
                    {
                        VecDeriv* vv = m->getVecDeriv(v.index);
                        VecDeriv* vb = m->getVecDeriv(b.index);
                        vv->resize(vb->size());
                        Kernels::vPEq(vv->size(), vv->deviceWrite(), vb->deviceRead());
                    }
                    else
                    {
                        // ERROR
                        std::cerr << "Invalid vOp operation ("<<v<<','<<a<<','<<b<<','<<f<<")\n";
                        return;
                    }
                }
                else
                {
                    // v += b*f
                    if (v.type == VecId::V_COORD)
                    {
                        VecCoord* vv = m->getVecCoord(v.index);
                        if (b.type == VecId::V_COORD)
                        {
                            VecCoord* vb = m->getVecCoord(b.index);
                            vv->resize(vb->size());
                            Kernels::vPEqBF(vv->size(), vv->deviceWrite(), vb->deviceRead(), (Real)f);
                        }
                        else
                        {
                            VecDeriv* vb = m->getVecDeriv(b.index);
                            vv->resize(vb->size());
                            Kernels::vPEqBF(vv->size(), vv->deviceWrite(), vb->deviceRead(), (Real)f);
                        }
                    }
                    else if (b.type == VecId::V_DERIV)
                    {
                        VecDeriv* vv = m->getVecDeriv(v.index);
                        VecDeriv* vb = m->getVecDeriv(b.index);
                        vv->resize(vb->size());
                        Kernels::vPEqBF(vv->size(), vv->deviceWrite(), vb->deviceRead(), (Real)f);
                    }
                    else
                    {
                        // ERROR
                        std::cerr << "Invalid vOp operation ("<<v<<','<<a<<','<<b<<','<<f<<")\n";
                        return;
                    }
                }
            }
            else
            {
                if (f==1.0)
                {
                    // v = a+b
                    if (v.type == VecId::V_COORD)
                    {
                        VecCoord* vv = m->getVecCoord(v.index);
                        VecCoord* va = m->getVecCoord(a.index);
                        vv->fastResize(va->size());
                        if (b.type == VecId::V_COORD)
                        {
                            VecCoord* vb = m->getVecCoord(b.index);
                            Kernels::vAdd(vv->size(), vv->deviceWrite(), va->deviceRead(), vb->deviceRead());
                        }
                        else
                        {
                            VecDeriv* vb = m->getVecDeriv(b.index);
                            Kernels::vAdd(vv->size(), vv->deviceWrite(), va->deviceRead(), vb->deviceRead());
                        }
                    }
                    else if (b.type == VecId::V_DERIV)
                    {
                        VecDeriv* vv = m->getVecDeriv(v.index);
                        VecDeriv* va = m->getVecDeriv(a.index);
                        VecDeriv* vb = m->getVecDeriv(b.index);
                        vv->fastResize(va->size());
                        Kernels::vAdd(vv->size(), vv->deviceWrite(), va->deviceRead(), vb->deviceRead());
                    }
                    else
                    {
                        // ERROR
                        std::cerr << "Invalid vOp operation ("<<v<<','<<a<<','<<b<<','<<f<<")\n";
                        return;
                    }
                }
                else
                {
                    // v = a+b*f
                    if (v.type == VecId::V_COORD)
                    {
                        VecCoord* vv = m->getVecCoord(v.index);
                        VecCoord* va = m->getVecCoord(a.index);
                        vv->fastResize(va->size());
                        if (b.type == VecId::V_COORD)
                        {
                            VecCoord* vb = m->getVecCoord(b.index);
                            Kernels::vOp(vv->size(), vv->deviceWrite(), va->deviceRead(), vb->deviceRead(), (Real)f);
                        }
                        else
                        {
                            VecDeriv* vb = m->getVecDeriv(b.index);
                            Kernels::vOp(vv->size(), vv->deviceWrite(), va->deviceRead(), vb->deviceRead(), (Real)f);
                        }
                    }
                    else if (b.type == VecId::V_DERIV)
                    {
                        VecDeriv* vv = m->getVecDeriv(v.index);
                        VecDeriv* va = m->getVecDeriv(a.index);
                        VecDeriv* vb = m->getVecDeriv(b.index);
                        vv->fastResize(va->size());
                        Kernels::vOp(vv->size(), vv->deviceWrite(), va->deviceRead(), vb->deviceRead(), (Real)f);
                    }
                    else
                    {
                        // ERROR
                        std::cerr << "Invalid vOp operation ("<<v<<','<<a<<','<<b<<','<<f<<")\n";
                        return;
                    }
                }
            }
        }
    }
    //std::cout << "< vOp operation ("<<v<<','<<a<<','<<b<<','<<f<<")\n";
}

template<class TCoord, class TDeriv, class TReal>
void MechanicalObjectInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::vMultiOp(Main* m, const VMultiOp& ops)
{
    // optimize common integration case: v += a*dt, x += v*dt
    if (ops.size() == 2 && ops[0].second.size() == 2 && ops[0].first == ops[0].second[0].first && ops[0].first.type == VecId::V_DERIV && ops[0].second[1].first.type == VecId::V_DERIV
        && ops[1].second.size() == 2 && ops[1].first == ops[1].second[0].first && ops[0].first == ops[1].second[1].first && ops[1].first.type == VecId::V_COORD)
    {
        VecDeriv* va = m->getVecDeriv(ops[0].second[1].first.index);
        VecDeriv* vv = m->getVecDeriv(ops[0].first.index);
        VecCoord* vx = m->getVecCoord(ops[1].first.index);
        const unsigned int n = vx->size();
        const double f_v_v = ops[0].second[0].second;
        const double f_v_a = ops[0].second[1].second;
        const double f_x_x = ops[1].second[0].second;
        const double f_x_v = ops[1].second[1].second;
        Kernels::vIntegrate(n, va->deviceRead(), vv->deviceWrite(), vx->deviceWrite(), (Real)f_v_v, (Real)f_v_a, (Real)f_x_x, (Real)f_x_v);
    }
    // optimize common CG step: x += a*p, q -= a*v
    else if (ops.size() == 2 && ops[0].second.size() == 2 && ops[0].first == ops[0].second[0].first && ops[0].second[0].second == 1.0 && ops[0].first.type == VecId::V_DERIV && ops[0].second[1].first.type == VecId::V_DERIV
            && ops[1].second.size() == 2 && ops[1].first == ops[1].second[0].first && ops[1].second[0].second == 1.0 && ops[1].first.type == VecId::V_DERIV && ops[1].second[1].first.type == VecId::V_DERIV)
    {
        VecDeriv* vv1 = m->getVecDeriv(ops[0].second[1].first.index);
        VecDeriv* vres1 = m->getVecDeriv(ops[0].first.index);
        VecDeriv* vv2 = m->getVecDeriv(ops[1].second[1].first.index);
        VecDeriv* vres2 = m->getVecDeriv(ops[1].first.index);
        const unsigned int n = vres1->size();
        const double f1 = ops[0].second[1].second;
        const double f2 = ops[1].second[1].second;
        Kernels::vPEqBF2(n, vres1->deviceWrite(), vv1->deviceRead(), f1, vres2->deviceWrite(), vv2->deviceRead(), f2);
    }
    // optimize a pair of generic vOps
    else if (ops.size()==2 && ops[0].second.size()==2 && ops[0].second[0].second == 1.0 && ops[1].second.size()==2 && ops[1].second[0].second == 1.0)
    {
        const unsigned int n = m->getSize();
        Kernels::vOp2(n,
                (ops[0].first.type == VecId::V_COORD) ? m->getVecCoord(ops[0].first.index)->deviceWrite() : m->getVecDeriv(ops[0].first.index)->deviceWrite(),
                (ops[0].second[0].first.type == VecId::V_COORD) ? m->getVecCoord(ops[0].second[0].first.index)->deviceRead() : m->getVecDeriv(ops[0].second[0].first.index)->deviceRead(),
                (ops[0].second[1].first.type == VecId::V_COORD) ? m->getVecCoord(ops[0].second[1].first.index)->deviceRead() : m->getVecDeriv(ops[0].second[1].first.index)->deviceRead(),
                ops[0].second[1].second,
                (ops[1].first.type == VecId::V_COORD) ? m->getVecCoord(ops[1].first.index)->deviceWrite() : m->getVecDeriv(ops[1].first.index)->deviceWrite(),
                (ops[1].second[0].first.type == VecId::V_COORD) ? m->getVecCoord(ops[1].second[0].first.index)->deviceRead() : m->getVecDeriv(ops[1].second[0].first.index)->deviceRead(),
                (ops[1].second[1].first.type == VecId::V_COORD) ? m->getVecCoord(ops[1].second[1].first.index)->deviceRead() : m->getVecDeriv(ops[1].second[1].first.index)->deviceRead(),
                ops[1].second[1].second);
    }
    // optimize a pair of 4-way accumulations (such as at the end of RK4)
    else if (ops.size()==2 && ops[0].second.size()==5 && ops[0].second[0].first == ops[0].first && ops[0].second[0].second == 1.0 &&
            ops[1].second.size()==5 && ops[1].second[0].first == ops[1].first && ops[1].second[0].second == 1.0)
    {
        const unsigned int n = m->getSize();
        Kernels::vPEq4BF2(n,
                (ops[0].first.type == VecId::V_COORD) ? m->getVecCoord(ops[0].first.index)->deviceWrite() : m->getVecDeriv(ops[0].first.index)->deviceWrite(),
                (ops[0].second[1].first.type == VecId::V_COORD) ? m->getVecCoord(ops[0].second[1].first.index)->deviceRead() : m->getVecDeriv(ops[0].second[1].first.index)->deviceRead(),
                ops[0].second[1].second,
                (ops[0].second[2].first.type == VecId::V_COORD) ? m->getVecCoord(ops[0].second[2].first.index)->deviceRead() : m->getVecDeriv(ops[0].second[2].first.index)->deviceRead(),
                ops[0].second[2].second,
                (ops[0].second[3].first.type == VecId::V_COORD) ? m->getVecCoord(ops[0].second[3].first.index)->deviceRead() : m->getVecDeriv(ops[0].second[3].first.index)->deviceRead(),
                ops[0].second[3].second,
                (ops[0].second[4].first.type == VecId::V_COORD) ? m->getVecCoord(ops[0].second[4].first.index)->deviceRead() : m->getVecDeriv(ops[0].second[4].first.index)->deviceRead(),
                ops[0].second[4].second,
                (ops[1].first.type == VecId::V_COORD) ? m->getVecCoord(ops[1].first.index)->deviceWrite() : m->getVecDeriv(ops[1].first.index)->deviceWrite(),
                (ops[1].second[1].first.type == VecId::V_COORD) ? m->getVecCoord(ops[1].second[1].first.index)->deviceRead() : m->getVecDeriv(ops[1].second[1].first.index)->deviceRead(),
                ops[1].second[1].second,
                (ops[1].second[2].first.type == VecId::V_COORD) ? m->getVecCoord(ops[1].second[2].first.index)->deviceRead() : m->getVecDeriv(ops[1].second[2].first.index)->deviceRead(),
                ops[1].second[2].second,
                (ops[1].second[3].first.type == VecId::V_COORD) ? m->getVecCoord(ops[1].second[3].first.index)->deviceRead() : m->getVecDeriv(ops[1].second[3].first.index)->deviceRead(),
                ops[1].second[3].second,
                (ops[1].second[4].first.type == VecId::V_COORD) ? m->getVecCoord(ops[1].second[4].first.index)->deviceRead() : m->getVecDeriv(ops[1].second[4].first.index)->deviceRead(),
                ops[1].second[4].second);
    }
    else // no optimization for now for other cases
    {
        std::cout << "CUDA: unoptimized vMultiOp:"<<std::endl;
        for (unsigned int i=0; i<ops.size(); ++i)
        {
            std::cout << ops[i].first << " =";
            if (ops[i].second.empty())
                std::cout << "0";
            else
                for (unsigned int j=0; j<ops[i].second.size(); ++j)
                {
                    if (j) std::cout << " + ";
                    std::cout << ops[i].second[j].first << "*" << ops[i].second[j].second;
                }
            std::cout << endl;
        }
        {
            using namespace sofa::core::componentmodel::behavior;
            m->BaseMechanicalState::vMultiOp(ops);
        }
    }
}

template<class TCoord, class TDeriv, class TReal>
double MechanicalObjectInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::vDot(Main* m, VecId a, VecId b)
{
    Real r = 0.0f;
    if (a.type == VecId::V_COORD && b.type == VecId::V_COORD)
    {
        VecCoord* va = m->getVecCoord(a.index);
        VecCoord* vb = m->getVecCoord(b.index);
        int tmpsize = Kernels::vDotTmpSize(va->size());
        if (tmpsize == 0)
        {
            Kernels::vDot(va->size(), &r, va->deviceRead(), vb->deviceRead(), NULL, NULL);
        }
        else
        {
            m->data.tmpdot.fastResize(tmpsize);
            Kernels::vDot(va->size(), &r, va->deviceRead(), vb->deviceRead(), m->data.tmpdot.deviceWrite(), (Real*)(&(m->data.tmpdot.getCached(0))));
        }
    }
    else if (a.type == VecId::V_DERIV && b.type == VecId::V_DERIV)
    {
        VecDeriv* va = m->getVecDeriv(a.index);
        VecDeriv* vb = m->getVecDeriv(b.index);
        int tmpsize = Kernels::vDotTmpSize(va->size());
        if (tmpsize == 0)
        {
            Kernels::vDot(va->size(), &r, va->deviceRead(), vb->deviceRead(), NULL, NULL);
        }
        else
        {
            m->data.tmpdot.fastResize(tmpsize);
            Kernels::vDot(va->size(), &r, va->deviceRead(), vb->deviceRead(), m->data.tmpdot.deviceWrite(), (Real*)(&(m->data.tmpdot.getCached(0))));
        }
#ifndef NDEBUG
        // Check the result
        //Real r2 = 0.0f;
        //for (unsigned int i=0; i<va->size(); i++)
        //	r2 += (*va)[i] * (*vb)[i];
        //std::cout << "CUDA vDot: GPU="<<r<<"  CPU="<<r2<<" relative error="<<(fabsf(r2)>0.000001?fabsf(r-r2)/fabsf(r2):fabsf(r-r2))<<"\n";
#endif
    }
    else
    {
        std::cerr << "Invalid dot operation ("<<a<<','<<b<<")\n";
    }
    return r;
}

template<class TCoord, class TDeriv, class TReal>
void MechanicalObjectInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::resetForce(Main* m)
{
    VecDeriv& f= *m->getF();
    Kernels::vClear(f.size(), f.deviceWrite());
}

// I know using macros is bad design but this is the only way not to repeat the code for all CUDA types
#define CudaMechanicalObject_ImplMethods(T) \
    template<> void MechanicalObject< T >::accumulateForce() \
    { data.accumulateForce(this); } \
    template<> void MechanicalObject< T >::vOp(VecId v, VecId a, VecId b, double f) \
    { data.vOp(this, v, a, b, f); } \
    template<> void MechanicalObject< T >::vMultiOp(const VMultiOp& ops) \
    { data.vMultiOp(this, ops); } \
    template<> double MechanicalObject< T >::vDot(VecId a, VecId b) \
    { return data.vDot(this, a, b); } \
    template<> void MechanicalObject< T >::resetForce() \
    { data.resetForce(this); }

CudaMechanicalObject_ImplMethods(gpu::cuda::CudaVec3fTypes);
CudaMechanicalObject_ImplMethods(gpu::cuda::CudaVec3f1Types);

#ifdef SOFA_GPU_CUDA_DOUBLE

CudaMechanicalObject_ImplMethods(gpu::cuda::CudaVec3dTypes);
CudaMechanicalObject_ImplMethods(gpu::cuda::CudaVec3d1Types);

#endif // SOFA_GPU_CUDA_DOUBLE

#undef CudaMechanicalObject_ImplMethods

} // namespace component

} // namespace sofa

#endif
