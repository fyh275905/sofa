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
#ifndef SOFA_COMPONENT_LINEARSOLVER_CGLINEARSOLVER_H
#define SOFA_COMPONENT_LINEARSOLVER_CGLINEARSOLVER_H

#include <sofa/core/componentmodel/behavior/LinearSolver.h>
#include <sofa/component/linearsolver/MatrixLinearSolver.h>
#include <sofa/helper/map.h>
#include <math.h>


namespace sofa
{

namespace component
{

namespace linearsolver
{

//#define DISPLAY_TIME

/// Linear system solver using the conjugate gradient iterative algorithm
template<class TMatrix, class TVector>
class SOFA_COMPONENT_LINEARSOLVER_API CGLinearSolver : public sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(CGLinearSolver,TMatrix,TVector),SOFA_TEMPLATE2(sofa::component::linearsolver::MatrixLinearSolver,TMatrix,TVector));

    typedef TMatrix Matrix;
    typedef TVector Vector;
    typedef sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector> Inherit;
    Data<unsigned> f_maxIter;
    Data<double> f_tolerance;
    Data<double> f_smallDenominatorThreshold;
    Data<bool> f_verbose;
    Data<std::map < std::string, sofa::helper::vector<double> > > f_graph;
#ifdef DISPLAY_TIME
    double time1;
    double time2;
    double timeStamp;
#endif

    CGLinearSolver();

protected:
    /// This method is separated from the rest to be able to use custom/optimized versions depending on the types of vectors.
    /// It computes: p = p*beta + r
    inline void cgstep_beta(Vector& p, Vector& r, double beta);
    /// This method is separated from the rest to be able to use custom/optimized versions depending on the types of vectors.
    /// It computes: x += p*alpha, r -= q*alpha
    inline void cgstep_alpha(Vector& x, Vector& r, Vector& p, Vector& q, double alpha);

public:
    /// Solve Mx=b
    void solve (Matrix& M, Vector& x, Vector& b);

    void setSystemMBKMatrix(double mFact, double bFact, double kFact);

};

template<class TMatrix, class TVector>
inline void CGLinearSolver<TMatrix,TVector>::cgstep_beta(Vector& p, Vector& r, double beta)
{
    p *= beta;
    p += r; //z;
}

template<class TMatrix, class TVector>
inline void CGLinearSolver<TMatrix,TVector>::cgstep_alpha(Vector& x, Vector& r, Vector& p, Vector& q, double alpha)
{
    x.peq(p,alpha);                 // x = x + alpha p
    r.peq(q,-alpha);                // r = r - alpha q
}

template<>
void CGLinearSolver<component::linearsolver::GraphScatteredMatrix,component::linearsolver::GraphScatteredVector>::cgstep_beta(Vector& p, Vector& r, double beta);

template<>
void CGLinearSolver<component::linearsolver::GraphScatteredMatrix,component::linearsolver::GraphScatteredVector>::cgstep_alpha(Vector& x, Vector& r, Vector& p, Vector& q, double alpha);

} // namespace linearsolver

} // namespace component

} // namespace sofa

#endif
