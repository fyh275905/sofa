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
#ifndef SOFA_COMPONENT_LINEARSOLVER_SparseCholeskySolver_H
#define SOFA_COMPONENT_LINEARSOLVER_SparseCholeskySolver_H

#include <sofa/core/componentmodel/behavior/LinearSolver.h>
#include <sofa/component/linearsolver/MatrixLinearSolver.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/component/linearsolver/FullMatrix.h>
#include <sofa/component/linearsolver/SparseMatrix.h>
#include <sofa/component/linearsolver/CompressedRowSparseMatrix.h>
#include <sofa/component/component.h>
#include <sofa/helper/map.h>
#include <math.h>
#include <csparse.h>

namespace sofa
{

namespace component
{

namespace linearsolver
{

/// Linear system solver using the conjugate gradient iterative algorithm
template<class TMatrix, class TVector>
class SparseCholeskySolver : public sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(SparseCholeskySolver,TMatrix,TVector),SOFA_TEMPLATE2(sofa::component::linearsolver::MatrixLinearSolver,TMatrix,TVector));

    typedef TMatrix Matrix;
    typedef TVector Vector;
    typedef sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector> Inherit;
    typedef sofa::core::componentmodel::behavior::BaseMechanicalState::VecId VecId;

    Data<bool> f_verbose;
    Data<std::map < std::string, sofa::helper::vector<double> > > f_graph;

    SparseCholeskySolver();
    ~SparseCholeskySolver();
    void solve (Matrix& M, Vector& x, Vector& b);
    void invert(Matrix& M);

    bool readFile(std::istream& in);
    bool writeFile(std::ostream& out);

private :
    css *S;
    csn *N;
    cs A;
    helper::vector<int> A_i, A_p;
    helper::vector<double> A_x;
    double * tmp;
};

#if defined(WIN32) && !defined(SOFA_BUILD_COMPONENT_LINEARSOLVER)
extern template class SOFA_COMPONENT_LINEARSOLVER_API SparseCholeskySolver< CompressedRowSparseMatrix<double>,FullVector<double> >;
#endif

} // namespace linearsolver

} // namespace component

} // namespace sofa


#endif
