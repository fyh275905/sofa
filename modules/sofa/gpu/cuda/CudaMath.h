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
#ifndef CUDAMATH_H
#define CUDAMATH_H

#include <cuda_runtime.h>
#include <cuda.h>

#if defined(__cplusplus) && CUDA_VERSION < 2000
namespace sofa
{
namespace gpu
{
namespace cuda
{
#endif

template<class real>
class CudaVec2;

template<class real>
class CudaVec3;

template<class real>
class CudaVec4;

template<>
class CudaVec2<float> : public float2
{
public:
    typedef float Real;
    static __inline__ __device__ __host__ CudaVec2<float> make(Real x, Real y)
    {
        CudaVec2<float> r; r.x = x; r.y = y; return r;
    }
    static __inline__ __device__ __host__ CudaVec2<float> make(float2 v)
    {
        CudaVec2<float> r; r.x = v.x; r.y = v.y; return r;
    }
    static __inline__ __device__ __host__ CudaVec2<float> make(float3 v)
    {
        CudaVec2<float> r; r.x = v.x; r.y = v.y; return r;
    }
};

template<>
class CudaVec3<float> : public float3
{
public:
    typedef float Real;
    static __inline__ __device__ __host__ CudaVec3<float> make(Real x, Real y, Real z=0)
    {
        CudaVec3<float> r; r.x = x; r.y = y;  r.z = z; return r;
    }
    static __inline__ __device__ __host__ CudaVec3<float> make(float2 v, Real z=0)
    {
        CudaVec3<float> r; r.x = v.x; r.y = v.y;  r.z = z; return r;
    }
    static __inline__ __device__ __host__ CudaVec3<float> make(float3 v)
    {
        CudaVec3<float> r; r.x = v.x; r.y = v.y;  r.z = v.z; return r;
    }
    static __inline__ __device__ __host__ CudaVec3<float> make(float4 v)
    {
        CudaVec3<float> r; r.x = v.x; r.y = v.y;  r.z = v.z; return r;
    }
};

template<>
class CudaVec4<float> : public float4
{
public:
    typedef float Real;
    static __inline__ __device__ __host__ CudaVec4<float> make(Real x, Real y, Real z, Real w=0)
    {
        CudaVec4<float> r; r.x = x; r.y = y;  r.z = z; r.w = w; return r;
    }
    static __inline__ __device__ __host__ CudaVec4<float> make(float3 v, Real w=0)
    {
        CudaVec4<float> r; r.x = v.x; r.y = v.y;  r.z = v.z; r.w = w; return r;
    }
    static __inline__ __device__ __host__ CudaVec4<float> make(float4 v)
    {
        CudaVec4<float> r; r.x = v.x; r.y = v.y;  r.z = v.z; r.w = v.w; return r;
    }
};

typedef CudaVec2<float> CudaVec2f;
typedef CudaVec3<float> CudaVec3f;
typedef CudaVec4<float> CudaVec4f;

#ifdef SOFA_GPU_CUDA_DOUBLE

class __align__(8) double3
{
public:
    double x, y, z;
};

class __align__(16) double4
{
public:
    double x, y, z, w;
};

template<>
class CudaVec2<double> : public double2
{
public:
    typedef double Real;
    static __inline__ __device__ __host__ CudaVec2<double> make(Real x, Real y)
    {
        CudaVec2<double> r; r.x = x; r.y = y; return r;
    }
    static __inline__ __device__ __host__ CudaVec2<double> make(double2 v)
    {
        CudaVec2<double> r; r.x = v.x; r.y = v.y; return r;
    }
    static __inline__ __device__ __host__ CudaVec2<double> make(double3 v)
    {
        CudaVec2<double> r; r.x = v.x; r.y = v.y; return r;
    }
};

template<>
class CudaVec3<double> : public double3
{
public:
    typedef double Real;
    static __inline__ __device__ __host__ CudaVec3<double> make(Real x, Real y, Real z=0)
    {
        CudaVec3<double> r; r.x = x; r.y = y;  r.z = z; return r;
    }
    static __inline__ __device__ __host__ CudaVec3<double> make(double2 v, Real z=0)
    {
        CudaVec3<double> r; r.x = v.x; r.y = v.y;  r.z = z; return r;
    }
    static __inline__ __device__ __host__ CudaVec3<double> make(double3 v)
    {
        CudaVec3<double> r; r.x = v.x; r.y = v.y;  r.z = v.z; return r;
    }
    static __inline__ __device__ __host__ CudaVec3<double> make(double4 v)
    {
        CudaVec3<double> r; r.x = v.x; r.y = v.y;  r.z = v.z; return r;
    }
};

template<>
class CudaVec4<double> : public double4
{
public:
    typedef double Real;
    static __inline__ __device__ __host__ CudaVec4<double> make(Real x, Real y, Real z, Real w=0)
    {
        CudaVec4<double> r; r.x = x; r.y = y;  r.z = z; r.w = w; return r;
    }
    static __inline__ __device__ __host__ CudaVec4<double> make(double3 v, Real w=0)
    {
        CudaVec4<double> r; r.x = v.x; r.y = v.y;  r.z = v.z; r.w = w; return r;
    }
    static __inline__ __device__ __host__ CudaVec4<double> make(double4 v)
    {
        CudaVec4<double> r; r.x = v.x; r.y = v.y;  r.z = v.z; r.w = v.w; return r;
    }
};

typedef CudaVec2<double> CudaVec2d;
typedef CudaVec3<double> CudaVec3d;
typedef CudaVec4<double> CudaVec4d;

#endif // SOFA_GPU_CUDA_DOUBLE

template<class real>
__device__ CudaVec3<real> operator+(CudaVec3<real> a, CudaVec3<real> b)
{
    return CudaVec3<real>::make(a.x+b.x, a.y+b.y, a.z+b.z);
}

template<class real>
__device__ void operator+=(CudaVec3<real>& a, CudaVec3<real> b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
}

template<class real>
__device__ void operator+=(CudaVec3<real>& a, real b)
{
    a.x += b;
    a.y += b;
    a.z += b;
}

template<class real>
__device__ CudaVec3<real> operator-(CudaVec3<real> a, CudaVec3<real> b)
{
    return CudaVec3<real>::make(a.x-b.x, a.y-b.y, a.z-b.z);
}

template<class real>
__device__ void operator-=(CudaVec3<real>& a, CudaVec3<real> b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
}

template<class real>
__device__ void operator-=(CudaVec3<real>& a, real b)
{
    a.x -= b;
    a.y -= b;
    a.z -= b;
}

template<class real>
__device__ CudaVec3<real> operator-(CudaVec3<real>& a)
{
    return CudaVec3<real>::make(-a.x, -a.y, -a.z);
}

template<class real>
__device__ CudaVec3<real> operator*(CudaVec3<real> a, real b)
{
    return CudaVec3<real>::make(a.x*b, a.y*b, a.z*b);
}

template<class real>
__device__ CudaVec3<real> operator/(CudaVec3<real> a, real b)
{
    return CudaVec3<real>::make(a.x/b, a.y/b, a.z/b);
}

template<class real>
__device__ void operator*=(CudaVec3<real>& a, real b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
}

template<class real>
__device__ CudaVec3<real> operator*(real a, CudaVec3<real> b)
{
    return CudaVec3<real>::make(a*b.x, a*b.y, a*b.z);
}

template<class real>
__device__ CudaVec3<real> mul(CudaVec3<real> a, CudaVec3<real> b)
{
    return CudaVec3<real>::make(a.x*b.x, a.y*b.y, a.z*b.z);
}

template<class real>
__device__ real dot(CudaVec3<real> a, CudaVec3<real> b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

template<class real>
__device__ CudaVec3<real> cross(CudaVec3<real> a, CudaVec3<real> b)
{
    return CudaVec3<real>::make(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

template<class real>
__device__ real norm2(CudaVec3<real> a)
{
    return a.x*a.x+a.y*a.y+a.z*a.z;
}

template<class real>
__device__ real norm(CudaVec3<real> a)
{
    return sqrtf(norm2(a));
}

template<class real>
__device__ real invnorm(CudaVec3<real> a)
{
    return rsqrtf(norm2(a));
}

template<class real>
class /*__align__(4)*/ matrix3
{
public:
    CudaVec3<real> x,y,z;
    /*
        CudaVec3<real> getLx() { return x; }
        CudaVec3<real> getLy() { return y; }
        CudaVec3<real> getLz() { return z; }

        CudaVec3<real> getCx() { return CudaVec3<real>::make(x.x,y.x,z.x); }
        CudaVec3<real> getCy() { return CudaVec3<real>::make(x.y,y.y,z.y); }
        CudaVec3<real> getCz() { return CudaVec3<real>::make(x.z,y.z,z.z); }

        void setLx(CudaVec3<real> v) { x = v; }
        void setLy(CudaVec3<real> v) { y = v; }
        void setLz(CudaVec3<real> v) { z = v; }

        void setCx(CudaVec3<real> v) { x.x = v.x; y.x = v.y; z.x = v.z; }
        void setCy(CudaVec3<real> v) { x.y = v.x; y.y = v.y; z.y = v.z; }
        void setCz(CudaVec3<real> v) { x.z = v.x; y.z = v.y; z.z = v.z; }
    */
    __device__ CudaVec3<real> operator*(CudaVec3<real> v)
    {
        return CudaVec3<real>::make(dot(x,v),dot(y,v),dot(z,v));
    }
    __device__ CudaVec3<real> mulT(CudaVec3<real> v)
    {
        return x*v.x+y*v.y+z*v.z;
    }
    __device__ matrix3<real> operator*(matrix3<real> v)
    {
        matrix3<real> r;
        r.x.x = x.x * v.x.x + x.y * v.y.x + x.z * v.z.x;
        r.x.y = x.x * v.x.y + x.y * v.y.y + x.z * v.z.y;
        r.x.z = x.x * v.x.z + x.y * v.y.z + x.z * v.z.z;

        r.y.x = y.x * v.x.x + y.y * v.y.x + y.z * v.z.x;
        r.y.y = y.x * v.x.y + y.y * v.y.y + y.z * v.z.y;
        r.y.z = y.x * v.x.z + y.y * v.y.z + y.z * v.z.z;

        r.z.x = z.x * v.x.x + z.y * v.y.x + z.z * v.z.x;
        r.z.y = z.x * v.x.y + z.y * v.y.y + z.z * v.z.y;
        r.z.z = z.x * v.x.z + z.y * v.y.z + z.z * v.z.z;
        return r;
    }
    __device__ matrix3<real> mulT(matrix3<real> v)
    {
        matrix3<real> r;
        r.x.x = x.x * v.x.x + y.x * v.y.x + z.x * v.z.x;
        r.x.y = x.x * v.x.y + y.x * v.y.y + z.x * v.z.y;
        r.x.z = x.x * v.x.z + y.x * v.y.z + z.x * v.z.z;

        r.y.x = x.y * v.x.x + y.y * v.y.x + z.y * v.z.x;
        r.y.y = x.y * v.x.y + y.y * v.y.y + z.y * v.z.y;
        r.y.z = x.y * v.x.z + y.y * v.y.z + z.y * v.z.z;

        r.z.x = x.z * v.x.x + y.z * v.y.x + z.z * v.z.x;
        r.z.y = x.z * v.x.y + y.z * v.y.y + z.z * v.z.y;
        r.z.z = x.z * v.x.z + y.z * v.y.z + z.z * v.z.z;
        return r;
    }
    __device__ real mulX(CudaVec3<real> v)
    {
        return dot(x,v);
    }
    __device__ real mulY(CudaVec3<real> v)
    {
        return dot(y,v);
    }
    __device__ real mulZ(CudaVec3<real> v)
    {
        return dot(z,v);
    }
    __device__ void readAoS(const real* data, int bsize = blockDim.x)
    {
        x.x=*data; data+=bsize;
        x.y=*data; data+=bsize;
        x.z=*data; data+=bsize;
        y.x=*data; data+=bsize;
        y.y=*data; data+=bsize;
        y.z=*data; data+=bsize;
        z.x=*data; data+=bsize;
        z.y=*data; data+=bsize;
        z.z=*data; data+=bsize;
    }
    __device__ void writeAoS(real* data, int bsize = blockDim.x)
    {
        *data=x.x; data+=bsize;
        *data=x.y; data+=bsize;
        *data=x.z; data+=bsize;
        *data=y.x; data+=bsize;
        *data=y.y; data+=bsize;
        *data=y.z; data+=bsize;
        *data=z.x; data+=bsize;
        *data=z.y; data+=bsize;
        *data=z.z; data+=bsize;
    }
};

#if defined(__cplusplus) && CUDA_VERSION < 2000
} // namespace cuda
} // namespace gpu
} // namespace sofa
#endif

#endif
