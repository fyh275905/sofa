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
#ifndef SOFA_COMPONENT_ODESOLVER_RUNGEKUTTA4SOLVER_H
#define SOFA_COMPONENT_ODESOLVER_RUNGEKUTTA4SOLVER_H

#include <sofa/core/behavior/OdeSolver.h>
#include <sofa/component/odesolver/OdeSolverImpl.h>

namespace sofa
{

namespace component
{

namespace odesolver
{

/** A popular time integration method, much more precise than the EulerSolver */
class SOFA_COMPONENT_ODESOLVER_API RungeKutta4Solver : public sofa::component::odesolver::OdeSolverImpl
{
public:
    SOFA_CLASS(RungeKutta4Solver, sofa::component::odesolver::OdeSolverImpl);

    void solve (double dt);

    void solve (double dt, sofa::core::behavior::BaseMechanicalState::VecId xResult, sofa::core::behavior::BaseMechanicalState::VecId vResult);


    /// Given an input derivative order (0 for position, 1 for velocity, 2 for acceleration),
    /// how much will it affect the output derivative of the given order.
    /// @TODO use real factors depending on the current RK4 step
    virtual double getIntegrationFactor(int inputDerivative, int outputDerivative) const
    {
        const double dt = getContext()->getDt();
        double matrix[3][3] =
        {
            { 1, dt/2, 0},
            { 0, 1, dt/2},
            { 0, 0, 0}
        };
        if (inputDerivative >= 3 || outputDerivative >= 3)
            return 0;
        else
            return matrix[outputDerivative][inputDerivative];
    }

    /// Given a solution of the linear system,
    /// how much will it affect the output derivative of the given order.
    /// @TODO use real factors depending on the current RK4 step
    virtual double getSolutionIntegrationFactor(int outputDerivative) const
    {
        const double dt = getContext()->getDt();
        double vect[3] = { 0.0, dt/2, 1};
        if (outputDerivative >= 3)
            return 0;
        else
            return vect[outputDerivative];
    }

};

} // namespace odesolver

} // namespace component

} // namespace sofa

#endif
