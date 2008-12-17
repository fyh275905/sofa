/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
// Author: Jeremie Allard, Sim Group @ CIMIT, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
#include <sofa/component/odesolver/BiCGStabImplicitSolver.h>
#include <math.h>
#include <iostream>
#include <sofa/core/ObjectFactory.h>




namespace sofa
{

namespace component
{

namespace odesolver
{

using namespace sofa::defaulttype;
using namespace core::componentmodel::behavior;

BiCGStabImplicitSolver::BiCGStabImplicitSolver()
{
    maxCGIter = 25;
    smallDenominatorThreshold = 1e-5;
    tolerance = 1e-5;
    rayleighStiffness = 0.1;
}

BiCGStabImplicitSolver* BiCGStabImplicitSolver::setMaxIter( int n )
{
    maxCGIter = n;
    return this;
}

void BiCGStabImplicitSolver::solve(double dt)
{
    BiCGStabImplicitSolver* group = this;
    MultiVector pos(group, VecId::position());
    MultiVector vel(group, VecId::velocity());
    MultiVector dx(group, VecId::dx());
    MultiVector f(group, VecId::force());
    MultiVector b(group, VecId::V_DERIV);
    MultiVector p(group, VecId::V_DERIV);
    //MultiVector q(group, VecId::V_DERIV);
    MultiVector r(group, VecId::V_DERIV);
    MultiVector rtilde(group, VecId::V_DERIV);
    MultiVector s(group, VecId::V_DERIV);
    MultiVector t(group, VecId::V_DERIV);
    MultiVector x(group, VecId::V_DERIV);
    MultiVector v(group, VecId::V_DERIV);
    //MultiVector z(group, VecId::V_DERIV);
    double h = dt;

    if( getDebug() )
    {
        serr<<"BiCGStabImplicitSolver, dt = "<< dt <<sendl;
        serr<<"BiCGStabImplicitSolver, initial x = "<< pos <<sendl;
        serr<<"BiCGStabImplicitSolver, initial v = "<< vel <<sendl;
    }

    addSeparateGravity(dt);	// v += dt*g . Used if mass wants to added G separately from the other forces to v.

    // compute the right-hand term of the equation system
    group->computeForce(b);             // b = f0
    if( getDebug() )
        serr<<"BiCGStabImplicitSolver, f0 = "<< b <<sendl;
    group->propagateDx(vel);            // dx = v
    group->computeDf(f);                // f = df/dx v
    b.peq(f,h);                         // b = f0+hdf/dx v
    b.teq(h);                           // b = h(f0+hdf/dx v)
    group->projectResponse(b);         // b is projected to the constrained space

    double normb = sqrt(b.dot(b));

    // -- solve the system using a bi conjugate gradient stabilized solution
    double rho_1=0, rho_2=0, alpha=0, beta=0, omega=0;
    group->v_clear( x );
    group->v_eq(r,b); // initial residual

    rtilde = r;

    if( getDebug() )
        serr<<"BiCGStabImplicitSolver, r0 = "<< r <<sendl;

    unsigned nb_iter;
    const char* endcond = "iterations";
    for( nb_iter=1; nb_iter<=maxCGIter; nb_iter++ )
    {
        //z = r; // no precond
        rho_1 = rtilde.dot(r);
        if( nb_iter==1 )
            p = r; //z;
        else
        {
            beta = ( rho_1 / rho_2 ) * ( alpha / omega );
            // p = r + beta * (p - omega * v);
            p.peq(v,-omega);
            p *= beta;
            p += r;
        }

        // matrix-vector product v = A * p
        group->propagateDx(p);          // dx = p
        group->computeDf(v);            // v = df/dx p
        v *= -h*(h+rayleighStiffness);  // v = -h(h+r) df/dx p
        group->addMdx( v, p);           // v = Mp -h(h+r) df/dx p
        // filter the product to take the constraints into account
        group->projectResponse(v);     // v is projected to the constrained space

        double den = rtilde.dot(v);
        if( fabs(den)<smallDenominatorThreshold )
        {
            endcond = "threshold1";
            break;
        }
        alpha = rho_1/den;
        s = r;
        s.peq(v,-alpha);                // s = r - alpha v
        x.peq(p,alpha);                 // x = x + alpha p

        double norms = sqrt(s.dot(s));
        if (norms / normb <= tolerance)
        {
            endcond = "tolerance1";
            break;
        }

        // matrix-vector product t = A * s
        group->propagateDx(s);          // dx = s
        group->computeDf(t);            // t = df/dx s
        t *= -h*(h+rayleighStiffness);  // t = -h(h+r) df/dx s
        group->addMdx( t, s);           // t = Ms -h(h+r) df/dx s
        // filter the product to take the constraints into account
        group->projectResponse(t);     // v is projected to the constrained space

        den = t.dot(t);
        if( fabs(den)<smallDenominatorThreshold )
        {
            endcond = "threshold2";
            break;
        }
        omega = t.dot(s)/den;

        r = s;
        r.peq(t,-omega);                // r = s - omega t
        x.peq(s,omega);                 // x = x + omega s

        double normr = sqrt(r.dot(r));
        if (normr / normb <= tolerance)
        {
            endcond = "tolerance2";
            break;
        }

        rho_2 = rho_1;
    }
    // x is the solution of the system
    serr<<"BiCGStabImplicitSolver::solve, nbiter = "<<nb_iter<<" stop because of "<<endcond<<sendl;

    // apply the solution
    vel.peq( x );                       // vel = vel + x
#ifdef SOFA_HAVE_LAPACK
    if (constraintVel.getValue())
    {
        solveConstraint(VecId::velocity());
    }
#endif
    pos.peq( vel, h );                  // pos = pos + h vel
#ifdef SOFA_HAVE_LAPACK
    if (constraintPos.getValue())
    {
        solveConstraint(VecId::position(), !constraintVel.getValue());
    }
#endif

    if( getDebug() )
    {
        serr<<"BiCGStabImplicitSolver, final x = "<< pos <<sendl;
        serr<<"BiCGStabImplicitSolver, final v = "<< vel <<sendl;
    }
}

void BiCGStabImplicitSolver::parse(core::objectmodel::BaseObjectDescription* arg)
{
    Inherited::parse(arg);
    if (arg->getAttribute("iterations"))
        this->setMaxIter( atoi(arg->getAttribute("iterations")) );
    if (arg->getAttribute("threshold"))
        this->smallDenominatorThreshold = atof(arg->getAttribute("threshold"));
    if (arg->getAttribute("tolerance"))
        this->tolerance = atof(arg->getAttribute("tolerance"));
    if (arg->getAttribute("stiffness"))
        this->rayleighStiffness = atof(arg->getAttribute("stiffness"));
    //if (arg->getAttribute("debug"))
    //    this->setDebug( atoi(arg->getAttribute("debug"))!=0 );
}

SOFA_DECL_CLASS(BiCGStabImplicit)
// Register in the Factory
int BiCGStabImplicitSolverClass = core::RegisterObject("An implicit solver able to handle some degenerate equation systems")
        .add< BiCGStabImplicitSolver >();


} // namespace odesolver

} // namespace component

} // namespace sofa
