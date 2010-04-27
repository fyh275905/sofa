/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_SMP_PARALLELSOLVERIMPL_H
#define SOFA_SMP_PARALLELSOLVERIMPL_H

#include <sofa/core/componentmodel/behavior/OdeSolver.h>
#include <sofa/core/componentmodel/behavior/LinearSolver.h>
#include <sofa/component/odesolver/OdeSolverImpl.h>
#ifdef SOFA_SMP
#include <sofa/core/componentmodel/behavior/ParallelMultivector.h>
using namespace sofa::defaulttype::SharedTypes;
#endif
namespace sofa
{

namespace simulation
{
namespace common
{


/**
 *  \brief Implementation of LinearSolver/OdeSolver/MasterSolver relying on GNode.
 *
 */
class ParallelSolverImpl : public virtual sofa::simulation::SolverImpl
{
public:
    typedef sofa::core::componentmodel::behavior::BaseMechanicalState::VecId VecId;

    ParallelSolverImpl();

    virtual ~ParallelSolverImpl();

    /// @name Visitors and MultiVectors
    /// These methods provides an abstract view of the mechanical system to animate.
    /// They are implemented by executing Visitors in the subtree of the scene-graph below this solver.
    /// @{

    /// @name Vector operations
    /// Most of these operations can be hidden by using the MultiVector class.
    /// @{

    /// Wait for the completion of previous operations and return the result of the last v_dot call.
    ///
    /// Note that currently all methods are blocking so finish simply return the result of the last v_dot call.


    virtual void v_op(VecId v, VecId a, VecId b, Shared<double> &f); ///< v=a+b*f

    virtual void v_dot(Shared<double> &result,VecId a, VecId b); ///< a dot b
    virtual void v_peq(VecId v, VecId a, Shared<double> &fSh, double f=1.0); ///< v+=f*a
    virtual void v_peq(VecId v, VecId a, double f=1.0); ///< v+=f*a

    virtual void v_meq(VecId v, VecId a, Shared<double> &fSh); ///< v+=f*a




};



} // namespace simulation
} // namespace simulation

} // namespace sofa

#endif
