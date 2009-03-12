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
// Author: François Faure, INRIA-UJF, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
#include <sofa/component/odesolver/EulerImplicitSolver.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/core/ObjectFactory.h>
#include <math.h>
#include <iostream>
#include "sofa/helper/system/thread/CTime.h"




namespace sofa
{

namespace component
{

namespace odesolver
{

using namespace sofa::defaulttype;
using namespace core::componentmodel::behavior;

EulerImplicitSolver::EulerImplicitSolver()
    : f_rayleighStiffness( initData(&f_rayleighStiffness,0.1,"rayleighStiffness","Rayleigh damping coefficient related to stiffness") )
    , f_rayleighMass( initData(&f_rayleighMass,0.1,"rayleighMass","Rayleigh damping coefficient related to mass"))
    , f_velocityDamping( initData(&f_velocityDamping,0.,"vdamping","Velocity decay coefficient (no decay if null)") )
    , f_verbose( initData(&f_verbose,false,"verbose","Dump system state at each iteration") )
{
}

void EulerImplicitSolver::init()
{
    if (!this->getTags().empty())
    {
        sout << "EulerImplicitSolver: responsible for the following objects with tags " << this->getTags() << " :" << sendl;
        helper::vector<core::objectmodel::BaseObject*> objs;
        this->getContext()->get<core::objectmodel::BaseObject>(&objs,this->getTags(),sofa::core::objectmodel::BaseContext::SearchDown);
        for (unsigned int i=0; i<objs.size(); ++i)
            sout << "  " << objs[i]->getClassName() << ' ' << objs[i]->getName() << sendl;
    }
}

void EulerImplicitSolver::solve(double dt, sofa::core::componentmodel::behavior::BaseMechanicalState::VecId xResult, sofa::core::componentmodel::behavior::BaseMechanicalState::VecId vResult)
{
    MultiVector pos(this, VecId::position());
    MultiVector vel(this, VecId::velocity());
    MultiVector f(this, VecId::force());
    MultiVector b(this, VecId::V_DERIV);
    //MultiVector p(this, VecId::V_DERIV);
    //MultiVector q(this, VecId::V_DERIV);
    //MultiVector q2(this, VecId::V_DERIV);
    //MultiVector r(this, VecId::V_DERIV);
    MultiVector x(this, VecId::V_DERIV);

    double h = dt;
    //const bool printLog = f_printLog.getValue();
    const bool verbose  = f_verbose.getValue();

    //projectResponse(vel);          // initial velocities are projected to the constrained space

    // compute the right-hand term of the equation system
    // accumulation through mappings is disabled as it will be done by addMBKv after all factors are computed
    computeForce(b, true, false);             // b = f0

    // new more powerful visitors
    // b += (h+rs)df/dx v - rd M v
    // values are not cleared so that contributions from computeForces are kept and accumulated through mappings once at the end
    addMBKv(b, (f_rayleighMass.getValue() == 0.0 ? 0.0 : -f_rayleighMass.getValue()), 0, h+f_rayleighStiffness.getValue(), false, true);

    b.teq(h);                           // b = h(f0 + (h+rs)df/dx v - rd M v)

    if( verbose )
        serr<<"EulerImplicitSolver, f0 = "<< b <<sendl;

    projectResponse(b);          // b is projected to the constrained space

    if( verbose )
        serr<<"EulerImplicitSolver, projected f0 = "<< b <<sendl;

    MultiMatrix matrix(this);
    matrix = MechanicalMatrix::K * (-h*(h+f_rayleighStiffness.getValue())) + MechanicalMatrix::M * (1+h*f_rayleighMass.getValue());

    //if( verbose )
//	serr<<"EulerImplicitSolver, matrix = "<< (MechanicalMatrix::K * (-h*(h+f_rayleighStiffness.getValue())) + MechanicalMatrix::M * (1+h*f_rayleighMass.getValue())) << " = " << matrix <<sendl;

    matrix.solve(x, b);
    // projectResponse(x);
    // x is the solution of the system

    // apply the solution

    MultiVector newPos(this, xResult);
    MultiVector newVel(this, vResult);
#ifdef SOFA_NO_VMULTIOP // unoptimized version
    //vel.peq( x );                       // vel = vel + x
    newVel.eq(vel, x);
    //pos.peq( vel, h );                  // pos = pos + h vel
    newPos.eq(pos, newVel, h);

#else // single-operation optimization
    {
        typedef core::componentmodel::behavior::BaseMechanicalState::VMultiOp VMultiOp;
        VMultiOp ops;
        ops.resize(2);
        ops[0].first = (VecId)newVel;
        ops[0].second.push_back(std::make_pair((VecId)vel,1.0));
        ops[0].second.push_back(std::make_pair((VecId)x,1.0));
        ops[1].first = (VecId)newPos;
        ops[1].second.push_back(std::make_pair((VecId)pos,1.0));
        ops[1].second.push_back(std::make_pair((VecId)newVel,h));
        simulation::MechanicalVMultiOpVisitor vmop(ops);
        vmop.setTags(this->getTags());
        vmop.execute(this->getContext());
    }
#endif

    addSeparateGravity(dt, newVel);	// v += dt*g . Used if mass wants to added G separately from the other forces to v.
    if (f_velocityDamping.getValue()!=0.0)
        newVel *= exp(-h*f_velocityDamping.getValue());

    if( verbose )
    {
        serr<<"EulerImplicitSolver, final x = "<< newPos <<sendl;
        serr<<"EulerImplicitSolver, final v = "<< newVel <<sendl;
    }

#ifdef SOFA_HAVE_LAPACK
    applyConstraints();
#endif


}

SOFA_DECL_CLASS(EulerImplicitSolver)

int EulerImplicitSolverClass = core::RegisterObject("Implicit time integrator using backward Euler scheme")
        .add< EulerImplicitSolver >()
        .addAlias("EulerImplicit")
        .addAlias("ImplicitEulerSolver")
        .addAlias("ImplicitEuler")
        ;

} // namespace odesolver

} // namespace component

} // namespace sofa

