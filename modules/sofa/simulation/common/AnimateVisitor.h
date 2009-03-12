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
#ifndef SOFA_SIMULATION_ANIMATEACTION_H
#define SOFA_SIMULATION_ANIMATEACTION_H

#include <sofa/simulation/common/common.h>
#include <sofa/simulation/common/Visitor.h>
#include <sofa/core/BehaviorModel.h>
#include <sofa/core/componentmodel/behavior/InteractionForceField.h>
#include <sofa/core/componentmodel/behavior/OdeSolver.h>
#include <sofa/core/componentmodel/behavior/MasterSolver.h>
#include <sofa/core/componentmodel/collision/Pipeline.h>

namespace sofa
{

namespace simulation
{

class SOFA_SIMULATION_COMMON_API AnimateVisitor : public Visitor
{
protected:
    double dt;
public:
    AnimateVisitor(double dt=0.0) : dt(dt) {}

    void setDt(double v) { dt = v; }
    double getDt() const { return dt; }

    virtual void processCollisionPipeline(simulation::Node* node, core::componentmodel::collision::Pipeline* obj);
    virtual void processBehaviorModel(simulation::Node* node, core::BehaviorModel* obj);
    virtual void fwdInteractionForceField(simulation::Node* node, core::componentmodel::behavior::InteractionForceField* obj);
    virtual void processMasterSolver(simulation::Node* node, core::componentmodel::behavior::MasterSolver* obj);
    virtual void processOdeSolver(simulation::Node* node, core::componentmodel::behavior::OdeSolver* obj);

    virtual Result processNodeTopDown(simulation::Node* node);
    //virtual void processNodeBottomUp(simulation::Node* node);

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const { return true; }

    /// Return a category name for this action.
    /// Only used for debugging / profiling purposes
    virtual const char* getCategoryName() const { return "animate"; }
    virtual const char* getClassName() const { return "AnimateVisitor"; }
};

} // namespace simulation

} // namespace sofa

#endif
