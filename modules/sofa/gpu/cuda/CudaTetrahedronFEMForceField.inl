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
#ifndef SOFA_GPU_CUDA_CUDATETRAHEDRONFEMFORCEFIELD_INL
#define SOFA_GPU_CUDA_CUDATETRAHEDRONFEMFORCEFIELD_INL

#include "CudaTetrahedronFEMForceField.h"
#include <sofa/component/forcefield/TetrahedronFEMForceField.inl>
#include "CudaDiagonalMatrix.h"

namespace sofa
{

namespace gpu
{

namespace cuda
{

extern "C"
{
    void TetrahedronFEMForceFieldCuda3f_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v);
    void TetrahedronFEMForceFieldCuda3f_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor);
    void TetrahedronFEMForceFieldCuda3f_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor);

    void TetrahedronFEMForceFieldCuda3f1_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v);
    void TetrahedronFEMForceFieldCuda3f1_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor);
    void TetrahedronFEMForceFieldCuda3f1_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor);

    void TetrahedronFEMForceFieldCuda3f_getRotations(unsigned int nbElem, unsigned int nbVertex, const void* initState, const void* state, const void* rotationIdx, void* rotations);

    struct TetraFEMForceOp
    {
        unsigned int nbElem;
        unsigned int nbVertex;
        unsigned int nbElemPerVertex;
        const void* elems;
        void* rotations;
        void* eforce;
        const void* velems;
        void* f;
        const void* x;
        const void* v;
    };

    struct TetraFEMDForceOp
    {
        unsigned int nbElem;
        unsigned int nbVertex;
        unsigned int nbElemPerVertex;
        const void* elems;
        const void* rotations;
        void* eforce;
        const void* velems;
        void* df;
        const void* dx;
    };

    void MultiTetrahedronFEMForceFieldCuda3f_addForce(int n, TetraFEMForceOp* ops);
    void MultiTetrahedronFEMForceFieldCuda3f_addDForce(int n, TetraFEMDForceOp* ops, double factor);

#ifdef SOFA_GPU_CUDA_DOUBLE

    void TetrahedronFEMForceFieldCuda3d_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v);
    void TetrahedronFEMForceFieldCuda3d_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor);
    void TetrahedronFEMForceFieldCuda3d_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor);

    void TetrahedronFEMForceFieldCuda3d1_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v);
    void TetrahedronFEMForceFieldCuda3d1_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor);
    void TetrahedronFEMForceFieldCuda3d1_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor);

    void TetrahedronFEMForceFieldCuda3d_getRotations(unsigned int nbElem, unsigned int nbVertex, const void* initState, const void* state, const void* rotationIdx, void* rotations);

#endif // SOFA_GPU_CUDA_DOUBLE

} // extern "C"

template<>
class CudaKernelsTetrahedronFEMForceField<CudaVec3fTypes>
{
public:
    static void addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v)
    {   TetrahedronFEMForceFieldCuda3f_addForce(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, f, x, v); }
    static void addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor)
    {   TetrahedronFEMForceFieldCuda3f_addDForce(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, df, dx, factor); }
    static void addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor)
    {   TetrahedronFEMForceFieldCuda3f_addKToMatrix(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, df, factor); }

    static void getRotations(unsigned int nbElem, unsigned int nbVertex, const void* initState, const void* state, const void* rotationIdx, void* rotations)
    {   TetrahedronFEMForceFieldCuda3f_getRotations(nbElem, nbVertex, initState, state, rotationIdx, rotations); }

    static bool supportMultiAddForce() { return mycudaMultiOpMax>0; }
    static void multiAddForce(int n, TetraFEMForceOp* ops)
    {    MultiTetrahedronFEMForceFieldCuda3f_addForce(n, ops); }
    static bool supportMultiAddDForce() { return mycudaMultiOpMax>0; }
    static void multiAddDForce(int n, TetraFEMDForceOp* ops, double factor)
    {    MultiTetrahedronFEMForceFieldCuda3f_addDForce(n, ops, factor); }
};

template<>
class CudaKernelsTetrahedronFEMForceField<CudaVec3f1Types>
{
public:
    static void addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v)
    {   TetrahedronFEMForceFieldCuda3f1_addForce(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, f, x, v); }
    static void addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor)
    {   TetrahedronFEMForceFieldCuda3f1_addDForce(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, df, dx, factor); }
    static void addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor)
    {   TetrahedronFEMForceFieldCuda3f1_addKToMatrix(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, df, factor); }

    static void getRotations(unsigned int nbElem, unsigned int nbVertex, const void* initState, const void* state, const void* rotationIdx, void* rotations)
    {   TetrahedronFEMForceFieldCuda3f_getRotations(nbElem, nbVertex, initState, state, rotationIdx, rotations); }

    static bool supportMultiAddForce() { return false; }
    static void multiAddForce(int, TetraFEMForceOp*)
    {}
    static bool supportMultiAddDForce() { return false; }
    static void multiAddDForce(int, TetraFEMDForceOp*, double)
    {}
};

#ifdef SOFA_GPU_CUDA_DOUBLE

template<>
class CudaKernelsTetrahedronFEMForceField<CudaVec3dTypes>
{
public:
    static void addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v)
    {   TetrahedronFEMForceFieldCuda3d_addForce(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, f, x, v); }
    static void addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor)
    {   TetrahedronFEMForceFieldCuda3d_addDForce(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, df, dx, factor); }
    static void addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor)
    {   TetrahedronFEMForceFieldCuda3d_addKToMatrix(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, df, factor); }

    static void getRotations(unsigned int nbElem, unsigned int nbVertex, const void* initState, const void* state, const void* rotationIdx, void* rotations)
    {   TetrahedronFEMForceFieldCuda3d_getRotations(nbElem, nbVertex, initState, state, rotationIdx, rotations); }

    static bool supportMultiAddForce() { return false; }
    static void multiAddForce(int, TetraFEMForceOp*)
    {}
    static bool supportMultiAddDForce() { return false; }
    static void multiAddDForce(int, TetraFEMDForceOp*, double)
    {}
};

template<>
class CudaKernelsTetrahedronFEMForceField<CudaVec3d1Types>
{
public:
    static void addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v)
    {   TetrahedronFEMForceFieldCuda3d1_addForce(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, f, x, v); }
    static void addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor)
    {   TetrahedronFEMForceFieldCuda3d1_addDForce(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, df, dx, factor); }
    static void addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor)
    {   TetrahedronFEMForceFieldCuda3d1_addKToMatrix(nbElem, nbVertex, nbElemPerVertex, elems, state, eforce, velems, df, factor); }

    static void getRotations(unsigned int nbElem, unsigned int nbVertex, const void* initState, const void* state, const void* rotationIdx, void* rotations)
    {   TetrahedronFEMForceFieldCuda3d_getRotations(nbElem, nbVertex, initState, state, rotationIdx, rotations); }

    static bool supportMultiAddForce() { return false; }
    static void multiAddForce(int, TetraFEMForceOp*)
    {}
    static bool supportMultiAddDForce() { return false; }
    static void multiAddDForce(int, TetraFEMDForceOp*, double)
    {}
};

#endif // SOFA_GPU_CUDA_DOUBLE

} // namespace cuda

} // namespace gpu

namespace component
{

namespace forcefield
{

using namespace gpu::cuda;

template<class TCoord, class TDeriv, class TReal>
void TetrahedronFEMForceFieldInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::reinit(Main* m)
{
    Data& data = m->data;
    m->_strainDisplacements.resize( m->_indexedElements->size() );
    m->_materialsStiffnesses.resize(m->_indexedElements->size() );

    const VecElement& elems = *m->_indexedElements;

    VecCoord& p = *m->mstate->getX0();
    m->_initialPoints.setValue(p);

    m->_rotations.resize( m->_indexedElements->size() );
    m->_initialRotations.resize( m->_indexedElements->size() );
    m->_rotationIdx.resize(m->_indexedElements->size() *4);
    m->_rotatedInitialElements.resize(m->_indexedElements->size());

    std::vector<int> activeElems;
    for (unsigned int i=0; i<elems.size(); i++)
    {
#ifdef SOFA_DEV
        if (!m->_trimgrid || m->_trimgrid->isCubeActive(i/6))
#endif // SOFA_DEV
        {
            activeElems.push_back(i);
        }
    }

    for (unsigned int i=0; i<activeElems.size(); i++)
    {
        int ei = activeElems[i];
        Index a = elems[ei][0];
        Index b = elems[ei][1];
        Index c = elems[ei][2];
        Index d = elems[ei][3];
        m->computeMaterialStiffness(ei,a,b,c,d);
        m->initLarge(ei,a,b,c,d);
    }

    std::map<int,int> nelems;
    for (unsigned int i=0; i<activeElems.size(); i++)
    {
        int ei = activeElems[i];
        const Element& e = elems[ei];
        for (unsigned int j=0; j<e.size(); j++)
            ++nelems[e[j]];
    }
    int nmax = 0;
    for (std::map<int,int>::const_iterator it = nelems.begin(); it != nelems.end(); ++it)
        if (it->second > nmax)
            nmax = it->second;
    int v0 = 0;
    int nbv = 0;
    if (!nelems.empty())
    {
        v0 = nelems.begin()->first;
        nbv = nelems.rbegin()->first - v0 + 1;
    }
    data.init(activeElems.size(), v0, nbv, nmax);

    nelems.clear();
    for (unsigned int i=0; i<activeElems.size(); i++)
    {
        int ei = activeElems[i];
        const Element& e = elems[ei];
        const Coord& a = m->_rotatedInitialElements[ei][0];
        const Coord& b = m->_rotatedInitialElements[ei][1];
        const Coord& c = m->_rotatedInitialElements[ei][2];
        const Coord& d = m->_rotatedInitialElements[ei][3];
        data.setE(i, e, a, b, c, d, m->_materialsStiffnesses[ei], m->_strainDisplacements[ei]);
        for (unsigned int j=0; j<e.size(); j++)
            data.setV(e[j], nelems[e[j]]++, i*e.size()+j);
    }
}

template<class TCoord, class TDeriv, class TReal>
void TetrahedronFEMForceFieldInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::addForce(Main* m, VecDeriv& f, const VecCoord& x, const VecDeriv& v, bool prefetch)
{
    if (m->needUpdateTopology)
    {
        reinit(m);
        m->needUpdateTopology = false;
    }
    Data& data = m->data;
#ifdef SOFA_DEV
    // Count active cubes in topology
    if (m->_trimgrid)
    {
        int nactive = 0;
#ifdef SOFA_NEW_HEXA
        int ncubes = m->_trimgrid->getNbHexahedra();
#else
        int ncubes = m->_trimgrid->getNbCubes();
#endif
        for (int i=0; i<ncubes; i++)
            if (m->_trimgrid->isCubeActive(i)) ++nactive;
        if ((int)data.size() != 6*nactive)
            m->reinit();
    }
#endif // SOFA_DEV

    f.resize(x.size());

    if (prefetch)
    {
        if (!Kernels::supportMultiAddForce()) return;
        ForceOp op;
        op.nbElem = data.size();
        op.nbVertex = data.nbVertex;
        op.nbElemPerVertex = data.nbElementPerVertex;
        op.elems = data.elems.deviceRead();
        op.rotations = data.state.deviceWrite();
        op.eforce = data.eforce.deviceWrite();
        op.velems = data.velems.deviceRead();
        op.f = (      Deriv*)f.deviceWrite() + data.vertex0;
        op.x = (const Coord*)x.deviceRead()  + data.vertex0;
        op.v = (const Deriv*)v.deviceRead()  + data.vertex0;
        m->data.preForceOpID = m->data.opsForce().size();
        m->data.opsForce().push_back(op);
        return;
    }
    else if (data.preForceOpID != -1)
    {
        helper::vector<ForceOp>& ops = data.opsForce();
        if (ops.size() == 1)
        {
            ops.clear();
            data.preForceOpID = -1;
        }
        else if (ops.size() > 1)
        {
            Kernels::multiAddForce(ops.size(), &(ops[0]));
            ops.clear();
        }
        if (data.preForceOpID != -1)
        {
            data.preForceOpID = -1;
            return;
        }
    }

    Kernels::addForce(
        data.size(),
        data.nbVertex,
        data.nbElementPerVertex,
        data.elems.deviceRead(),
        data.state.deviceWrite(),
        data.eforce.deviceWrite(),
        data.velems.deviceRead(),
        (      Deriv*)f.deviceWrite() + data.vertex0,
        (const Coord*)x.deviceRead()  + data.vertex0,
        (const Deriv*)v.deviceRead()  + data.vertex0);

#if 0
    // compare with CPU version

    const VecElement& elems = *m->_indexedElements;
    for (unsigned int i=0; i<elems.size(); i++)
    {
        Index a = elems[i][0];
        Index b = elems[i][1];
        Index c = elems[i][2];
        Index d = elems[i][3];
        typename Main::Transformation Rt;
        m->computeRotationLarge(Rt, x, a, b, c);
        const GPUElementState& s = data.state[i];
        const GPUElement& e = data.elems[i];
        Mat3x3f Rdiff = Rt-s.Rt;
        if ((Rdiff[0].norm2()+Rdiff[1].norm2()+Rdiff[2].norm2()) > 0.000001f)
        {
            sout << "CPU Rt "<<i<<" = "<<Rt<<sendl;
            sout << "GPU Rt "<<i<<" = "<<s.Rt<<sendl;
            sout << "DIFF   "<<i<<" = "<<Rdiff<<sendl;
        }
        Coord xb = Rt*(x[b]-x[a]);
        Coord xc = Rt*(x[c]-x[a]);
        Coord xd = Rt*(x[d]-x[a]);

        typename Main::Displacement D;
        D[0] = 0;
        D[1] = 0;
        D[2] = 0;
        D[3] = m->_rotatedInitialElements[i][1][0] - xb[0];
        D[4] = m->_rotatedInitialElements[i][1][1] - xb[1];
        D[5] = m->_rotatedInitialElements[i][1][2] - xb[2];
        D[6] = m->_rotatedInitialElements[i][2][0] - xc[0];
        D[7] = m->_rotatedInitialElements[i][2][1] - xc[1];
        D[8] = m->_rotatedInitialElements[i][2][2] - xc[2];
        D[9] = m->_rotatedInitialElements[i][3][0] - xd[0];
        D[10]= m->_rotatedInitialElements[i][3][1] - xd[1];
        D[11]= m->_rotatedInitialElements[i][3][2] - xd[2];
        Vec<6,Real> S = -((m->_materialsStiffnesses[i]) * ((m->_strainDisplacements[i]).multTranspose(D)))*(e.bx);

        Vec<6,Real> Sdiff = S-s.S;

        if (Sdiff.norm2() > 0.0001f)
        {
            sout << "    D "<<i<<" = "<<D<<sendl;
            sout << "CPU S "<<i<<" = "<<S<<sendl;
            sout << "GPU S "<<i<<" = "<<s.S<<sendl;
            sout << "DIFF   "<<i<<" = "<<Sdiff<<sendl;
        }

    }
#endif

}

template<class TCoord, class TDeriv, class TReal>
void TetrahedronFEMForceFieldInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::addDForce (Main* m, VecDeriv& df, const VecDeriv& dx, double kFactor, double /*bFactor*/, bool prefetch)
{
    Data& data = m->data;
    df.resize(dx.size());
    if (prefetch)
    {
        if (!Kernels::supportMultiAddDForce()) return;
        DForceOp op;
        op.nbElem = data.size();
        op.nbVertex = data.nbVertex;
        op.nbElemPerVertex = data.nbElementPerVertex;
        op.elems = data.elems.deviceRead();
        op.rotations = data.state.deviceRead();
        op.eforce = data.eforce.deviceWrite();
        op.velems = data.velems.deviceRead();
        op.df = (      Deriv*)df.deviceWrite() + data.vertex0;
        op.dx = (const Deriv*)dx.deviceRead()  + data.vertex0;
        m->data.preDForceOpID = m->data.opsDForce().size();
        m->data.opsDForce().push_back(op);
        return;
    }
    else if (data.preDForceOpID != -1)
    {
        helper::vector<DForceOp>& ops = data.opsDForce();
        if (ops.size() == 1)
        {
            ops.clear();
            data.preDForceOpID = -1;
        }
        else if (ops.size() > 1)
        {
            Kernels::multiAddDForce(ops.size(), &(ops[0]), kFactor);
            ops.clear();
        }
        if (data.preDForceOpID != -1)
        {
            data.preDForceOpID = -1;
            return;
        }
    }
    Kernels::addDForce(
        data.size(),
        data.nbVertex,
        data.nbElementPerVertex,
        data.elems.deviceRead(),
        data.state.deviceRead(),
        data.eforce.deviceWrite(),
        data.velems.deviceRead(),
        (      Deriv*)df.deviceWrite() + data.vertex0,
        (const Deriv*)dx.deviceRead()  + data.vertex0,
        kFactor);
}


template<class TCoord, class TDeriv, class TReal>
void TetrahedronFEMForceFieldInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::addKToMatrix(Main* m, sofa::defaulttype::BaseMatrix *mat, double k, unsigned int &offset)
{
    Data& data = m->data;
    if (CudaDiagonalMatrix<Real> * diag = dynamic_cast<CudaDiagonalMatrix<Real> * >(mat))
    {
        Kernels::addKToMatrix(
            data.size(),
            data.nbVertex,
            data.nbElementPerVertex,
            data.elems.deviceRead(),
            data.state.deviceRead(),
            data.eforce.deviceWrite(),
            data.velems.deviceRead(),
            diag->getCudaMatrix().deviceWriteAt(offset),
            k);
    }
    else
    {
        const VecElement& elems = *m->_indexedElements;

        helper::ReadAccessor< gpu::cuda::CudaVector<GPUElementState> > state = data.state;

        // Build Matrix Block for this ForceField
        int i,j,n1, n2, row, column, ROW, COLUMN;

        typename Main::Transformation Rot;
        typename Main::StiffnessMatrix JKJt,tmp;

        Index noeud1, noeud2;

        Rot[0][0]=Rot[1][1]=Rot[2][2]=1;
        Rot[0][1]=Rot[0][2]=0;
        Rot[1][0]=Rot[1][2]=0;
        Rot[2][0]=Rot[2][1]=0;

        for (int ei=0; ei<data.nbElement; ++ei)
        {
            const Element& e = elems[ei];

            int blockIdx = ei / BSIZE;
            int threadIdx = ei % BSIZE;
            for(i=0; i<3; i++)
                for (j=0; j<3; j++)
                    Rot[j][i] = state[blockIdx].Rt[i][j][threadIdx];

            m->computeStiffnessMatrix(JKJt, tmp, m->_materialsStiffnesses[ei], m->_strainDisplacements[ei], Rot);

            // find index of node 1
            for (n1=0; n1<4; n1++)
            {
                noeud1 = e[n1];

                for(i=0; i<3; i++)
                {
                    ROW = offset+3*noeud1+i;
                    row = 3*n1+i;
                    // find index of node 2
                    for (n2=0; n2<4; n2++)
                    {
                        noeud2 = e[n2];

                        for (j=0; j<3; j++)
                        {
                            COLUMN = offset+3*noeud2+j;
                            column = 3*n2+j;
                            mat->add(ROW, COLUMN, - tmp[row][column]*k);
                        }
                    }
                }
            }
        }
    }
}

template<class TCoord, class TDeriv, class TReal>
void TetrahedronFEMForceFieldInternalData< gpu::cuda::CudaVectorTypes<TCoord,TDeriv,TReal> >::getRotations(Main* m, VecReal& rotations, bool prefetch)
{
    if (prefetch) return;
    Data& data = m->data;
    if (data.initState.empty())
    {
        data.initState.resize((data.nbElement+BSIZE-1)/BSIZE);
        data.rotationIdx.resize(data.nbVertex);
        for (int i=0; i<data.nbVertex; ++i)
        {
            data.rotationIdx[i] = m->_rotationIdx[i];
            //m->sout << "RotationIdx["<<i<<"] = " << data.rotationIdx[i]<<m->sendl;
        }
        for (int i=0; i<data.nbElement; ++i)
        {
            defaulttype::Mat<3,3,TReal> initR, curR;
            for (int l=0; l<3; ++l)
                for (int c=0; c<3; ++c)
                {
                    initR[l][c] = m->_initialRotations[i][c][l];
                    data.initState[i/BSIZE].Rt[l][c][i%BSIZE] = m->_initialRotations[i][c][l];
                    curR[l][c] = data.state[i/BSIZE].Rt[l][c][i%BSIZE];
                }
            //m->sout << "rotation element "<<i<<": init = " << initR << ", cur = " << curR <<m->sendl;
        }
    }
    if ((int)rotations.size() < data.nbVertex*9)
        rotations.resize(data.nbVertex*9);

    Kernels::getRotations(data.size(),
            data.nbVertex,
            data.initState.deviceRead(),
            data.state.deviceRead(),
            data.rotationIdx.deviceRead(),
            rotations.deviceWrite());
}

// I know using macros is bad design but this is the only way not to repeat the code for all CUDA types
#define CudaTetrahedronFEMForceField_ImplMethods(T) \
    template<> bool TetrahedronFEMForceField< T >::canPrefetch() const \
    { return true; }						       \
    template<> void TetrahedronFEMForceField< T >::reinit() \
    { data.reinit(this); } \
    template<> void TetrahedronFEMForceField< T >::addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& v) \
    { data.addForce(this, f, x, v, this->isPrefetching()); }		\
	template<> void TetrahedronFEMForceField< T >::getRotations(VecReal& rotations) \
	{ data.getRotations(this, rotations, this->isPrefetching()); } \
    template<> void TetrahedronFEMForceField< T >::addDForce(VecDeriv& df, const VecDeriv& dx, double kFactor, double bFactor) \
    { data.addDForce(this, df, dx, kFactor, bFactor, this->isPrefetching()); } \
    template<> void TetrahedronFEMForceField< T >::addKToMatrix(sofa::defaulttype::BaseMatrix* mat, SReal kFactor, unsigned int& offset) \
    { data.addKToMatrix(this, mat, kFactor, offset); }

CudaTetrahedronFEMForceField_ImplMethods(gpu::cuda::CudaVec3fTypes);
CudaTetrahedronFEMForceField_ImplMethods(gpu::cuda::CudaVec3f1Types);

#ifdef SOFA_GPU_CUDA_DOUBLE

CudaTetrahedronFEMForceField_ImplMethods(gpu::cuda::CudaVec3dTypes);
CudaTetrahedronFEMForceField_ImplMethods(gpu::cuda::CudaVec3d1Types);

#endif // SOFA_GPU_CUDA_DOUBLE

#undef CudaTetrahedronFEMForceField_ImplMethods

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
