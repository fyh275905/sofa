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
#ifndef SOFA_COMPONENT_ODESOLVER_EULERIMPLICITSOLVER_H
#define SOFA_COMPONENT_ODESOLVER_EULERIMPLICITSOLVER_H

#include <sofa/core/componentmodel/behavior/OdeSolver.h>
#include <sofa/component/odesolver/OdeSolverImpl.h>


namespace sofa
{

namespace component
{

namespace odesolver
{

using namespace sofa::defaulttype;

/** Implicit time integrator using backward Euler scheme for first and second order. By default second.
 *
 *** 2nd Order ***
 *
 * This integration scheme is based on the following equations:
 *
 *   $x_{t+h} = x_t + h v_{t+h}$
 *   $v_{t+h} = v_t + h a_{t+h}$
 *
 * Applied to a mechanical system where $ M a_t + (B + r_M M + r_K K) v_t + K x_t = f_ext$, we need to solve the following system:
 *
 *   $ M a_{t+h} + (B + r_M M + r_K K) v_{t+h} + K x_{t+h} = f_ext $
 *   $ M a_{t+h} + (B + r_M M + r_K K) ( v_t + h a_{t+h} ) + K ( x_t + h v_t + h^2 a_{t+h} ) = f_ext $
 *   $ ( M + h (B + r_M M + r_K K) + h^2 K ) a_{t+h} = f_ext - (B + r_M M + r_K K) v_t - K ( x_t + h v_t ) $
 *   $ ( M + h (B + r_M M + r_K K) + h^2 K ) a_{t+h} = f_ext - K x_t - B v_t - (r_M M + r_K K + h K) v_t $
 *   $ ( M + h (B + r_M M + r_K K) + h^2 K ) a_{t+h} = f_t - (r_M M + r_K K + h K) v_t $
 *
 *
 *** 1st Order ***
 *
 * This integration scheme is based on the following eqation:
 *
 *   $x_{t+h} = x_t + h v_{t+h}$
 *
 * Applied to this mechanical system:
 *
 *   $ M v_t = f_ext $
 *
 *   $ M v_{t+h} = f_ext{t+h} $
 *   $           = f_ext{t} + h (df_ext/dt){t+h} $
 *   $           = f_ext{t} + h (df_ext/dx){t+h} v_{t+h} $
 *   $           = f_ext{t} - h K v_{t+h} $
 *
 *   $ ( M + h K ) v_{t+h} = f_ext $
 *
 */
class SOFA_COMPONENT_ODESOLVER_API EulerImplicitSolver : public sofa::component::odesolver::OdeSolverImpl
{
public:
    SOFA_CLASS(EulerImplicitSolver, sofa::component::odesolver::OdeSolverImpl);

    Data<double> f_rayleighStiffness;
    Data<double> f_rayleighMass;
    Data<double> f_velocityDamping;
    Data<bool> f_firstOrder;
    Data<bool> f_verbose;

    EulerImplicitSolver();

    void init();

    void solve (double dt, sofa::core::componentmodel::behavior::BaseMechanicalState::VecId xResult, sofa::core::componentmodel::behavior::BaseMechanicalState::VecId vResult);

    /// Given a displacement as computed by the linear system inversion, how much will it affect the velocity
    ///
    /// This method is used to compute the compliance for contact corrections
    /// For Euler methods, it is typically dt.
    virtual double getVelocityIntegrationFactor() const
    {
        return 1.0; // getContext()->getDt();
    }

    /// Given a displacement as computed by the linear system inversion, how much will it affect the position
    ///
    /// This method is used to compute the compliance for contact corrections
    /// For Euler methods, it is typically dt².
    virtual double getPositionIntegrationFactor() const
    {
        return getContext()->getDt(); //*getContext()->getDt());
    }

    /// Given an input derivative order (0 for position, 1 for velocity, 2 for acceleration),
    /// how much will it affect the output derivative of the given order.
    ///
    /// This method is used to compute the compliance for contact corrections.
    /// For example, a backward-Euler dynamic implicit integrator would use:
    /// Input:      x_t  v_t  a_{t+dt}
    /// x_{t+dt}     1    dt  dt^2
    /// v_{t+dt}     0    1   dt
    ///
    /// If the linear system is expressed on s = a_{t+dt} dt, then the final factors are:
    /// Input:      x_t   v_t    a_t  s
    /// x_{t+dt}     1    dt     0    dt
    /// v_{t+dt}     0    1      0    1
    /// a_{t+dt}     0    0      0    1/dt
    /// The last column is returned by the getSolutionIntegrationFactor method.
    double getIntegrationFactor(int inputDerivative, int outputDerivative) const
    {
        const double dt = getContext()->getDt();
        double matrix[3][3] =
        {
            { 1, dt, 0},
            { 0, 1, 0},
            { 0, 0, 0}
        };
        if (inputDerivative >= 3 || outputDerivative >= 3)
            return 0;
        else
            return matrix[outputDerivative][inputDerivative];
    }

    /// Given a solution of the linear system,
    /// how much will it affect the output derivative of the given order.
    double getSolutionIntegrationFactor(int outputDerivative) const
    {
        const double dt = getContext()->getDt();
        double vect[3] = { dt, 1, 1/dt};
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
