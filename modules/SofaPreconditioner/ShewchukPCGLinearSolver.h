/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2015 INRIA, USTL, UJF, CNRS, MGH                    *
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
#ifndef SOFA_COMPONENT_LINEARSOLVER_ShewchukPCGLinearSolver_H
#define SOFA_COMPONENT_LINEARSOLVER_ShewchukPCGLinearSolver_H
#include "config.h"

#include <sofa/core/behavior/LinearSolver.h>
#include <SofaBaseLinearSolver/MatrixLinearSolver.h>
#include <SofaBaseLinearSolver/GraphScatteredTypes.h>
#include <sofa/helper/map.h>

#include <math.h>

namespace sofa
{

namespace component
{

namespace linearsolver
{


/// Linear system solver using the conjugate gradient iterative algorithm
template<class TMatrix, class TVector>
class ShewchukPCGLinearSolver : public sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(ShewchukPCGLinearSolver,TMatrix,TVector),SOFA_TEMPLATE2(sofa::component::linearsolver::MatrixLinearSolver,TMatrix,TVector));

    typedef TMatrix Matrix;
    typedef TVector Vector;
    typedef sofa::component::linearsolver::MatrixLinearSolver<TMatrix,TVector> Inherit;

    Data<unsigned> f_maxIter;
    Data<double> f_tolerance;
    Data<bool> f_use_precond;
    Data<unsigned> f_update_step;
    Data<bool> f_build_precond;
    Data< std::string > f_preconditioners;
    Data<std::map < std::string, sofa::helper::vector<double> > > f_graph;

    Data<std::string> f_target;

protected:
    ShewchukPCGLinearSolver();
public:
    void solve (Matrix& M, Vector& x, Vector& b);
    void init();
    void setSystemMBKMatrix(const core::MechanicalParams* mparams);
    //void setSystemRHVector(VecId v);
    //void setSystemLHVector(VecId v);

private :
    unsigned next_refresh_step;
    sofa::core::behavior::LinearSolver* preconditioners;
    bool first;
    int newton_iter;

protected:
    /// This method is separated from the rest to be able to use custom/optimized versions depending on the types of vectors.
    /// It computes: p = p*beta + r
    inline void cgstep_beta(const core::ExecParams* params,Vector& p, Vector& r, double beta);
    /// This method is separated from the rest to be able to use custom/optimized versions depending on the types of vectors.
    /// It computes: x += p*alpha, r -= q*alpha
    inline void cgstep_alpha(const core::ExecParams* params, Vector& x, Vector& r, Vector& p, Vector& q, SReal alpha);

    void handleEvent(sofa::core::objectmodel::Event* event);


};

template<class TMatrix, class TVector>
inline void ShewchukPCGLinearSolver<TMatrix,TVector>::cgstep_beta(const core::ExecParams* /*params*/,Vector& p, Vector& r, double beta)
{
    std::cout<<"ShewchukPCGLinearSolver<TMatrix,TVector>::cgstep_beta is called !!!!!!!!!!!!"<<std::endl;
    p *= beta;
    p += r; //z;
}

template<class TMatrix, class TVector>
inline void ShewchukPCGLinearSolver<TMatrix,TVector>::cgstep_alpha(const core::ExecParams* /*params*/, Vector& x, Vector& r, Vector& p, Vector& q, SReal alpha)
{
    std::cout<<"ShewchukPCGLinearSolver<TMatrix,TVector>::cgstep_alpha is called !!!!!!!!!!!!"<<std::endl;
    x.peq(p,alpha);                 // x = x + alpha p
}

template<>
inline void ShewchukPCGLinearSolver<component::linearsolver::GraphScatteredMatrix,component::linearsolver::GraphScatteredVector>::cgstep_beta(const core::ExecParams* params,Vector& p, Vector& r, double beta);

template<>
inline void ShewchukPCGLinearSolver<component::linearsolver::GraphScatteredMatrix,component::linearsolver::GraphScatteredVector>::cgstep_alpha(const core::ExecParams* params, Vector& x, Vector& r, Vector& p, Vector& q, SReal alpha);

} // namespace linearsolver

} // namespace component

} // namespace sofa

#endif
