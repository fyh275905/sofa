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
#define SOFA_COMPONENT_LINEARSOLVER_SPARSELDLSOLVER_CPP
#include <sofa/component/linearsolver/direct/config.h>
#include <sofa/component/linearsolver/direct/SparseLDLSolver.inl>
#include <sofa/core/ObjectFactory.h>

namespace sofa::component::linearsolver::direct
{

using namespace sofa::linearalgebra;

int SparseLDLSolverClass = core::RegisterObject("Direct Linear Solver using a Sparse LDL^T factorization.")
        .add< SparseLDLSolver< CompressedRowSparseMatrix<double>,FullVector<double> > >(true)
        .add< SparseLDLSolver< CompressedRowSparseMatrix<type::Mat<3,3,double> >,FullVector<double> > >()

;

template class SOFA_COMPONENT_LINEARSOLVER_DIRECT_API SparseLDLSolver< CompressedRowSparseMatrix<double>,FullVector<double> >;
template class SOFA_COMPONENT_LINEARSOLVER_DIRECT_API SparseLDLSolver< CompressedRowSparseMatrix< type::Mat<3,3,double> >,FullVector<double> >;


} // namespace sofa::component::linearsolver::direct
