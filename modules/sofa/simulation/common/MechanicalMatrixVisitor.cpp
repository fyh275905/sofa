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
#include <sofa/simulation/common/MechanicalMatrixVisitor.h>
#include <sofa/simulation/common/Node.h>
#include <iostream>
using std::cerr;
using std::endl;

namespace sofa
{

namespace simulation
{


Visitor::Result MechanicalMatrixVisitor::processNodeTopDown(simulation::Node* node)
{
    Result res = RESULT_CONTINUE;


    for (unsigned i=0; i<node->solver.size() && res!=RESULT_PRUNE; i++ )
    {
        ctime_t t0 = begin(node, node->solver[i]);
        res = this->fwdOdeSolver(node, node->solver[i]);
        end(node, node->solver[i], t0);
    }
    if (res != RESULT_PRUNE)
    {
        if (node->mechanicalState != NULL)
        {
            if (node->mechanicalMapping != NULL)
            {
                //cerr<<"MechanicalAction::processNodeTopDown, node "<<node->getName()<<" is a mapped model"<<endl;
                Result res2;
                ctime_t t0 = begin(node, node->mechanicalMapping);
                res = this->fwdMechanicalMapping(node, node->mechanicalMapping);
                end(node, node->mechanicalMapping, t0);
                t0 = begin(node, node->mechanicalState);
                res2 = this->fwdMappedMechanicalState(node, node->mechanicalState);
                end(node, node->mechanicalState, t0);
                if (res2 == RESULT_PRUNE)
                    res = res2;
            }
            else
            {
                //cerr<<"MechanicalAction::processNodeTopDown, node "<<node->getName()<<" is a no-map model"<<endl;
                ctime_t t0 = begin(node, node->mechanicalState);
                res = this->fwdMechanicalState(node, node->mechanicalState);
                end(node, node->mechanicalState, t0);
            }
        }
    }
    if (res != RESULT_PRUNE)
    {
        if (node->mass != NULL)
        {
            ctime_t t0 = begin(node, node->mass);
            res = this->fwdMass(node, node->mass);
            end(node, node->mass, t0);
        }
    }
    if (res != RESULT_PRUNE)
    {
        res = for_each_r(this, node, node->forceField, &MechanicalMatrixVisitor::fwdForceField);
    }
    if (res != RESULT_PRUNE)
    {
        res = for_each_r(this, node, node->interactionForceField, &MechanicalMatrixVisitor::fwdInteractionForceField);
    }
    if (res != RESULT_PRUNE)
    {
        res = for_each_r(this, node, node->constraint, &MechanicalMatrixVisitor::fwdConstraint);
    }

    offsetOnEnter = offsetOnExit;
    return res;
}

void MechanicalMatrixVisitor::processNodeBottomUp(simulation::Node* node)
{
    if (node->mechanicalState != NULL)
    {
        if (node->mechanicalMapping != NULL)
        {
            ctime_t t0 = begin(node, node->mechanicalState);
            this->bwdMappedMechanicalState(node, node->mechanicalState);
            end(node, node->mechanicalState, t0);
            t0 = begin(node, node->mechanicalMapping);
            this->bwdMechanicalMapping(node, node->mechanicalMapping);
            end(node, node->mechanicalMapping, t0);
        }
        else
        {
            ctime_t t0 = begin(node, node->mechanicalState);
            this->bwdMechanicalState(node, node->mechanicalState);
            end(node, node->mechanicalState, t0);
        }
    }
    /*    if (node->solver != NULL) {
            ctime_t t0 = begin(node, node->solver);
            this->bwdOdeSolver(node, node->solver);
            end(node, node->solver, t0);
        }*/
    for (unsigned i=0; i<node->solver.size(); i++ )
    {
        ctime_t t0 = begin(node, node->solver[i]);
        this->bwdOdeSolver(node, node->solver[i]);
        end(node, node->solver[i], t0);
    }
}

} // namespace simulation

} // namespace sofa

