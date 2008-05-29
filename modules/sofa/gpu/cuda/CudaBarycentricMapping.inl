#ifndef SOFA_GPU_CUDA_CUDABARYCENTRICMAPPING_INL
#define SOFA_GPU_CUDA_CUDABARYCENTRICMAPPING_INL

#include "CudaBarycentricMapping.h"
#include <sofa/component/mapping/BarycentricMapping.inl>

namespace sofa
{

namespace gpu
{

namespace cuda
{

extern "C"
{
    void RegularGridMapperCuda3f_apply(unsigned int size, const unsigned int* gridsize, const void* map, void* out, const void* in);
    void RegularGridMapperCuda3f_applyJ(unsigned int size, const unsigned int* gridsize, const void* map, void* out, const void* in);
    void RegularGridMapperCuda3f_applyJT(unsigned int insize, unsigned int maxNOut, const unsigned int* gridsize, const void* mapT, void* out, const void* in);

    void RegularGridMapperCuda3f1_apply(unsigned int size, const unsigned int* gridsize, const void* map, void* out, const void* in);
    void RegularGridMapperCuda3f1_applyJ(unsigned int size, const unsigned int* gridsize, const void* map, void* out, const void* in);
    void RegularGridMapperCuda3f1_applyJT(unsigned int insize, unsigned int maxNOut, const unsigned int* gridsize, const void* mapT, void* out, const void* in);

    void RegularGridMapperCuda3f_3f1_apply(unsigned int size, const unsigned int* gridsize, const void* map, void* out, const void* in);
    void RegularGridMapperCuda3f_3f1_applyJ(unsigned int size, const unsigned int* gridsize, const void* map, void* out, const void* in);
    void RegularGridMapperCuda3f_3f1_applyJT(unsigned int insize, unsigned int maxNOut, const unsigned int* gridsize, const void* mapT, void* out, const void* in);

    void RegularGridMapperCuda3f1_3f_apply(unsigned int size, const unsigned int* gridsize, const void* map, void* out, const void* in);
    void RegularGridMapperCuda3f1_3f_applyJ(unsigned int size, const unsigned int* gridsize, const void* map, void* out, const void* in);
    void RegularGridMapperCuda3f1_3f_applyJT(unsigned int insize, unsigned int maxNOut, const unsigned int* gridsize, const void* mapT, void* out, const void* in);

    void MeshMapperCuda3f_apply(unsigned int size, unsigned int maxN, const void* map, void* out, const void* in);
    void MeshMapperCuda3f1_apply(unsigned int size, unsigned int maxN, const void* map, void* out, const void* in);
    void MeshMapperCuda3f_3f1_apply(unsigned int size, unsigned int maxN, const void* map, void* out, const void* in);
    void MeshMapperCuda3f1_3f_apply(unsigned int size, unsigned int maxN, const void* map, void* out, const void* in);
}

} // namespace cuda

} // namespace gpu

namespace component
{

namespace mapping
{

using namespace gpu::cuda;


template <typename VecIn, typename VecOut>
void BarycentricMapperRegularGridTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::clear(int reserve)
{
    map.clear();
    if (reserve>0) map.reserve(reserve);
}

template <typename VecIn, typename VecOut>
int BarycentricMapperRegularGridTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::addPointInCube(int cubeIndex, const Real* baryCoords)
{
    map.resize(map.size()+1);
    CubeData& data = map[map.size()-1];
    //data.in_index = cubeIndex;
#ifdef SOFA_NEW_HEXA
    data.in_index = topology->getHexaCopy(cubeIndex)[0];
#else
    data.in_index = topology->getCubeCopy(cubeIndex)[0];
#endif
    data.baryCoords[0] = baryCoords[0];
    data.baryCoords[1] = baryCoords[1];
    data.baryCoords[2] = baryCoords[2];
    maxNOut = 0; // mapT must be recomputed
    return map.size()-1;
}

template <typename VecIn, typename VecOut>
void BarycentricMapperRegularGridTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::init(const typename Out::VecCoord& out, const typename In::VecCoord& /*in*/)
{
    int outside = 0;

    clear(out.size());
    for (unsigned int i=0; i<out.size(); i++)
    {
        Vector3 coefs;
        int cube = topology->findCube(Vector3(out[i]), coefs[0], coefs[1], coefs[2]);
        if (cube==-1)
        {
            ++outside;
            cube = topology->findNearestCube(Vector3(out[i]), coefs[0], coefs[1], coefs[2]);
        }
        Vec<3,Real> baryCoords = coefs;
        addPointInCube(cube, baryCoords.ptr());
    }
}

template <typename VecIn, typename VecOut>
void BarycentricMapperRegularGridTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::calcMapT()
{
    if (!map.empty() && maxNOut == 0)
    {
        const unsigned int insize = topology->getNbPoints();
        const unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
        // compute mapT
        const int nx = gridsize[0];
        const int nxny = gridsize[0]*gridsize[1];
        const int shift[8] = { 0, 1, nx, 1+nx, nxny, 1+nxny, nx+nxny, 1+nx+nxny };
        std::vector<int> nout(insize);
        for (unsigned int i=0; i<map.size(); i++)
        {
            int index0 = map[i].in_index;
            for (int j=0; j<8; j++)
                nout[index0+shift[j]]++;
        }
        for (unsigned int i=0; i<insize; i++)
            if (nout[i] > maxNOut) maxNOut = nout[i];
        int nbloc = (insize+BSIZE-1)/BSIZE;
        std::cout << "CudaBarycentricMapping: mapT with "<<maxNOut<<" entries per DOF and "<<nbloc<<" blocs."<<std::endl;
        mapT.resize(nbloc*(BSIZE*maxNOut));
        for (unsigned int i=0; i<mapT.size(); i++)
            mapT[i] = std::make_pair(-1,0.0f);
        nout.clear();
        nout.resize(insize);
        for (unsigned int i=0; i<map.size(); i++)
        {
            int index0 = map[i].in_index;
            for (int j=0; j<8; j++)
            {
                int index = index0+shift[j];
                int num = nout[index]++;
                int b = (index / BSIZE); index -= b*BSIZE;
                float f;
                f  = (j&1)?(map[i].baryCoords[0]):(1-map[i].baryCoords[0]);
                f *= (j&2)?(map[i].baryCoords[1]):(1-map[i].baryCoords[1]);
                f *= (j&4)?(map[i].baryCoords[2]):(1-map[i].baryCoords[2]);
                //std::cout << "mapT["<<b<<"*"<<maxNOut*BSIZE<<"+"<<num<<"*"<<BSIZE<<"+"<<index<<"] = < "<<i<<", "<<f<<">"<<std::endl;
                mapT[(maxNOut*b+num)*BSIZE+index] = std::make_pair(i,f);
            }
        }
    }
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3fTypes,CudaVec3fTypes>::apply( Out::VecCoord& out, const In::VecCoord& in )
{
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    out.fastResize(map.size());
    RegularGridMapperCuda3f_apply(map.size(), gridsize, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3fTypes,CudaVec3fTypes>::applyJ( Out::VecDeriv& out, const In::VecDeriv& in )
{
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    out.fastResize(map.size());
    RegularGridMapperCuda3f_applyJ(map.size(), gridsize, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3fTypes,CudaVec3fTypes>::applyJT( In::VecDeriv& out, const Out::VecDeriv& in )
{
    if (map.size() == 0) return;
    calcMapT();
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    unsigned int insize = out.size();

    RegularGridMapperCuda3f_applyJT(insize, maxNOut, gridsize, mapT.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3fTypes,CudaVec3fTypes>::applyJT( In::VecConst& /*out*/, const Out::VecConst& /*in*/ )
{
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3fTypes,CudaVec3fTypes>::draw( const Out::VecCoord& /*out*/, const In::VecCoord& /*in*/)
{
}




template<>
void BarycentricMapperRegularGridTopology<CudaVec3f1Types,CudaVec3f1Types>::apply( Out::VecCoord& out, const In::VecCoord& in )
{
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    out.fastResize(map.size());
    RegularGridMapperCuda3f1_apply(map.size(), gridsize, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3f1Types,CudaVec3f1Types>::applyJ( Out::VecDeriv& out, const In::VecDeriv& in )
{
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    out.fastResize(map.size());
    RegularGridMapperCuda3f1_applyJ(map.size(), gridsize, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3f1Types,CudaVec3f1Types>::applyJT( In::VecDeriv& out, const Out::VecDeriv& in )
{
    calcMapT();
    if (map.size() == 0) return;
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    unsigned int insize = out.size();

    RegularGridMapperCuda3f1_applyJT(insize, maxNOut, gridsize, mapT.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3f1Types,CudaVec3f1Types>::applyJT( In::VecConst& /*out*/, const Out::VecConst& /*in*/ )
{
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3f1Types,CudaVec3f1Types>::draw( const Out::VecCoord& /*out*/, const In::VecCoord& /*in*/)
{
}




template<>
void BarycentricMapperRegularGridTopology<CudaVec3f1Types,CudaVec3fTypes>::apply( Out::VecCoord& out, const In::VecCoord& in )
{
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    out.fastResize(map.size());
    RegularGridMapperCuda3f1_3f_apply(map.size(), gridsize, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3f1Types,CudaVec3fTypes>::applyJ( Out::VecDeriv& out, const In::VecDeriv& in )
{
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    out.fastResize(map.size());
    RegularGridMapperCuda3f1_3f_applyJ(map.size(), gridsize, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3f1Types,CudaVec3fTypes>::applyJT( In::VecDeriv& out, const Out::VecDeriv& in )
{
    calcMapT();
    if (map.size() == 0) return;
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    unsigned int insize = out.size();

    RegularGridMapperCuda3f1_3f_applyJT(insize, maxNOut, gridsize, mapT.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3f1Types,CudaVec3fTypes>::applyJT( In::VecConst& /*out*/, const Out::VecConst& /*in*/ )
{
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3f1Types,CudaVec3fTypes>::draw( const Out::VecCoord& /*out*/, const In::VecCoord& /*in*/)
{
}



template<>
void BarycentricMapperRegularGridTopology<CudaVec3fTypes,CudaVec3f1Types>::apply( Out::VecCoord& out, const In::VecCoord& in )
{
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    out.fastResize(map.size());
    RegularGridMapperCuda3f_3f1_apply(map.size(), gridsize, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3fTypes,CudaVec3f1Types>::applyJ( Out::VecDeriv& out, const In::VecDeriv& in )
{
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    out.fastResize(map.size());
    RegularGridMapperCuda3f_3f1_applyJ(map.size(), gridsize, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3fTypes,CudaVec3f1Types>::applyJT( In::VecDeriv& out, const Out::VecDeriv& in )
{
    calcMapT();
    if (map.size() == 0) return;
    unsigned int gridsize[3] = { topology->getNx(), topology->getNy(), topology->getNz() };
    unsigned int insize = out.size();

    RegularGridMapperCuda3f_3f1_applyJT(insize, maxNOut, gridsize, mapT.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3fTypes,CudaVec3f1Types>::applyJT( In::VecConst& /*out*/, const Out::VecConst& /*in*/ )
{
}

template<>
void BarycentricMapperRegularGridTopology<CudaVec3fTypes,CudaVec3f1Types>::draw( const Out::VecCoord& /*out*/, const In::VecCoord& /*in*/)
{
}













template <typename VecIn, typename VecOut>
void BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::resizeMap(int size2, int maxNIn2)
{
    if (maxNIn2 < maxNIn) maxNIn2 = maxNIn;
    map.resize(((size2+BSIZE-1)/BSIZE)*maxNIn2);
    if (maxNIn2 > maxNIn)
    {
        int n = (size2+BSIZE-1)/BSIZE;
        for (int i=n-1; i>=0; --i)
            for (int j=maxNIn-1; j>=0; --j)
                map[i*maxNIn2+j]=map[i*maxNIn+j];
    }
    size = size2;
    maxNIn = maxNIn2;
}

template <typename VecIn, typename VecOut>
void BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::setMap(int outIndex, int j, int inIndex, Real val)
{
    int b    = outIndex / BSIZE;
    outIndex = outIndex % BSIZE;
    map[b*maxNIn+j].d[outIndex].i = inIndex+1;
    map[b*maxNIn+j].d[outIndex].val = val;
}

template <typename VecIn, typename VecOut>
void BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::clear(int reserve)
{
    map.clear(); if (reserve>0) map.reserve((reserve+BSIZE-1)/BSIZE*maxNIn);
    size = 0;
}

template <typename VecIn, typename VecOut>
int BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::addPointInLine(int lineIndex, const SReal* baryCoords)
{
    unsigned int i0 = size;
    resizeMap(i0+1,2);
    core::componentmodel::topology::BaseMeshTopology::Line e = topology->getLine(lineIndex);
    setMap(i0,0,e[0],(Real)(1-baryCoords[0]));
    setMap(i0,1,e[1],(Real)(baryCoords[0]));
    return i0;
}

template <typename VecIn, typename VecOut>
int BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::addPointInTriangle(int triangleIndex, const SReal* baryCoords)
{
    unsigned int i0 = size;
    resizeMap(i0+1,3);
    core::componentmodel::topology::BaseMeshTopology::Triangle e = topology->getTriangle(triangleIndex);
    setMap(i0,0,e[0],(Real)(1-baryCoords[0]-baryCoords[1]));
    setMap(i0,1,e[1],(Real)(baryCoords[0]));
    setMap(i0,2,e[2],(Real)(baryCoords[1]));
    return i0;
}

template <typename VecIn, typename VecOut>
int BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::addPointInQuad(int quadIndex, const SReal* baryCoords)
{
    unsigned int i0 = size;
    resizeMap(i0+1,4);
    core::componentmodel::topology::BaseMeshTopology::Quad e = topology->getQuad(quadIndex);
    setMap(i0,0,e[0],(Real)((1-baryCoords[0])*(1-baryCoords[1])));
    setMap(i0,1,e[1],(Real)((  baryCoords[0])*(1-baryCoords[1])));
    setMap(i0,2,e[3],(Real)((1-baryCoords[0])*(  baryCoords[1])));
    setMap(i0,3,e[2],(Real)((  baryCoords[0])*(  baryCoords[1])));
    return i0;
}

template <typename VecIn, typename VecOut>
int BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::addPointInTetra(int tetraIndex, const SReal* baryCoords)
{
    unsigned int i0 = size;
    resizeMap(i0+1,4);
    core::componentmodel::topology::BaseMeshTopology::Tetra e = topology->getTetra(tetraIndex);
    setMap(i0,0,e[0],(Real)(1-baryCoords[0]-baryCoords[1]-baryCoords[2]));
    setMap(i0,1,e[1],(Real)(baryCoords[0]));
    setMap(i0,2,e[2],(Real)(baryCoords[1]));
    setMap(i0,3,e[3],(Real)(baryCoords[2]));
    return i0;
}

template <typename VecIn, typename VecOut>
int BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::addPointInCube(int cubeIndex, const SReal* baryCoords)
{
    unsigned int i0 = size;
    resizeMap(i0+1,8);
#ifdef SOFA_NEW_HEXA
    core::componentmodel::topology::BaseMeshTopology::Hexa e = topology->getHexa(cubeIndex);
    setMap(i0,0,e[0],(Real)((1-baryCoords[0])*(1-baryCoords[1])*(1-baryCoords[2])));
    setMap(i0,1,e[1],(Real)((  baryCoords[0])*(1-baryCoords[1])*(1-baryCoords[2])));
    setMap(i0,2,e[3],(Real)((1-baryCoords[0])*(  baryCoords[1])*(1-baryCoords[2])));
    setMap(i0,3,e[2],(Real)((  baryCoords[0])*(  baryCoords[1])*(1-baryCoords[2])));
    setMap(i0,4,e[4],(Real)((1-baryCoords[0])*(1-baryCoords[1])*(  baryCoords[2])));
    setMap(i0,5,e[5],(Real)((  baryCoords[0])*(1-baryCoords[1])*(  baryCoords[2])));
    setMap(i0,6,e[7],(Real)((1-baryCoords[0])*(  baryCoords[1])*(  baryCoords[2])));
    setMap(i0,7,e[6],(Real)((  baryCoords[0])*(  baryCoords[1])*(  baryCoords[2])));
#else
    core::componentmodel::topology::BaseMeshTopology::Cube e = topology->getCube(cubeIndex);
    setMap(i0,0,e[0],(Real)((1-baryCoords[0])*(1-baryCoords[1])*(1-baryCoords[2])));
    setMap(i0,1,e[1],(Real)((  baryCoords[0])*(1-baryCoords[1])*(1-baryCoords[2])));
    setMap(i0,2,e[2],(Real)((1-baryCoords[0])*(  baryCoords[1])*(1-baryCoords[2])));
    setMap(i0,3,e[3],(Real)((  baryCoords[0])*(  baryCoords[1])*(1-baryCoords[2])));
    setMap(i0,4,e[4],(Real)((1-baryCoords[0])*(1-baryCoords[1])*(  baryCoords[2])));
    setMap(i0,5,e[5],(Real)((  baryCoords[0])*(1-baryCoords[1])*(  baryCoords[2])));
    setMap(i0,6,e[6],(Real)((1-baryCoords[0])*(  baryCoords[1])*(  baryCoords[2])));
    setMap(i0,7,e[7],(Real)((  baryCoords[0])*(  baryCoords[1])*(  baryCoords[2])));
#endif
    return i0;
}


template <typename VecIn, typename VecOut>
int BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::createPointInLine(const typename Out::Coord& p, int lineIndex, const typename In::VecCoord* points)
{
    SReal baryCoords[1];
    const topology::MeshTopology::Line& elem = topology->getLine(lineIndex);
    const typename In::Coord p0 = (*points)[elem[0]];
    const typename In::Coord pA = (*points)[elem[1]] - p0;
    typename In::Coord pos = p - p0;
    baryCoords[0] = ((pos*pA)/pA.norm2());
    return this->addPointInLine(lineIndex, baryCoords);
}

template <typename VecIn, typename VecOut>
int BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::createPointInTriangle(const typename Out::Coord& p, int triangleIndex, const typename In::VecCoord* points)
{
    SReal baryCoords[2];
    const topology::MeshTopology::Triangle& elem = topology->getTriangle(triangleIndex);
    const typename In::Coord p0 = (*points)[elem[0]];
    const typename In::Coord pA = (*points)[elem[1]] - p0;
    const typename In::Coord pB = (*points)[elem[2]] - p0;
    typename In::Coord pos = p - p0;
    // First project to plane
    typename In::Coord normal = cross(pA, pB);
    Real norm2 = normal.norm2();
    pos -= normal*((pos*normal)/norm2);
    baryCoords[0] = (Real)sqrt(cross(pB, pos).norm2() / norm2);
    baryCoords[1] = (Real)sqrt(cross(pA, pos).norm2() / norm2);
    return this->addPointInTriangle(triangleIndex, baryCoords);
}

template <typename VecIn, typename VecOut>
int BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::createPointInQuad(const typename Out::Coord& p, int quadIndex, const typename In::VecCoord* points)
{
    SReal baryCoords[2];
    const topology::MeshTopology::Quad& elem = topology->getQuad(quadIndex);
    const typename In::Coord p0 = (*points)[elem[0]];
    const typename In::Coord pA = (*points)[elem[1]] - p0;
    const typename In::Coord pB = (*points)[elem[3]] - p0;
    typename In::Coord pos = p - p0;
    Mat<3,3,typename In::Real> m,mt,base;
    m[0] = pA;
    m[1] = pB;
    m[2] = cross(pA, pB);
    mt.transpose(m);
    base.invert(mt);
    const typename In::Coord base0 = base[0];
    const typename In::Coord base1 = base[1];
    baryCoords[0] = base0 * pos;
    baryCoords[1] = base1 * pos;
    return this->addPointInQuad(quadIndex, baryCoords);
}


template <typename VecIn, typename VecOut>
void BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::init(const typename Out::VecCoord& out, const typename In::VecCoord& in)
{
    int outside = 0;

    const topology::MeshTopology::SeqTetras& tetras = topology->getTetras();
#ifdef SOFA_NEW_HEXA
    const topology::MeshTopology::SeqHexas& cubes = topology->getHexas();
#else
    const topology::MeshTopology::SeqCubes& cubes = topology->getCubes();
#endif
    const topology::MeshTopology::SeqTriangles& triangles = topology->getTriangles();
    const topology::MeshTopology::SeqQuads& quads = topology->getQuads();
    sofa::helper::vector<Matrix3> bases;
    sofa::helper::vector<Vector3> centers;
    clear(out.size()); // reserve space for mapping
    if (tetras.empty() && cubes.empty())
    {
        // no 3D elements -> map on 2D elements
        int c0 = triangles.size();
        bases.resize(triangles.size()+quads.size());
        centers.resize(triangles.size()+quads.size());
        for (unsigned int t = 0; t < triangles.size(); t++)
        {
            Mat3x3d m,mt;
            m[0] = in[triangles[t][1]]-in[triangles[t][0]];
            m[1] = in[triangles[t][2]]-in[triangles[t][0]];
            m[2] = cross(m[0],m[1]);
            mt.transpose(m);
            bases[t].invert(mt);
            centers[t] = (in[triangles[t][0]]+in[triangles[t][1]]+in[triangles[t][2]])/3;
        }
        for (unsigned int c = 0; c < quads.size(); c++)
        {
            Mat3x3d m,mt;
            m[0] = in[quads[c][1]]-in[quads[c][0]];
            m[1] = in[quads[c][3]]-in[quads[c][0]];
            m[2] = cross(m[0],m[1]);
            mt.transpose(m);
            bases[c0+c].invert(mt);
            centers[c0+c] = (in[quads[c][0]]+in[quads[c][1]]+in[quads[c][2]]+in[quads[c][3]])*0.25;
        }
        for (unsigned int i=0; i<out.size(); i++)
        {
            Vector3 pos = out[i];
            Vector3 coefs;
            int index = -1;
            double distance = 1e10;
            for (unsigned int t = 0; t < triangles.size(); t++)
            {
                Vec3d v = bases[t] * (pos - in[triangles[t][0]]);
                double d = std::max(std::max(-v[0],-v[1]),std::max((v[2]<0?-v[2]:v[2])-0.01,v[0]+v[1]-1));
                if (d>0) d = (pos-centers[t]).norm2();
                if (d<distance) { coefs = v; distance = d; index = t; }
            }
            for (unsigned int c = 0; c < quads.size(); c++)
            {
                Vec3d v = bases[c0+c] * (pos - in[quads[c][0]]);
                double d = std::max(std::max(-v[0],-v[1]),std::max(std::max(v[1]-1,v[0]-1),std::max(v[2]-0.01,-v[2]-0.01)));
                if (d>0) d = (pos-centers[c0+c]).norm2();
                if (d<distance) { coefs = v; distance = d; index = c0+c; }
            }
            if (distance>0)
            {
                ++outside;
            }
            if (index < c0)
                addPointInTriangle(index, coefs.ptr());
            else
                addPointInQuad(index-c0, coefs.ptr());
        }
    }
    else
    {
        int c0 = tetras.size();
        bases.resize(tetras.size()+cubes.size());
        centers.resize(tetras.size()+cubes.size());
        for (unsigned int t = 0; t < tetras.size(); t++)
        {
            Mat3x3d m,mt;
            m[0] = in[tetras[t][1]]-in[tetras[t][0]];
            m[1] = in[tetras[t][2]]-in[tetras[t][0]];
            m[2] = in[tetras[t][3]]-in[tetras[t][0]];
            mt.transpose(m);
            bases[t].invert(mt);
            centers[t] = (in[tetras[t][0]]+in[tetras[t][1]]+in[tetras[t][2]]+in[tetras[t][3]])*0.25;
            //std::cout << "Tetra "<<t<<" center="<<centers[t]<<" base="<<m<<std::endl;
        }
        for (unsigned int c = 0; c < cubes.size(); c++)
        {
            Mat3x3d m,mt;
            m[0] = in[cubes[c][1]]-in[cubes[c][0]];
#ifdef SOFA_NEW_HEXA
            m[1] = in[cubes[c][3]]-in[cubes[c][0]];
#else
            m[1] = in[cubes[c][2]]-in[cubes[c][0]];
#endif
            m[2] = in[cubes[c][4]]-in[cubes[c][0]];
            mt.transpose(m);
            bases[c0+c].invert(mt);
            centers[c0+c] = (in[cubes[c][0]]+in[cubes[c][1]]+in[cubes[c][2]]+in[cubes[c][3]]+in[cubes[c][4]]+in[cubes[c][5]]+in[cubes[c][6]]+in[cubes[c][7]])*0.125;
        }
        for (unsigned int i=0; i<out.size(); i++)
        {
            Vector3 pos = out[i];
            Vector3 coefs;
            int index = -1;
            double distance = 1e10;
            for (unsigned int t = 0; t < tetras.size(); t++)
            {
                Vector3 v = bases[t] * (pos - in[tetras[t][0]]);
                double d = std::max(std::max(-v[0],-v[1]),std::max(-v[2],v[0]+v[1]+v[2]-1));
                if (d>0) d = (pos-centers[t]).norm2();
                if (d<distance) { coefs = v; distance = d; index = t; }
            }
            for (unsigned int c = 0; c < cubes.size(); c++)
            {
                Vector3 v = bases[c0+c] * (pos - in[cubes[c][0]]);
                double d = std::max(std::max(-v[0],-v[1]),std::max(std::max(-v[2],v[0]-1),std::max(v[1]-1,v[2]-1)));
                if (d>0) d = (pos-centers[c0+c]).norm2();
                if (d<distance) { coefs = v; distance = d; index = c0+c; }
            }
            if (distance>0)
            {
                ++outside;
            }
            if (index < c0)
                addPointInTetra(index, coefs.ptr());
            else
                addPointInCube(index-c0, coefs.ptr());
        }
    }
}


template <typename VecIn, typename VecOut>
void BarycentricMapperMeshTopology<gpu::cuda::CudaVectorTypes<VecIn,VecIn,float>, gpu::cuda::CudaVectorTypes<VecOut,VecOut,float> >::calcMapT()
{
    if (size > 0 && maxNOut == 0)
    {
        const unsigned int insize = topology->getNbPoints();
        // compute mapT
        std::vector<int> nout(insize);
        int nb = (size+BSIZE-1)/BSIZE;
        for (int b=0; b<nb; b++)
        {
            for (int j=0; j<maxNIn; j++)
            {
                int n = (b<nb-1) ? BSIZE : size-b*BSIZE;
                for (int i=0; i<n; i++)
                {
                    int index = map[b*maxNIn+j].d[i].i-1;
                    if (index >= 0)
                        nout[index]++;
                }
            }
        }
        for (unsigned int i=0; i<insize; i++)
            if (nout[i] > maxNOut) maxNOut = nout[i];
        int nbloc = (insize+BSIZE-1)/BSIZE;
        std::cout << "CudaBarycentricMapping: mapT with "<<maxNOut<<" entries per DOF and "<<nbloc<<" blocs."<<std::endl;
        mapT.clear();
        mapT.resize(nbloc*maxNOut);
        //for (unsigned int i=0;i<mapT.size();i++)
        //    mapT[i] = std::make_pair(-1,0.0f);
        nout.clear();
        nout.resize(insize);
        for (int b=0; b<nb; b++)
        {
            for (int j=0; j<maxNIn; j++)
            {
                int n = (b<nb-1) ? BSIZE : size-b*BSIZE;
                for (int i=0; i<n; i++)
                {
                    int index = map[b*maxNIn+j].d[i].i-1;
                    float val = (float) map[b*maxNIn+j].d[i].val;
                    if (index >= 0)
                    {
                        int num = nout[index]++;
                        int bo = (index / BSIZE); index -= b*BSIZE;
                        mapT[bo*maxNOut+num].d[index].i = b*BSIZE+i;
                        mapT[bo*maxNOut+num].d[index].val = val;
                    }
                }
            }
        }
    }
}




template<>
void BarycentricMapperMeshTopology<CudaVec3fTypes,CudaVec3fTypes>::apply( Out::VecCoord& out, const In::VecCoord& in )
{
    out.fastResize(size);
    MeshMapperCuda3f_apply(size, maxNIn, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3fTypes,CudaVec3fTypes>::applyJ( Out::VecDeriv& out, const In::VecDeriv& in )
{
    out.fastResize(size);
    MeshMapperCuda3f_apply(size, maxNIn, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3fTypes,CudaVec3fTypes>::applyJT( In::VecDeriv& out, const Out::VecDeriv& in )
{
    if (size == 0) return;
    calcMapT();
    unsigned int insize = out.size();
    MeshMapperCuda3f_apply(insize, maxNOut, mapT.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3fTypes,CudaVec3fTypes>::applyJT( In::VecConst& /*out*/, const Out::VecConst& /*in*/ )
{
}

template<>
void BarycentricMapperMeshTopology<CudaVec3fTypes,CudaVec3fTypes>::draw( const Out::VecCoord& /*out*/, const In::VecCoord& /*in*/)
{
}





template<>
void BarycentricMapperMeshTopology<CudaVec3f1Types,CudaVec3f1Types>::apply( Out::VecCoord& out, const In::VecCoord& in )
{
    out.fastResize(size);
    MeshMapperCuda3f1_apply(size, maxNIn, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3f1Types,CudaVec3f1Types>::applyJ( Out::VecDeriv& out, const In::VecDeriv& in )
{
    out.fastResize(size);
    MeshMapperCuda3f1_apply(size, maxNIn, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3f1Types,CudaVec3f1Types>::applyJT( In::VecDeriv& out, const Out::VecDeriv& in )
{
    if (size == 0) return;
    calcMapT();
    unsigned int insize = out.size();
    MeshMapperCuda3f1_apply(insize, maxNOut, mapT.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3f1Types,CudaVec3f1Types>::applyJT( In::VecConst& /*out*/, const Out::VecConst& /*in*/ )
{
}

template<>
void BarycentricMapperMeshTopology<CudaVec3f1Types,CudaVec3f1Types>::draw( const Out::VecCoord& /*out*/, const In::VecCoord& /*in*/)
{
}






template<>
void BarycentricMapperMeshTopology<CudaVec3f1Types,CudaVec3fTypes>::apply( Out::VecCoord& out, const In::VecCoord& in )
{
    out.fastResize(size);
    MeshMapperCuda3f1_3f_apply(size, maxNIn, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3f1Types,CudaVec3fTypes>::applyJ( Out::VecDeriv& out, const In::VecDeriv& in )
{
    out.fastResize(size);
    MeshMapperCuda3f1_3f_apply(size, maxNIn, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3f1Types,CudaVec3fTypes>::applyJT( In::VecDeriv& out, const Out::VecDeriv& in )
{
    if (size == 0) return;
    calcMapT();
    unsigned int insize = out.size();
    MeshMapperCuda3f_3f1_apply(insize, maxNOut, mapT.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3f1Types,CudaVec3fTypes>::applyJT( In::VecConst& /*out*/, const Out::VecConst& /*in*/ )
{
}

template<>
void BarycentricMapperMeshTopology<CudaVec3f1Types,CudaVec3fTypes>::draw( const Out::VecCoord& /*out*/, const In::VecCoord& /*in*/)
{
}





template<>
void BarycentricMapperMeshTopology<CudaVec3fTypes,CudaVec3f1Types>::apply( Out::VecCoord& out, const In::VecCoord& in )
{
    out.fastResize(size);
    MeshMapperCuda3f_3f1_apply(size, maxNIn, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3fTypes,CudaVec3f1Types>::applyJ( Out::VecDeriv& out, const In::VecDeriv& in )
{
    out.fastResize(size);
    MeshMapperCuda3f_3f1_apply(size, maxNIn, map.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3fTypes,CudaVec3f1Types>::applyJT( In::VecDeriv& out, const Out::VecDeriv& in )
{
    if (size == 0) return;
    calcMapT();
    unsigned int insize = out.size();
    MeshMapperCuda3f1_3f_apply(insize, maxNOut, mapT.deviceRead(), out.deviceWrite(), in.deviceRead());
}

template<>
void BarycentricMapperMeshTopology<CudaVec3fTypes,CudaVec3f1Types>::applyJT( In::VecConst& /*out*/, const Out::VecConst& /*in*/ )
{
}

template<>
void BarycentricMapperMeshTopology<CudaVec3fTypes,CudaVec3f1Types>::draw( const Out::VecCoord& /*out*/, const In::VecCoord& /*in*/)
{
}


} // namespace mapping

} // namespace component

} // namespace sofa

#endif
