#ifndef SOFA_GPU_CUDA_CUDASPRINGFORCEFIELD_INL
#define SOFA_GPU_CUDA_CUDASPRINGFORCEFIELD_INL

#include "CudaSpringForceField.h"
#include <sofa/component/forcefield/SpringForceField.inl>
#include <sofa/component/forcefield/StiffSpringForceField.inl>
#include <sofa/component/forcefield/MeshSpringForceField.inl>
#include <sofa/component/forcefield/TriangleBendingSprings.inl>
#include <sofa/component/forcefield/QuadBendingSprings.inl>

namespace sofa
{

namespace gpu
{

namespace cuda
{

extern "C"
{
    void SpringForceFieldCuda3f_addForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f, const void* x, const void* v);
    void SpringForceFieldCuda3f_addExternalForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f1, const void* x1, const void* v1, const void* x2, const void* v2);
    void StiffSpringForceFieldCuda3f_addForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f, const void* x, const void* v, void* dfdx);
    void StiffSpringForceFieldCuda3f_addExternalForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f1, const void* x1, const void* v1, const void* x2, const void* v2, void* dfdx);
    void StiffSpringForceFieldCuda3f_addDForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f, const void* dx, const void* x, const void* dfdx);
    void StiffSpringForceFieldCuda3f_addExternalDForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f1, const void* dx1, const void* x1, const void* dx2, const void* x2, const void* dfdx);

    void SpringForceFieldCuda3f1_addForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f, const void* x, const void* v);
    void SpringForceFieldCuda3f1_addExternalForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f1, const void* x1, const void* v1, const void* x2, const void* v2);
    void StiffSpringForceFieldCuda3f1_addForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f, const void* x, const void* v, void* dfdx);
    void StiffSpringForceFieldCuda3f1_addExternalForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f1, const void* x1, const void* v1, const void* x2, const void* v2, void* dfdx);
    void StiffSpringForceFieldCuda3f1_addDForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f, const void* dx, const void* x, const void* dfdx);
    void StiffSpringForceFieldCuda3f1_addExternalDForce(unsigned int nbVertex, unsigned int nbSpringPerVertex, const void* springs, void* f1, const void* dx1, const void* x1, const void* dx2, const void* x2, const void* dfdx);
}

} // namespace cuda

} // namespace gpu

namespace component
{

namespace forcefield
{

using namespace gpu::cuda;

template <>
void SpringForceField<CudaVec3fTypes>::init()
{
    this->Inherit::init();
    const sofa::helper::vector<Spring>& springs = this->springs.getValue();
    if (!springs.empty())
    {
        bool external = (this->mstate1!=this->mstate2);
        if (external)
        {
            std::map<int,int> nsprings1;
            std::map<int,int> nsprings2;
            for (unsigned int i=0; i<springs.size(); i++)
            {
                nsprings1[springs[i].m1]++;
                nsprings2[springs[i].m2]++;
            }

            int nmax1 = 0;
            for (std::map<int,int>::const_iterator it = nsprings1.begin(); it != nsprings1.end(); ++it)
                if (it->second > nmax1)
                    nmax1 = it->second;
            data.springs1.init(nsprings1.begin()->first, nsprings1.rbegin()->first - nsprings1.begin()->first + 1, nmax1);

            int nmax2 = 0;
            for (std::map<int,int>::const_iterator it = nsprings2.begin(); it != nsprings2.end(); ++it)
                if (it->second > nmax2)
                    nmax2 = it->second;
            data.springs2.init(nsprings2.begin()->first, nsprings2.rbegin()->first - nsprings2.begin()->first + 1, nmax2);

            nsprings1.clear();
            nsprings2.clear();
            for (unsigned int i=0; i<springs.size(); i++)
            {
                int m1 = springs[i].m1 - data.springs1.vertex0;
                int m2 = springs[i].m2 - data.springs2.vertex0;
                data.springs1.set(m1, nsprings1[m1]++, m2,
                        (float)springs[i].initpos,
                        (float)springs[i].ks,
                        (float)springs[i].kd);
                data.springs2.set(m2, nsprings2[m2]++, m1,
                        (float)springs[i].initpos,
                        (float)springs[i].ks,
                        (float)springs[i].kd);
            }
        }
        else
        {
            std::map<int,int> nsprings;
            for (unsigned int i=0; i<springs.size(); i++)
            {
                nsprings[springs[i].m1]++;
                nsprings[springs[i].m2]++;
            }

            int nmax = 0;
            for (std::map<int,int>::const_iterator it = nsprings.begin(); it != nsprings.end(); ++it)
                if (it->second > nmax)
                    nmax = it->second;
            data.springs1.init(nsprings.begin()->first, nsprings.rbegin()->first - nsprings.begin()->first + 1, nmax);
            std::cout << "CUDA SpringForceField: "<<springs.size()<<" springs, "<<data.springs1.nbVertex<<" attached points, max "<<data.springs1.nbSpringPerVertex<<" springs per point."<<std::endl;
            nsprings.clear();
            for (unsigned int i=0; i<springs.size(); i++)
            {
                int m1 = springs[i].m1 - data.springs1.vertex0;
                int m2 = springs[i].m2 - data.springs1.vertex0;
                data.springs1.set(m1, nsprings[m1]++, m2,
                        (float)springs[i].initpos,
                        (float)springs[i].ks,
                        (float)springs[i].kd);
                data.springs1.set(m2, nsprings[m2]++, m1,
                        (float)springs[i].initpos,
                        (float)springs[i].ks,
                        (float)springs[i].kd);
            }
        }
    }
}

// -- InteractionForceField interface
template <>
void SpringForceField<CudaVec3fTypes>::addForce(VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2)
{
    if (this->mstate1 == this->mstate2)
    {
        VecDeriv& f = f1;
        const VecCoord& x = x1;
        const VecDeriv& v = v1;
        f.resize(x.size());
        int d = data.springs1.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            SpringForceFieldCuda3f_addForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)f.deviceWrite() + d,
                    (const Coord*)x.deviceRead()  + d,
                    (const Deriv*)v.deviceRead()  + d);
        }
    }
    else
    {
        f1.resize(x1.size());
        f2.resize(x2.size());
        int d1 = data.springs1.vertex0;
        int d2 = data.springs2.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            SpringForceFieldCuda3f_addExternalForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)f1.deviceWrite() + d1,
                    (const Coord*)x1.deviceRead()  + d1,
                    (const Deriv*)v1.deviceRead()  + d1,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)v2.deviceRead()  + d2);
        }
        if (data.springs2.nbSpringPerVertex > 0)
        {
            SpringForceFieldCuda3f_addExternalForce(data.springs2.nbVertex,
                    data.springs2.nbSpringPerVertex,
                    data.springs2.springs.deviceRead(),
                    (      Deriv*)f2.deviceWrite() + d2,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)v2.deviceRead()  + d2,
                    (const Coord*)x1.deviceRead()  + d1,
                    (const Deriv*)v1.deviceRead()  + d1);
        }
    }
}

template <>
void StiffSpringForceField<CudaVec3fTypes>::init()
{
    this->SpringForceField<CudaVec3fTypes>::init();
    data.springs1.dfdx.resize(data.springs1.springs.size());
    data.springs2.dfdx.resize(data.springs2.springs.size());
}

// -- InteractionForceField interface
template <>
void StiffSpringForceField<CudaVec3fTypes>::addForce(VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2)
{
    if (this->mstate1 == this->mstate2)
    {
        VecDeriv& f = f1;
        const VecCoord& x = x1;
        const VecDeriv& v = v1;
        f.resize(x.size());
        int d = data.springs1.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f_addForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)f.deviceWrite() + d,
                    (const Coord*)x.deviceRead()  + d,
                    (const Deriv*)v.deviceRead()  + d,
                    data.springs1.dfdx.deviceWrite());
        }
    }
    else
    {
        f1.resize(x1.size());
        f2.resize(x2.size());
        int d1 = data.springs1.vertex0;
        int d2 = data.springs2.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f_addExternalForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)f1.deviceWrite() + d1,
                    (const Coord*)x1.deviceRead()  + d1,
                    (const Deriv*)v1.deviceRead()  + d1,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)v2.deviceRead()  + d2,
                    data.springs1.dfdx.deviceWrite());
        }
        if (data.springs2.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f_addExternalForce(data.springs2.nbVertex,
                    data.springs2.nbSpringPerVertex,
                    data.springs2.springs.deviceRead(),
                    (      Deriv*)f2.deviceWrite() + d2,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)v2.deviceRead()  + d2,
                    (const Coord*)x1.deviceRead()  + d1,
                    (const Deriv*)v1.deviceRead()  + d1,
                    data.springs2.dfdx.deviceWrite());
        }
    }
}

template <>
void StiffSpringForceField<CudaVec3fTypes>::addDForce(VecDeriv& df1, VecDeriv& df2, const VecDeriv& dx1, const VecDeriv& dx2)
{
    if (this->mstate1 == this->mstate2)
    {
        VecDeriv& df = df1;
        const VecDeriv& dx = dx1;
        const VecCoord& x = *this->mstate1->getX();
        df.resize(x.size());
        int d = data.springs1.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f_addDForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)df.deviceWrite() + d,
                    (const Deriv*)dx.deviceRead() + d,
                    (const Coord*)x.deviceRead()  + d,
                    data.springs1.dfdx.deviceRead());
        }
    }
    else
    {
        const VecCoord& x1 = *this->mstate1->getX();
        const VecCoord& x2 = *this->mstate2->getX();
        df1.resize(x1.size());
        df2.resize(x2.size());
        int d1 = data.springs1.vertex0;
        int d2 = data.springs2.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f_addExternalDForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)df1.deviceWrite() + d1,
                    (const Coord*)x1.deviceRead()  + d1,
                    (const Deriv*)dx1.deviceRead()  + d1,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)dx2.deviceRead()  + d2,
                    data.springs1.dfdx.deviceRead());
        }
        if (data.springs2.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f_addExternalDForce(data.springs2.nbVertex,
                    data.springs2.nbSpringPerVertex,
                    data.springs2.springs.deviceRead(),
                    (      Deriv*)df2.deviceWrite() + d2,
                    (const Deriv*)dx2.deviceRead() + d2,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)dx1.deviceRead() + d1,
                    (const Coord*)x1.deviceRead()  + d1,
                    data.springs2.dfdx.deviceRead());
        }
    }
}




//////// CudaVec3f1

template <>
void SpringForceField<CudaVec3f1Types>::init()
{
    this->Inherit::init();
    const sofa::helper::vector<Spring>& springs = this->springs.getValue();
    if (!springs.empty())
    {
        bool external = (this->mstate1!=this->mstate2);
        if (external)
        {
            std::map<int,int> nsprings1;
            std::map<int,int> nsprings2;
            for (unsigned int i=0; i<springs.size(); i++)
            {
                nsprings1[springs[i].m1]++;
                nsprings2[springs[i].m2]++;
            }

            int nmax1 = 0;
            for (std::map<int,int>::const_iterator it = nsprings1.begin(); it != nsprings1.end(); ++it)
                if (it->second > nmax1)
                    nmax1 = it->second;
            data.springs1.init(nsprings1.begin()->first, nsprings1.rbegin()->first - nsprings1.begin()->first + 1, nmax1);

            int nmax2 = 0;
            for (std::map<int,int>::const_iterator it = nsprings2.begin(); it != nsprings2.end(); ++it)
                if (it->second > nmax2)
                    nmax2 = it->second;
            data.springs2.init(nsprings2.begin()->first, nsprings2.rbegin()->first - nsprings2.begin()->first + 1, nmax2);

            nsprings1.clear();
            nsprings2.clear();
            for (unsigned int i=0; i<springs.size(); i++)
            {
                int m1 = springs[i].m1 - data.springs1.vertex0;
                int m2 = springs[i].m2 - data.springs2.vertex0;
                data.springs1.set(m1, nsprings1[m1]++, m2,
                        (float)springs[i].initpos,
                        (float)springs[i].ks,
                        (float)springs[i].kd);
                data.springs2.set(m2, nsprings2[m2]++, m1,
                        (float)springs[i].initpos,
                        (float)springs[i].ks,
                        (float)springs[i].kd);
            }
        }
        else
        {
            std::map<int,int> nsprings;
            for (unsigned int i=0; i<springs.size(); i++)
            {
                nsprings[springs[i].m1]++;
                nsprings[springs[i].m2]++;
            }

            int nmax = 0;
            for (std::map<int,int>::const_iterator it = nsprings.begin(); it != nsprings.end(); ++it)
                if (it->second > nmax)
                    nmax = it->second;
            data.springs1.init(nsprings.begin()->first, nsprings.rbegin()->first - nsprings.begin()->first + 1, nmax);
            std::cout << "CUDA SpringForceField: "<<springs.size()<<" springs, "<<data.springs1.nbVertex<<" attached points, max "<<data.springs1.nbSpringPerVertex<<" springs per point."<<std::endl;
            nsprings.clear();
            for (unsigned int i=0; i<springs.size(); i++)
            {
                int m1 = springs[i].m1 - data.springs1.vertex0;
                int m2 = springs[i].m2 - data.springs1.vertex0;
                data.springs1.set(m1, nsprings[m1]++, m2,
                        (float)springs[i].initpos,
                        (float)springs[i].ks,
                        (float)springs[i].kd);
                data.springs1.set(m2, nsprings[m2]++, m1,
                        (float)springs[i].initpos,
                        (float)springs[i].ks,
                        (float)springs[i].kd);
            }
        }
    }
}

// -- InteractionForceField interface
template <>
void SpringForceField<CudaVec3f1Types>::addForce(VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2)
{
    if (this->mstate1 == this->mstate2)
    {
        VecDeriv& f = f1;
        const VecCoord& x = x1;
        const VecDeriv& v = v1;
        f.resize(x.size());
        int d = data.springs1.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            SpringForceFieldCuda3f1_addForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)f.deviceWrite() + d,
                    (const Coord*)x.deviceRead()  + d,
                    (const Deriv*)v.deviceRead()  + d);
        }
    }
    else
    {
        f1.resize(x1.size());
        f2.resize(x2.size());
        int d1 = data.springs1.vertex0;
        int d2 = data.springs2.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            SpringForceFieldCuda3f1_addExternalForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)f1.deviceWrite() + d1,
                    (const Coord*)x1.deviceRead()  + d1,
                    (const Deriv*)v1.deviceRead()  + d1,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)v2.deviceRead()  + d2);
        }
        if (data.springs2.nbSpringPerVertex > 0)
        {
            SpringForceFieldCuda3f1_addExternalForce(data.springs2.nbVertex,
                    data.springs2.nbSpringPerVertex,
                    data.springs2.springs.deviceRead(),
                    (      Deriv*)f2.deviceWrite() + d2,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)v2.deviceRead()  + d2,
                    (const Coord*)x1.deviceRead()  + d1,
                    (const Deriv*)v1.deviceRead()  + d1);
        }
    }
}

template <>
void StiffSpringForceField<CudaVec3f1Types>::init()
{
    this->SpringForceField<CudaVec3f1Types>::init();
    data.springs1.dfdx.resize(data.springs1.springs.size());
    data.springs2.dfdx.resize(data.springs2.springs.size());
}

// -- InteractionForceField interface
template <>
void StiffSpringForceField<CudaVec3f1Types>::addForce(VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2)
{
    if (this->mstate1 == this->mstate2)
    {
        VecDeriv& f = f1;
        const VecCoord& x = x1;
        const VecDeriv& v = v1;
        f.resize(x.size());
        int d = data.springs1.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f1_addForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)f.deviceWrite() + d,
                    (const Coord*)x.deviceRead()  + d,
                    (const Deriv*)v.deviceRead()  + d,
                    data.springs1.dfdx.deviceWrite());
        }
    }
    else
    {
        f1.resize(x1.size());
        f2.resize(x2.size());
        int d1 = data.springs1.vertex0;
        int d2 = data.springs2.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f1_addExternalForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)f1.deviceWrite() + d1,
                    (const Coord*)x1.deviceRead()  + d1,
                    (const Deriv*)v1.deviceRead()  + d1,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)v2.deviceRead()  + d2,
                    data.springs1.dfdx.deviceWrite());
        }
        if (data.springs2.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f1_addExternalForce(data.springs2.nbVertex,
                    data.springs2.nbSpringPerVertex,
                    data.springs2.springs.deviceRead(),
                    (      Deriv*)f2.deviceWrite() + d2,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)v2.deviceRead()  + d2,
                    (const Coord*)x1.deviceRead()  + d1,
                    (const Deriv*)v1.deviceRead()  + d1,
                    data.springs2.dfdx.deviceWrite());
        }
    }
}

template <>
void StiffSpringForceField<CudaVec3f1Types>::addDForce(VecDeriv& df1, VecDeriv& df2, const VecDeriv& dx1, const VecDeriv& dx2)
{
    if (this->mstate1 == this->mstate2)
    {
        VecDeriv& df = df1;
        const VecDeriv& dx = dx1;
        const VecCoord& x = *this->mstate1->getX();
        df.resize(x.size());
        int d = data.springs1.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f1_addDForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)df.deviceWrite() + d,
                    (const Deriv*)dx.deviceRead() + d,
                    (const Coord*)x.deviceRead()  + d,
                    data.springs1.dfdx.deviceRead());
        }
    }
    else
    {
        const VecCoord& x1 = *this->mstate1->getX();
        const VecCoord& x2 = *this->mstate2->getX();
        df1.resize(x1.size());
        df2.resize(x2.size());
        int d1 = data.springs1.vertex0;
        int d2 = data.springs2.vertex0;
        if (data.springs1.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f1_addExternalDForce(data.springs1.nbVertex,
                    data.springs1.nbSpringPerVertex,
                    data.springs1.springs.deviceRead(),
                    (      Deriv*)df1.deviceWrite() + d1,
                    (const Coord*)x1.deviceRead()  + d1,
                    (const Deriv*)dx1.deviceRead()  + d1,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)dx2.deviceRead()  + d2,
                    data.springs1.dfdx.deviceRead());
        }
        if (data.springs2.nbSpringPerVertex > 0)
        {
            StiffSpringForceFieldCuda3f1_addExternalDForce(data.springs2.nbVertex,
                    data.springs2.nbSpringPerVertex,
                    data.springs2.springs.deviceRead(),
                    (      Deriv*)df2.deviceWrite() + d2,
                    (const Deriv*)dx2.deviceRead() + d2,
                    (const Coord*)x2.deviceRead()  + d2,
                    (const Deriv*)dx1.deviceRead() + d1,
                    (const Coord*)x1.deviceRead()  + d1,
                    data.springs2.dfdx.deviceRead());
        }
    }
}

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
