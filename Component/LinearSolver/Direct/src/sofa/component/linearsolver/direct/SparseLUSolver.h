/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <sofa/component/linearsolver/direct/config.h>

#include <sofa/component/linearsolver/iterative/MatrixLinearSolver.h>
#include <csparse.h>
#include <sofa/component/linearsolver/direct/CSR_to_adj.h>
#include <sofa/helper/OptionsGroup.h>
extern "C" {
#include <metis.h>
}

namespace sofa::component::linearsolver::direct
{

//defaut structure for a LU factorization
template<class Real>
class SparseLUInvertData : public MatrixInvertData {
public :

    css *S; //store the permutations and the number of non null values by rows and by lines of the LU factorization
    csn *N; // store the partial pivot and the LU factorization
    cs A;
    cs* permuted_A;
    type::vector<int> perm,iperm; // fill reducing permutation
    type::vector<int> Previous_colptr,Previous_rowind; // shape of the matrix at the previous step
    type::vector<sofa::Index> A_i, A_p;
    type::vector<Real> A_x;
    Real * tmp;
    bool need_factorization;
    SparseLUInvertData()
    {
        S=nullptr; N=nullptr; tmp=nullptr;
    }

    ~SparseLUInvertData()
    {
        if (S) cs_sfree (S);
        if (N) cs_nfree (N);
        if (tmp) cs_free (tmp);
    }
};

/// Direct linear solver based on Sparse LU factorization, implemented with the CSPARSE library
template<class TMatrix, class TVector, class TThreadManager= NoThreadManager>
class SparseLUSolver : public sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector,TThreadManager>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE3(SparseLUSolver,TMatrix,TVector,TThreadManager),SOFA_TEMPLATE3(sofa::component::linearsolver::MatrixLinearSolver,TMatrix,TVector,TThreadManager));

    typedef TMatrix Matrix;
    typedef TVector Vector;
    typedef typename Matrix::Real Real;

    typedef sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector,TThreadManager> Inherit;

    Data<bool> f_verbose; ///< Dump system state at each iteration
    Data<double> f_tol; ///< tolerance of factorization
    
    void solve (Matrix& M, Vector& x, Vector& b) override;
    void invert(Matrix& M) override;

    SparseLUSolver();

protected :

    Data<sofa::helper::OptionsGroup> d_typePermutation;
  
    void fill_reducing_perm(cs A, int * perm, int * invperm);
    css* symbolic_LU(cs *A);

    MatrixInvertData * createInvertData() override {
        return new SparseLUInvertData<Real>();
    }

};


inline bool need_symbolic_factorization(int s_M, int * M_colptr,int * M_rowind, int s_P, int * P_colptr,int * P_rowind) {
    if (s_M != s_P) return true;
    if (M_colptr[s_M] != P_colptr[s_M] ) return true;

    for (int i=0;i<s_P;i++) {
        if (M_colptr[i]!=P_colptr[i]) return true;
    }

    for (int i=0;i<M_colptr[s_M];i++) {
        if (M_rowind[i]!=P_rowind[i]) return true;
    }

    return false;
}

#if  !defined(SOFA_COMPONENT_LINEARSOLVER_SPARSELUSOLVER_CPP)
extern template class SOFA_COMPONENT_LINEARSOLVER_DIRECT_API SparseLUSolver< sofa::linearalgebra::CompressedRowSparseMatrix< SReal>, sofa::linearalgebra::FullVector<SReal> >;
#endif

} // namespace sofa::component::linearsolver::direct
