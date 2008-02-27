#ifndef SOFA_GPU_CUDA_CUDATETRAHEDRONFEMFORCEFIELD_H
#define SOFA_GPU_CUDA_CUDATETRAHEDRONFEMFORCEFIELD_H

#include "CudaTypes.h"
#include <sofa/component/forcefield/TetrahedronFEMForceField.h>


namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;

template <class Coord, class Deriv, class Real>
class TetrahedronFEMForceFieldInternalData< gpu::cuda::CudaVectorTypes<Coord,Deriv,Real> >
{
public:
    //typedef float Real;
    //typedef gpu::cuda::CudaVec3fTypes::Coord Coord;
    typedef topology::MeshTopology::Tetra Element;
    typedef Mat<6, 6, Real> MaterialStiffness;
    typedef Mat<12, 6, Real> StrainDisplacement;

    /** Static data associated with each element

    Only supports the LARGE computation method, allowing for several simplifications.

    For each element ABCD, the original algorithm computes the \f$3 \times 3\f$ rotation matrix R as follow:

    - the X axis is aligned to the AB edge
    - the Z axis is aligned to the cross-product between AB and AC
    - the Y axis is aligned to the cross-product between the previous two edges

    This matrix is used to compute the initial position of each element in the local rotated coordinate frame, with the origin on A, as follow:
    \f{eqnarray*}
        a &=& R^t \cdot (A-A) = \left( 0   \quad 0   \quad 0   \right) \\
        b &=& R^t \cdot (B-A) = \left( b_x \quad 0   \quad 0   \right) \\
        c &=& R^t \cdot (C-A) = \left( c_x \quad c_y \quad 0   \right) \\
        d &=& R^t \cdot (D-A) = \left( d_x \quad d_y \quad d_z \right) \\
    \f}

    The material stiffness is handled by a \f$6 \times 6\f$ matrix K computed as follow:

    \f{eqnarray*}
        K &=& \begin{pmatrix}
            \gamma+\mu^2 & \gamma & \gamma & 0 & 0 & 0 \\
            \gamma & \gamma+\mu^2 & \gamma & 0 & 0 & 0 \\
            \gamma & \gamma & \gamma+\mu^2 & 0 & 0 & 0 \\
            0 & 0 & 0 & \mu^2/2 & 0 & 0 \\
            0 & 0 & 0 & 0 & \mu^2/2 & 0 \\
            0 & 0 & 0 & 0 & 0 & \mu^2/2 \\
            \end{pmatrix} \\
        \gamma &=& \frac{youngModulus}{6 vol(ABCD)}\frac{poissonRatio}{(1 + poissonRatio)(1 - 2 poissonRatio)} \\
        \mu^2 &=& \frac{youngModulus}{6 vol(ABCD)}\frac{1}{1+poissonRatio}
    \f}

    The \f$12 \times 6\f$ Strain-displacement matrix is computed from the rotated initial positions abcd as follow:

    \f{eqnarray*}
        J &=& \begin{pmatrix}
            -p_x(bcd) & 0 & 0 & -p_y(bcd) & 0 & -p_z(bcd) \\
            0 & -p_y(bcd) & 0 & -p_x(bcd) & -p_z(bcd) & 0 \\
            0 & 0 & -p_z(bcd) & 0 & -p_y(bcd) & -p_x(bcd) \\
            p_x(cda) & 0 & 0 & p_y(cda) & 0 & p_z(cda) \\
            0 & p_y(cda) & 0 & p_x(cda) & p_z(cda) & 0 \\
            0 & 0 & p_z(cda) & 0 & p_y(cda) & p_x(cda) \\
            -p_x(dab) & 0 & 0 & -p_y(dab) & 0 & -p_z(dab) \\
            0 & -p_y(dab) & 0 & -p_x(dab) & -p_z(dab) & 0 \\
            0 & 0 & -p_z(dab) & 0 & -p_y(dab) & -p_x(abc) \\
            p_x(abc) & 0 & 0 & p_y(abc) & 0 & p_z(abc) \\
            0 & p_y(abc) & 0 & p_x(abc) & p_z(abc) & 0 \\
            0 & 0 & p_z(abc) & 0 & p_y(abc) & p_x(abc) \\
            \end{pmatrix} \\
        p(uvw) &=& u \times v + v \times w + w \times u
    \f}

    Given the zeros in abcd, we have:
    \f{eqnarray*}
        Jb &=& p(cda) = c \times d + d \times a + a \times c = \begin{pmatrix}c_x \\ c_y \\ 0\end{pmatrix} \times \begin{pmatrix}d_x \\ d_y \\ d_z\end{pmatrix} = \begin{pmatrix}c_y d_z \\ - c_x d_z \\ c_x d_y - c_y d_x\end{pmatrix} \\
        Jc &=& -p(dab) = - d \times a - a \times b - b \times d = - \begin{pmatrix}b_x \\ 0 \\ 0\end{pmatrix} \times \begin{pmatrix}d_x \\ d_y \\ d_z\end{pmatrix} = \begin{pmatrix}0 \\ b_x d_z \\ - b_x d_y\end{pmatrix} \\
        Jd &=& p(abc) = a \times b + b \times c + c \times a = \begin{pmatrix}b_x \\ 0 \\ 0\end{pmatrix} \times \begin{pmatrix}c_x \\ c_y \\ 0\end{pmatrix} = \begin{pmatrix}0 \\ 0 \\ b_x c_y\end{pmatrix}
    \f}

    Also, as the sum of applied forces must be zero, we know that:
    \f{eqnarray*}
        Ja+Jb+Jc+Jd &=& 0 \\
        -p(bcd)+p(cda)-p(dab)+b(abc) &=& - b \times c - c \times d - d \times b \\
                                     &+& c \times d + d \times a + a \times c \\
                                     &-& d \times a - a \times b - b \times d \\
                                     &+& a \times b + b \times c + c \times a \\
                                     &=& 0 \\
        Ja &=& -Jb-Jc-Jd
    \f}

    The forces will be computed using \f$F = R J K J^t R^t X\f$.
    We can apply a scaling factor to J if we divide K by its square: \f$F = R (1/b_x J) (b_x^2 K) (1/b_x J)^t R^t X\f$.

    This allows us to do all computations from the values \f$\left(b_x \quad c_x \quad c_y \quad d_x \quad d_y \quad d_z \quad \gamma b_x^2 \quad \mu^2 b_x^2 \quad Jb_x/b_x \quad Jb_y/b_x \quad Jb_z/b_x\right)\f$. Including the 4 vertex indices, this represents 15 values, so we have one extra available variable to align the structure to 64 octets.

    To allow for easy coalesced accesses on the GPU, the data could be split in group of 16 bytes, or 4 32-bits values.

    */
    struct GPUElement
    {
        /// @name index of the 4 connected vertices
        /// @{
        //Vec<4,int> tetra;
        int ia[BSIZE],ib[BSIZE],ic[BSIZE],id[BSIZE];
        /// @}
        //};
        //struct GPUElement2
        //{
        /// @name material stiffness matrix
        /// @{
        //Mat<6,6,Real> K;
        float gamma_bx2[BSIZE], mu2_bx2[BSIZE];
        /// @}
        /// @name initial position of the vertices in the local (rotated) coordinate system
        /// @{
        //Vec3f initpos[4];
        float bx[BSIZE],cx[BSIZE];
        //};
        //struct GPUElement3
        //{
        float cy[BSIZE],dx[BSIZE],dy[BSIZE],dz[BSIZE];
        /// @}
        //};
        //struct GPUElement4
        //{
        /// strain-displacement matrix
        /// @{
        //Mat<12,6,Real> J;
        float Jbx_bx[BSIZE],Jby_bx[BSIZE],Jbz_bx[BSIZE];
        /// @}
        /// unused value to align to 64 bytes
        float dummy[BSIZE];
    };

    gpu::cuda::CudaVector<GPUElement> elems;

    /// Varying data associated with each element
    struct GPUElementState
    {
        /// rotation matrix
        Mat<3,3,float> Rt;
        /// current internal stress
        Vec<6,float> S;
        /// unused value to align to 64 bytes
        float dummy;
    };

    /// Varying data associated with each element
    struct GPUElementForce
    {
        Vec<4,float> fA,fB,fC,fD;
    };

    gpu::cuda::CudaVector<GPUElementState> state;
    gpu::cuda::CudaVector<GPUElementForce> eforce;
    int nbElement; ///< number of elements
    int vertex0; ///< index of the first vertex connected to an element
    int nbVertex; ///< number of vertices to process to compute all elements
    int nbElementPerVertex; ///< max number of elements connected to a vertex
    /// Index of elements attached to each points (layout per bloc of NBLOC vertices, with first element of each vertex, then second element, etc)
    /// No that each integer is actually equat the the index of the element * 4 + the index of this vertex inside the tetrahedron.
    gpu::cuda::CudaVector<int> velems;
    TetrahedronFEMForceFieldInternalData() : nbElement(0), vertex0(0), nbVertex(0), nbElementPerVertex(0) {}
    void init(int nbe, int v0, int nbv, int nbelemperv)
    {
        nbElement = nbe;
        elems.resize((nbe+BSIZE-1)/BSIZE);
        state.resize(nbe);
        eforce.resize(nbe);
        vertex0 = v0;
        nbVertex = nbv;
        nbElementPerVertex = nbelemperv;
        int nbloc = (nbVertex+BSIZE-1)/BSIZE;
        velems.resize(nbloc*nbElementPerVertex*BSIZE);
        for (unsigned int i=0; i<velems.size(); i++)
            velems[i] = 0;
    }
    int size() const { return nbElement; }
    void setV(int vertex, int num, int index)
    {
        vertex -= vertex0;
        int bloc = vertex/BSIZE;
        int b_x  = vertex%BSIZE;
        velems[ bloc*BSIZE*nbElementPerVertex // start of the bloc
                + num*BSIZE                     // offset to the element
                + b_x                           // offset to the vertex
              ] = index+1;
    }

    void setE(int i, const Element& indices, const Coord& /*a*/, const Coord& b, const Coord& c, const Coord& d, const MaterialStiffness& K, const StrainDisplacement& /*J*/)
    {
        /*std::cout << "CPU Info:\n a = "<<a<<"\n b = "<<b<<"\n c = "<<c<<"\n d = "<<d<<"\n K = "
            <<K[0]<<"\n     "<<K[1]<<"\n     "<<K[2]<<"\n     "
            <<K[3]<<"\n     "<<K[4]<<"\n     "<<K[5]<<"\n J="
            <<J[0]<<"\n     "<<J[1]<<"\n     "<<J[2]<<"\n     "
            <<J[3]<<"\n     "<<J[4]<<"\n     "<<J[5]<<"\n     "
            <<J[6]<<"\n     "<<J[7]<<"\n     "<<J[8]<<"\n     "
            <<J[9]<<"\n     "<<J[10]<<"\n     "<<J[11]<<std::endl;*/
        GPUElement& e = elems[i/BSIZE]; i = i%BSIZE;
        e.ia[i] = indices[0] - vertex0;
        e.ib[i] = indices[1] - vertex0;
        e.ic[i] = indices[2] - vertex0;
        e.id[i] = indices[3] - vertex0;
        e.bx[i] = b[0];
        e.cx[i] = c[0]; e.cy[i] = c[1];
        e.dx[i] = d[0]; e.dy[i] = d[1]; e.dz[i] = d[2];
        float bx2 = e.bx[i] * e.bx[i];
        e.gamma_bx2[i] = K[0][1] * bx2;
        e.mu2_bx2[i] = 2*K[3][3] * bx2;
        e.Jbx_bx[i] = (e.cy[i] * e.dz[i]) / e.bx[i];
        e.Jby_bx[i] = (-e.cx[i] * e.dz[i]) / e.bx[i];
        e.Jbz_bx[i] = (e.cx[i]*e.dy[i] - e.cy[i]*e.dx[i]) / e.bx[i];
        e.dummy[i] = 0;
        /*std::cout << "GPU Info:\n b = "<<e.bx<<"\n c = "<<e.cx<<" "<<e.cy<<"\n d = "<<e.dx<<" "<<e.dy<<" "<<e.dz<<"\n K = "
            <<(e.gamma_bx2+e.mu2_bx2)/bx2<<" "<<(e.gamma_bx2)/bx2<<" "<<(e.gamma_bx2)/bx2<<" 0 0 0\n     "
            <<(e.gamma_bx2)/bx2<<" "<<(e.gamma_bx2+e.mu2_bx2)/bx2<<" "<<(e.gamma_bx2)/bx2<<" 0 0 0\n     "
            <<(e.gamma_bx2)/bx2<<" "<<(e.gamma_bx2)/bx2<<" "<<(e.gamma_bx2+e.mu2_bx2)/bx2<<" 0 0 0\n     "

            <<"0 0 0 "<<(e.mu2_bx2/2)/bx2<<" 0 0\n     "
            <<"0 0 0 0 "<<(e.mu2_bx2/2)/bx2<<" 0\n     "
            <<"0 0 0 0 0 "<<(e.mu2_bx2/2)/bx2<<"\n J = "

            <<(-e.Jbx_bx)*e.bx<<" 0 0 "<<(-e.Jby_bx-e.dz)*e.bx<<" 0 "<<(-e.Jbz_bx+e.dy-e.cy)*e.bx<<"\n     "
            <<"0 "<<(-e.Jby_bx-e.dz)*e.bx<<" 0 "<<(-e.Jbx_bx)*e.bx<<" "<<(-e.Jbz_bx+e.dy-e.cy)*e.bx<<" 0\n     "
            <<"0 0 "<<(-e.Jbz_bx+e.dy-e.cy)*e.bx<<" 0 "<<(-e.Jby_bx-e.dz)*e.bx<<" "<<(-e.Jbx_bx)*e.bx<<"\n     "

            <<(e.Jbx_bx)*e.bx<<" 0 0 "<<(e.Jby_bx)*e.bx<<" 0 "<<(e.Jbz_bx)*e.bx<<"\n     "
            <<"0 "<<(e.Jby_bx)*e.bx<<" 0 "<<(e.Jbx_bx)*e.bx<<" "<<(e.Jbz_bx)*e.bx<<" 0\n     "
            <<"0 0 "<<(e.Jbz_bx)*e.bx<<" 0 "<<(e.Jby_bx)*e.bx<<" "<<(e.Jbx_bx)*e.bx<<"\n     "

            <<(0)*e.bx<<" 0 0 "<<(e.dz)*e.bx<<" 0 "<<(-e.dy)*e.bx<<"\n     "
            <<"0 "<<(e.dz)*e.bx<<" 0 "<<(0)*e.bx<<" "<<(-e.dy)*e.bx<<" 0\n     "
            <<"0 0 "<<(-e.dy)*e.bx<<" 0 "<<(e.dz)*e.bx<<" "<<(0)*e.bx<<"\n     "

            <<(0)*e.bx<<" 0 0 "<<(0)*e.bx<<" 0 "<<(e.cy)*e.bx<<"\n     "
            <<"0 "<<(0)*e.bx<<" 0 "<<(0)*e.bx<<" "<<(e.cy)*e.bx<<" 0\n     "
            <<"0 0 "<<(e.cy)*e.bx<<" 0 "<<(0)*e.bx<<" "<<(0)*e.bx<<std::endl;*/
    }
};

//
// TetrahedronFEMForceField
//

template <>
void TetrahedronFEMForceField<gpu::cuda::CudaVec3fTypes>::reinit();

template <>
void TetrahedronFEMForceField<gpu::cuda::CudaVec3fTypes>::addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& /*v*/);

template <>
void TetrahedronFEMForceField<gpu::cuda::CudaVec3fTypes>::addDForce (VecDeriv& df, const VecDeriv& dx);


//////// CudaVec3f1

template <>
void TetrahedronFEMForceField<gpu::cuda::CudaVec3f1Types>::reinit();

template <>
void TetrahedronFEMForceField<gpu::cuda::CudaVec3f1Types>::addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& /*v*/);

template <>
void TetrahedronFEMForceField<gpu::cuda::CudaVec3f1Types>::addDForce (VecDeriv& df, const VecDeriv& dx);


} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
