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
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/Node.h>
#include <iostream>

namespace sofa
{

namespace simulation
{
using std::cerr;
using std::endl;

Visitor::Result MechanicalVisitor::processNodeTopDown(simulation::Node* node, VisitorContext* ctx)
{
    Result res = RESULT_CONTINUE;

    for (unsigned i=0; i<node->solver.size() && res!=RESULT_PRUNE; i++ )
    {
        if(testTags(node->solver[i]))
        {
            debug_write_state_before(node->solver[i]);
            ctime_t t = begin(node, node->solver[i], "fwd");
            res = this->fwdOdeSolver(ctx, node->solver[i]);
            end(node, node->solver[i], t);
            debug_write_state_after(node->solver[i]);
        }
    }

    if (res != RESULT_PRUNE)
    {
        if (node->mechanicalState != NULL)
        {
            if (node->mechanicalMapping != NULL)
            {
                //cerr<<"MechanicalVisitor::processNodeTopDown, node "<<node->getName()<<" is a mapped model"<<endl;
                if (stopAtMechanicalMapping(node, node->mechanicalMapping))
                {
                    // stop all mechanical computations
                    //std::cerr << "Pruning " << this->getClassName() << " at " << node->getPathName() << " with non-mechanical mapping" << std::endl;
                    return RESULT_PRUNE;
                }
                //else if (!node->mechanicalMapping->isMechanical()) std::cerr << "Continuing " << this->getClassName() << " at " << node->getPathName() << " with non-mechanical mapping" << std::endl;

                Result res2 = RESULT_CONTINUE;
                if(testTags(node->mechanicalMapping))
                {
                    debug_write_state_before(node->mechanicalMapping);
                    ctime_t t = begin(node, node->mechanicalMapping, "fwd");
                    res = this->fwdMechanicalMapping(ctx, node->mechanicalMapping);
                    end(node, node->mechanicalMapping , t);
                    debug_write_state_after(node->mechanicalMapping);
                }

                if(testTags(node->mechanicalState))
                {
                    debug_write_state_before(node->mechanicalState);
                    ctime_t t = begin(node, node->mechanicalState, "fwd");
                    res2 = this->fwdMappedMechanicalState(ctx, node->mechanicalState);
                    end(node, node->mechanicalState, t);
                    debug_write_state_after(node->mechanicalState);
                }

                if (res2 == RESULT_PRUNE)
                    res = res2;
            }
            else
            {
                if(testTags(node->mechanicalState))
                {
                    //cerr<<"MechanicalVisitor::processNodeTopDown, node "<<node->getName()<<" is a no-map model"<<endl;
                    debug_write_state_before(node->mechanicalState);
                    ctime_t t = begin(node, node->mechanicalState, "fwd");
                    res = this->fwdMechanicalState(ctx, node->mechanicalState);
                    end(node, node->mechanicalState, t);
                    debug_write_state_after(node->mechanicalState);
                }
            }
        }
    }

    if (res != RESULT_PRUNE)
    {
        if (node->mass != NULL)
        {
            if(testTags(node->mass))
            {
                debug_write_state_before(node->mass);
                ctime_t t = begin(node, node->mass, "fwd");
                res = this->fwdMass(ctx, node->mass);
                end(node, node->mass, t);
                debug_write_state_after(node->mass);
            }
        }
    }

    if (res != RESULT_PRUNE)
    {
        res = for_each_r(this, ctx, node->constraintSolver, &MechanicalVisitor::fwdConstraintSolver);
    }

    if (res != RESULT_PRUNE)
    {
        res = for_each_r(this, ctx, node->forceField, &MechanicalVisitor::fwdForceField);
    }

    if (res != RESULT_PRUNE)
    {
        res = for_each_r(this, ctx, node->interactionForceField, &MechanicalVisitor::fwdInteractionForceField);
    }


    if (res != RESULT_PRUNE)
    {
        res = for_each_r(this, ctx, node->projectiveConstraintSet, &MechanicalVisitor::fwdProjectiveConstraintSet);
    }

    if (res != RESULT_PRUNE)
    {
        res = for_each_r(this, ctx, node->constraintSet, &MechanicalVisitor::fwdConstraintSet);
    }


    return res;
}


void MechanicalVisitor::processNodeBottomUp(simulation::Node* node, VisitorContext* ctx)
{
    for_each(this, ctx, node->projectiveConstraintSet, &MechanicalVisitor::bwdProjectiveConstraintSet);
    for_each(this, ctx, node->constraintSet, &MechanicalVisitor::bwdConstraintSet);
    for_each(this, ctx, node->constraintSolver, &MechanicalVisitor::bwdConstraintSolver);

    if (node->mechanicalState != NULL)
    {
        if (node->mechanicalMapping != NULL)
        {
            if (!stopAtMechanicalMapping(node, node->mechanicalMapping))
            {
                if(testTags(node->mechanicalState))
                {
                    ctime_t t = begin(node, node->mechanicalState, "bwd");
                    this->bwdMappedMechanicalState(ctx, node->mechanicalState);
                    end(node, node->mechanicalState, t);
                    t = begin(node, node->mechanicalMapping, "bwd");
                    this->bwdMechanicalMapping(ctx, node->mechanicalMapping);
                    end(node, node->mechanicalMapping, t);
                }
            }
        }
        else
        {
            if(testTags(node->mechanicalState))
            {
                ctime_t t = begin(node, node->mechanicalState, "bwd");
                this->bwdMechanicalState(ctx, node->mechanicalState);
                end(node, node->mechanicalState, t);
            }
        }

    }

    for (unsigned i=0; i<node->solver.size(); i++ )
    {
        if(testTags(node->solver[i]))
        {
            ctime_t t = begin(node, node->solver[i], "bwd");
            this->bwdOdeSolver(ctx, node->solver[i]);
            end(node, node->solver[i], t);
        }
    }

    if (node == root)
    {
        root = NULL;
    }
}


Visitor::Result MechanicalVisitor::processNodeTopDown(simulation::Node* node)
{
    if (root == NULL)
    {
        root = node;
    }

    VisitorContext ctx;
    ctx.root = root;
    ctx.node = node;
    ctx.nodeData = rootData;

    const bool writeData = writeNodeData();
    const bool readData = readNodeData();
    const bool useNodeData = readData || writeData;

    if (nodeMap && !nodeMap->empty())
    {
        // only apply visitor to the specified subset of child nodes from the root
        if (node == root)
        {
            tmpNodeDataMap.clear();
        }
        else if (node->hasParent(root))
        {
            MultiNodeDataMap::iterator it = nodeMap->find(node->getContext());
            if (it == nodeMap->end())
                return RESULT_PRUNE;
            if (useNodeData)
                ctx.nodeData = &(it->second);
        }
        else
        {
            if (useNodeData)
            {
                std::map<simulation::Node*, double*>::iterator it = tmpNodeDataMap.find(node);
                if (it != tmpNodeDataMap.end())
                    ctx.nodeData = it->second;
            }
        }
        if (useNodeData && ctx.nodeData != rootData)
        {
            // propagate the pointer to node-specific data to child nodes
            for (Node::ChildIterator it = node->child.begin(), itend = node->child.end(); it != itend; ++it)
                tmpNodeDataMap[*it] = ctx.nodeData;
        }
    }
    return processNodeTopDown(node, &ctx);
}


void MechanicalVisitor::processNodeBottomUp(simulation::Node* node)
{
    VisitorContext ctx;
    ctx.root = root;
    ctx.node = node;
    ctx.nodeData = rootData;

    const bool writeData = writeNodeData();
    const bool readData = readNodeData();
    const bool useNodeData = readData || writeData;

    if (nodeMap && !nodeMap->empty())
    {
        // only apply visitor to the specified subset of child nodes from the root
        if (node == root)
        {
            tmpNodeDataMap.clear();
        }
        else if (node->hasParent(root))
        {
            MultiNodeDataMap::iterator it = nodeMap->find(node->getContext());
            if (it == nodeMap->end())
                return; // RESULT_PRUNE;
            if (useNodeData)
                ctx.nodeData = &(it->second);
        }
        else
        {
            if (useNodeData)
            {
                std::map<simulation::Node*, double*>::iterator it = tmpNodeDataMap.find(node);
                if (it != tmpNodeDataMap.end())
                    ctx.nodeData = it->second;
            }
        }
    }

    processNodeBottomUp(node, &ctx);


    if (writeData && nodeMap && !nodeMap->empty() && node->hasParent(root) && rootData != ctx.nodeData)
        addNodeData(node, rootData, ctx.nodeData);

}


Visitor::Result MechanicalVisitor::processNodeTopDown(simulation::Node* node, LocalStorage* stack)
{
    if (root == NULL)
    {
        root = node;
    }

    VisitorContext ctx;
    ctx.root = root;
    ctx.node = node;
    ctx.nodeData = rootData;

    const bool writeData = writeNodeData();
    const bool readData = readNodeData();
    const bool useNodeData = readData || writeData;

    if (nodeMap && !nodeMap->empty())
    {
        // only apply visitor to the specified subset of child nodes from the root

        if (node == root)
        {
        }
        else if (node->hasParent(root))
        {
            MultiNodeDataMap::iterator it = nodeMap->find(node->getContext());
            if (it == nodeMap->end())
                return RESULT_PRUNE;
            if (useNodeData)
            {
                ctx.nodeData = &(it->second);
                stack->push(ctx.nodeData);
            }
        }
        else
        {
            if (useNodeData)
            {
                double* parentData = stack->empty() ? rootData : (double*)stack->top();
                if (!writeData)
                    ctx.nodeData = parentData;
                else // must create a local copy
                {
                    ctx.nodeData = new double(0.0);
                    setNodeData(node, ctx.nodeData, parentData);
                    stack->push(ctx.nodeData);
                }
            }
        }
    }
    else if (writeData)
    {
        // create temporary accumulation buffer for parallel reductions (dot products)
        if (node != root)
        {
            double* parentData = stack->empty() ? rootData : (double*)stack->top();
            ctx.nodeData = new double(0.0);
            setNodeData(node, ctx.nodeData, parentData);
            stack->push(ctx.nodeData);
        }
    }

    return processNodeTopDown(node, &ctx);
}


void MechanicalVisitor::processNodeBottomUp(simulation::Node* node, LocalStorage* stack)
{
    VisitorContext ctx;
    ctx.root = root;
    ctx.node = node;
    ctx.nodeData = rootData;
    double* parentData = rootData;

    const bool writeData = writeNodeData();
    const bool readData = readNodeData();
    const bool useNodeData = readData || writeData;

    if (nodeMap && !nodeMap->empty())
    {
        // only apply visitor to the specified subset of child nodes from the root
        if (node == root)
        {
        }
        else if (node->hasParent(root))
        {
            MultiNodeDataMap::iterator it = nodeMap->find(node->getContext());
            if (it == nodeMap->end())
                return; // RESULT_PRUNE;
            if (useNodeData)
            {
                ctx.nodeData = &(it->second);
                stack->pop();
            }
        }
        else
        {
            if (useNodeData)
            {
                ctx.nodeData = (double*)stack->top();
                if (writeData)
                    stack->pop();
                parentData = stack->empty() ? rootData : (double*)stack->top();
            }
        }
    }
    else if (writeData)
    {
        // use temporary accumulation buffer for parallel reductions (dot products)
        if (node != root)
        {
            ctx.nodeData = (double*)stack->pop();
            parentData = stack->empty() ? rootData : (double*)stack->top();
        }
    }

    processNodeBottomUp(node, &ctx);

    if (writeData && parentData != ctx.nodeData)
        addNodeData(node, parentData, ctx.nodeData);
}


#ifdef SOFA_DUMP_VISITOR_INFO

void MechanicalVisitor::printReadVectors(core::behavior::BaseMechanicalState* mm)
{
    if (!mm || !readVector.size() || !Visitor::printActivated || !Visitor::outputStateVector) return;

    printNode("Input");
    for (unsigned int i=0; i<readVector.size(); ++i) printVector(mm, readVector[i]);
    printCloseNode("Input");
}


void MechanicalVisitor::printWriteVectors(core::behavior::BaseMechanicalState* mm)
{
    if (!mm || !writeVector.size() || !Visitor::printActivated || !Visitor::outputStateVector) return;

    printNode("Output");
    for (unsigned int i=0; i<writeVector.size(); ++i) printVector(mm, writeVector[i]);
    printCloseNode("Output");
}


void MechanicalVisitor::printReadVectors(simulation::Node* node, core::objectmodel::BaseObject* obj)
{
    using sofa::core::behavior::InteractionForceField;
    using sofa::core::behavior::InteractionProjectiveConstraintSet;
    using sofa::core::behavior::InteractionConstraint;

    if (!Visitor::printActivated || !Visitor::outputStateVector) return;

    if (readVector.size())
    {
        core::behavior::BaseMechanicalState *dof1, *dof2;

        if (InteractionForceField* interact = dynamic_cast< InteractionForceField* > (obj))
        {
            dof1 = interact->getMechModel1();
            dof2 = interact->getMechModel2();
        }
        else if (InteractionProjectiveConstraintSet* interact = dynamic_cast< InteractionProjectiveConstraintSet* > (obj))
        {
            dof1 = interact->getMechModel1();
            dof2 = interact->getMechModel2();
        }
        else if (InteractionConstraint* interact = dynamic_cast< InteractionConstraint* > (obj))
        {
            dof1 = interact->getMechModel1();
            dof2 = interact->getMechModel2();
        }/*
		else if (BaseLMConstraint* interact = dynamic_cast< BaseLMConstraint* > (obj))
		{
			dof1 = interact->getConstrainedMechModel1();
			dof2 = interact->getConstrainedMechModel2();
        }*/
        else
        {
            printReadVectors(node->mechanicalState);
            return;
        }

        TRACE_ARGUMENT arg1;
        arg1.push_back(std::make_pair("type", dof1->getClassName()));
        printNode("Components", dof1->getName(), arg1);
        printReadVectors(dof1);
        printCloseNode("Components");

        TRACE_ARGUMENT arg2;
        arg2.push_back(std::make_pair("type", dof2->getClassName()));
        printNode("Components", dof2->getName(), arg2);
        printReadVectors(dof2);
        printCloseNode("Components");
    }
}


void MechanicalVisitor::printWriteVectors(simulation::Node* node, core::objectmodel::BaseObject* obj)
{
    using sofa::core::behavior::InteractionForceField;
    using sofa::core::behavior::InteractionProjectiveConstraintSet;
    using sofa::core::behavior::InteractionConstraint;
    using sofa::core::behavior::BaseMechanicalState;

    if (!Visitor::printActivated) return;

    if (writeVector.size())
    {
        BaseMechanicalState *dof1, *dof2;

        if (InteractionForceField* interact = dynamic_cast< InteractionForceField* > (obj))
        {
            dof1 = interact->getMechModel1();
            dof2 = interact->getMechModel2();
        }
        else if (InteractionProjectiveConstraintSet* interact = dynamic_cast< InteractionProjectiveConstraintSet* > (obj))
        {
            dof1 = interact->getMechModel1();
            dof2 = interact->getMechModel2();
        }
        else if (InteractionConstraint* interact = dynamic_cast< InteractionConstraint* > (obj))
        {
            dof1 = interact->getMechModel1();
            dof2 = interact->getMechModel2();
        }/*
		else if (BaseLMConstraint* interact = dynamic_cast< BaseLMConstraint* > (obj))
		{
			dof1 = interact->getConstrainedMechModel1();
			dof2 = interact->getConstrainedMechModel2();
        }*/
        else
        {
            BaseMechanicalState* dof = node->mechanicalState;
            if (dof == NULL)
                node->getContext()->get(dof);
            printWriteVectors(dof);
            return;
        }

        TRACE_ARGUMENT arg1;
        arg1.push_back(std::make_pair("type", dof1->getClassName()));
        printNode("Components", dof1->getName(), arg1);
        printWriteVectors(dof1);
        printCloseNode("Components");

        TRACE_ARGUMENT arg2;
        arg2.push_back(std::make_pair("type", dof2->getClassName()));
        printNode("Components", dof2->getName(), arg2);
        printWriteVectors(dof2);
        printCloseNode("Components");
    }
}


simulation::Node::ctime_t MechanicalVisitor::begin(simulation::Node* node, core::objectmodel::BaseObject* obj, const std::string &info)
{
    ctime_t t=Visitor::begin(node, obj, info);
    printReadVectors(node, obj);
    return t;
}


void MechanicalVisitor::end(simulation::Node* node, core::objectmodel::BaseObject* obj, ctime_t t0)
{
    printWriteVectors(node, obj);
    Visitor::end(node, obj, t0);
}
#endif


Visitor::Result MechanicalGetDimensionVisitor::fwdMechanicalState(VisitorContext* ctx, core::behavior::BaseMechanicalState* mm)
{
    const unsigned int n = mm->getMatrixSize();
    *ctx->nodeData += (double)n;
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalIntegrationVisitor::fwdOdeSolver(simulation::Node* node, core::behavior::OdeSolver* obj)
{
    double nextTime = node->getTime() + dt;
    MechanicalBeginIntegrationVisitor beginVisitor(dt);
    node->execute(&beginVisitor);

#ifdef SOFA_HAVE_EIGEN2
    {
        unsigned int constraintId=0;
        MechanicalAccumulateConstraint(constraintId).execute(node);
    }
#endif
    //cerr<<"MechanicalIntegrationVisitor::fwdOdeSolver start solve obj"<<endl;
    obj->solve(dt);
    //cerr<<"MechanicalIntegrationVisitor::fwdOdeSolver endVisitor ok"<<endl;

    //cerr<<"MechanicalIntegrationVisitor::fwdOdeSolver end solve obj"<<endl;
    obj->propagatePositionAndVelocity(nextTime,core::behavior::OdeSolver::VecId::position(),core::behavior::OdeSolver::VecId::velocity());

    MechanicalEndIntegrationVisitor endVisitor(dt);
    node->execute(&endVisitor);

    return RESULT_PRUNE;
}


Visitor::Result MechanicalIntegrationVisitor::fwdInteractionForceField(simulation::Node* /*node*/, core::behavior::InteractionForceField* obj)
{
    obj->addForce();
    return RESULT_CONTINUE;
}


Visitor::Result  MechanicalVAvailVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->vAvail(v);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVAllocVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->vAlloc(v);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVFreeVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->vFree(v);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVOpVisitor::fwdMechanicalState(VisitorContext* ctx, core::behavior::BaseMechanicalState* mm)
{
    //cerr<<"    MechanicalVOpVisitor::fwdMechanicalState, model "<<mm->getName()<<endl;
    mm->vOp(v,a,b,((ctx->nodeData && *ctx->nodeData != 1.0) ? *ctx->nodeData * f : f));
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVOpVisitor::fwdMappedMechanicalState(VisitorContext* /*ctx*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    //cerr<<"    MechanicalVOpVisitor::fwdMappedMechanicalState, model "<<mm->getName()<<endl;
    //mm->vOp(v,a,b,((ctx->nodeData && *ctx->nodeData != 1.0) ? *ctx->nodeData * f : f));
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVMultiOpVisitor::fwdMechanicalState(VisitorContext* ctx, core::behavior::BaseMechanicalState* mm)
{
    //cerr<<"    MechanicalVOpVisitor::fwdMechanicalState, model "<<mm->getName()<<endl;
    if (ctx->nodeData && *ctx->nodeData != 1.0)
    {
        VMultiOp ops2 = ops;
        const double fact = *ctx->nodeData;
        for (VMultiOp::iterator it = ops2.begin(), itend = ops2.end(); it != itend; ++it)
            for (unsigned int i = 1; i < it->second.size(); ++i)
                it->second[i].second *= fact;
        mm->vMultiOp(ops2);
    }
    else
    {
        mm->vMultiOp(ops);
    }
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVMultiOpVisitor::fwdMappedMechanicalState(VisitorContext* /*ctx*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    //cerr<<"    MechanicalVOpVisitor::fwdMappedMechanicalState, model "<<mm->getName()<<endl;
    //mm->vMultiOp(ops);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVDotVisitor::fwdMechanicalState(VisitorContext* ctx, core::behavior::BaseMechanicalState* mm)
{
    *ctx->nodeData += mm->vDot(a,b);
    return RESULT_CONTINUE;
}

#if 0
/// Parallel code
Visitor::Result MechanicalVDotVisitor::processNodeTopDown(simulation::Node* /*node*/, LocalStorage* stack)
{
    double* localTotal = new double(0.0);
    stack->push(localTotal);
    if (node->mechanicalState && !node->mechanicalMapping)
    {
        core::behavior::BaseMechanicalState* mm = node->mechanicalState;
        *localTotal += mm->vDot(a,b);
    }
    return RESULT_CONTINUE;
}


/// Parallel code
void MechanicalVDotVisitor::processNodeBottomUp(simulation::Node* /*node*/, LocalStorage* stack)
{
    double* localTotal = static_cast<double*>(stack->pop());
    double* parentTotal = static_cast<double*>(stack->top());
    if (!parentTotal)
        *total += *localTotal; // root
    else
        *parentTotal += *localTotal;
    delete localTotal;
}
#endif

Visitor::Result MechanicalPropagateDxVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setDx(dx);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateDxVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom());
        ForceMaskActivate(map->getMechTo());
        map->propagateDx();
        ForceMaskDeactivate(map->getMechTo());
    }
    else map->propagateDx();

    return RESULT_CONTINUE;
}


void MechanicalPropagateDxVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalPropagateVVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setV(v);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateVVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->propagateV();
        ForceMaskDeactivate(map->getMechTo() );
    }
    else map->propagateV();

    return RESULT_CONTINUE;
}


void MechanicalPropagateVVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalPropagateXVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setX(x);

    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateXVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->propagateX();
        ForceMaskDeactivate(map->getMechTo() );
    }
    else map->propagateX();

    return RESULT_CONTINUE;
}


void MechanicalPropagateXVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalPropagateDxAndResetForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setDx(dx);
    mm->setF(f);
    mm->resetForce();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateDxAndResetForceVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->propagateDx();
        ForceMaskDeactivate(map->getMechTo() );
    }
    else map->propagateDx();

    return RESULT_CONTINUE;
}


void MechanicalPropagateDxAndResetForceVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalPropagateDxAndResetForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetForce();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateXAndResetForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setX(x);
    mm->setF(f);
    mm->resetForce();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateXAndResetForceVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->propagateX();
        ForceMaskDeactivate(map->getMechTo() );
    }
    else map->propagateX();

    return RESULT_CONTINUE;
}


void MechanicalPropagateXAndResetForceVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalPropagateXAndResetForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetForce();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateAndAddDxVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->propagateDx();
        map->propagateV();
        ForceMaskDeactivate(map->getMechTo() );
    }
    else
    {
        map->propagateDx();
        map->propagateV();
    }
    return RESULT_CONTINUE;
}


void MechanicalPropagateAndAddDxVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalPropagateAndAddDxVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    //mm->printDOF(VecId::dx());
    mm->addDxToCollisionModel();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAddMDxVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setF(res);
    if (!dx.isNull())
        mm->setDx(dx);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAddMDxVisitor::fwdMass(simulation::Node* /*node*/, core::behavior::BaseMass* mass)
{
    mass->addMDx(factor);
    return RESULT_PRUNE;
}


Visitor::Result MechanicalAccFromFVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setDx(a);
    mm->setF(f);
    /// \todo Check presence of Mass
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAccFromFVisitor::fwdMass(simulation::Node* /*node*/, core::behavior::BaseMass* mass)
{
    mass->accFromF();
    return RESULT_CONTINUE;
}

#ifdef SOFA_SUPPORT_MAPPED_MASS

MechanicalPropagatePositionAndVelocityVisitor::MechanicalPropagatePositionAndVelocityVisitor(double t, VecId x, VecId v, VecId a, bool m) : t(t), x(x), v(v), a(a), ignoreMask(m)
{
#ifdef SOFA_DUMP_VISITOR_INFO
    setReadWriteVectors();
#endif
    //cerr<<"::MechanicalPropagatePositionAndVelocityVisitor"<<endl;
}

#else

MechanicalPropagatePositionAndVelocityVisitor::MechanicalPropagatePositionAndVelocityVisitor(double t, VecId x, VecId v, bool m) : t(t), x(x), v(v), ignoreMask(m)
{
#ifdef SOFA_DUMP_VISITOR_INFO
    setReadWriteVectors();
#endif
    //cerr<<"::MechanicalPropagatePositionAndVelocityVisitor"<<endl;
}

#endif // SOFA_SUPPORT_MAPPED_MASS

MechanicalPropagatePositionVisitor::MechanicalPropagatePositionVisitor(double t, VecId x, bool m) : t(t), x(x), ignoreMask(m)
{
#ifdef SOFA_DUMP_VISITOR_INFO
    setReadWriteVectors();
#endif
    //cerr<<"::MechanicalPropagatePositionAndVelocityVisitor"<<endl;
}

#ifdef SOFA_SUPPORT_MAPPED_MASS

Visitor::Result MechanicalAddMDxVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!dx.isNull())
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->propagateDx();
        ForceMaskDeactivate( map->getMechTo() );
    }
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAddMDxVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetForce();
    return RESULT_CONTINUE;
}


void MechanicalAddMDxVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    ForceMaskActivate(map->getMechFrom() );
    ForceMaskActivate(map->getMechTo() );
    map->accumulateForce();
    ForceMaskDeactivate( map->getMechTo() );
}


void MechanicalAddMDxVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}

#else

Visitor::Result MechanicalAddMDxVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* /*map*/)
{
    return RESULT_PRUNE;
}


Visitor::Result MechanicalAddMDxVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_PRUNE;
}

#endif // SOFA_SUPPORT_MAPPED_MASS

Visitor::Result MechanicalProjectJacobianMatrixVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* /*map*/)
{
    return RESULT_PRUNE;
}
Visitor::Result MechanicalProjectJacobianMatrixVisitor::fwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectJacobianMatrix();
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalProjectVelocityVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* /*map*/)
{
    return RESULT_PRUNE;
}
Visitor::Result MechanicalProjectVelocityVisitor::fwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectVelocity();
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalProjectPositionVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* /*map*/)
{
    return RESULT_PRUNE;
}
Visitor::Result MechanicalProjectPositionVisitor::fwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectPosition();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagatePositionVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setX(x);
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalPropagatePositionVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->propagateX();
        ForceMaskDeactivate( map->getMechTo() );
    }
    else
    {
        map->propagateX();
    }
    return RESULT_CONTINUE;
}



void MechanicalPropagatePositionVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}

Visitor::Result MechanicalPropagatePositionVisitor::fwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectPosition();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagatePositionAndVelocityVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setX(x);
    mm->setV(v);
#ifdef SOFA_SUPPORT_MAPPED_MASS
    mm->setDx(a);
    mm->resetAcc();
#endif
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalPropagatePositionAndVelocityVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->propagateX();
        map->propagateV();
#ifdef SOFA_SUPPORT_MAPPED_MASS
        map->propagateA();
#endif
        ForceMaskDeactivate( map->getMechTo() );
    }
    else
    {
        map->propagateX();
        map->propagateV();
#ifdef SOFA_SUPPORT_MAPPED_MASS
        map->propagateA();
#endif
    }
    return RESULT_CONTINUE;
}

void MechanicalPropagatePositionAndVelocityVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}

Visitor::Result MechanicalPropagatePositionAndVelocityVisitor::fwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectPosition();
    c->projectVelocity();
    return RESULT_CONTINUE;
}


#ifdef SOFA_SUPPORT_MAPPED_MASS

MechanicalSetPositionAndVelocityVisitor::MechanicalSetPositionAndVelocityVisitor(double t, VecId x, VecId v, VecId a)
    : t(t), x(x), v(v), a(a)
{

#ifdef SOFA_DUMP_VISITOR_INFO
    setReadWriteVectors();
#endif // SOFA_DUMP_VISITOR_INFO
//	cerr<<"::MechanicalSetPositionAndVelocityVisitor"<<endl;
}
#else
MechanicalSetPositionAndVelocityVisitor::MechanicalSetPositionAndVelocityVisitor(double t, VecId x, VecId v)
    : t(t), x(x), v(v)
{
#ifdef SOFA_DUMP_VISITOR_INFO
    setReadWriteVectors();
#endif
//	cerr<<"::MechanicalSetPositionAndVelocityVisitor"<<endl;
}

#endif // SOFA_SUPPORT_MAPPED_MASS

Visitor::Result MechanicalSetPositionAndVelocityVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setX(x);
    mm->setV(v);
#ifdef SOFA_SUPPORT_MAPPED_MASS
    mm->setDx(a);
    mm->resetAcc();
#endif
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateFreePositionVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setXfree(x);
    mm->setVfree(v);
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalPropagateFreePositionVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->propagateXfree();
        ForceMaskDeactivate( map->getMechTo() );
    }
    else
    {
        map->propagateXfree();
    }
    return RESULT_CONTINUE;
}

void MechanicalPropagateFreePositionVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}

Visitor::Result MechanicalPropagateFreePositionVisitor::fwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectFreePosition();
    c->projectFreeVelocity();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalResetForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setF(res);
    if (!onlyMapped)
        mm->resetForce();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalResetForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetForce();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setF(res);
    mm->accumulateForce();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->accumulateForce();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeForceVisitor::fwdForceField(simulation::Node* /*node*/, core::behavior::BaseForceField* ff)
{
    //cerr<<"MechanicalComputeForceVisitor::fwdForceField "<<ff->getName()<<endl;
    ff->addForce();
    return RESULT_CONTINUE;
}

void MechanicalComputeForceVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    //       cerr<<"MechanicalComputeForceVisitor::bwdMechanicalMapping "<<map->getName()<<endl;
    if (accumulate)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->accumulateForce();
        ForceMaskDeactivate( map->getMechTo() );
    }
}


void MechanicalComputeForceVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalComputeDfVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setF(res);
    mm->accumulateDf();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeDfVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->accumulateDf();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeDfVisitor::fwdForceField(simulation::Node* /*node*/, core::behavior::BaseForceField* ff)
{
    if (useV)
        ff->addDForceV();
    else
        ff->addDForce();
    return RESULT_CONTINUE;
}


void MechanicalComputeDfVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (accumulate)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->accumulateDf();
        ForceMaskDeactivate( map->getMechTo() );
    }
}


void MechanicalComputeDfVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalAddMBKdxVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setF(res);
    mm->accumulateDf();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAddMBKdxVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->accumulateDf();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAddMBKdxVisitor::fwdForceField(simulation::Node* /*node*/, core::behavior::BaseForceField* ff)
{
    if (useV)
        ff->addMBKv(mFactor, bFactor, kFactor);
    else
        ff->addMBKdx(mFactor, bFactor, kFactor);
    return RESULT_CONTINUE;
}


void MechanicalAddMBKdxVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (accumulate)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->accumulateDf();
        ForceMaskDeactivate( map->getMechTo() );
    }
}


void MechanicalAddMBKdxVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalResetConstraintVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    // mm->setC(res);
    mm->resetConstraint();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalResetConstraintVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetConstraint();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalResetConstraintVisitor::fwdConstraintSet(simulation::Node* /*node*/, core::behavior::BaseConstraintSet* c)
{
    // mm->setC(res);
    c->resetConstraint();
    return RESULT_CONTINUE;
}


#ifdef SOFA_HAVE_EIGEN2

//MechanicalExpressJacobianVisitor::MechanicalExpressJacobianVisitor(simulation::Node* /*n*/)
//{
//#ifdef SOFA_DUMP_VISITOR_INFO
//setReadWriteVectors();
//#endif
//constraintId=0;
//}


//Visitor::Result MechanicalExpressJacobianVisitor::fwdLMConstraint(simulation::Node* /*node*/, core::behavior::BaseLMConstraint* c)
//{
//	c->buildJacobian(constraintId);
//	return RESULT_CONTINUE;
//}


//void MechanicalExpressJacobianVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
//{
//	map->accumulateConstraint();
//}


//Visitor::Result MechanicalSolveLMConstraintVisitor::fwdConstraintSolver(simulation::Node* /*node*/, core::behavior::ConstraintSolver* s)
//{
//	typedef core::behavior::BaseMechanicalState::VecId VecId;
//	s->solveConstraint(propagateState,state);
//	return RESULT_PRUNE;
//}


Visitor::Result MechanicalWriteLMConstraint::fwdConstraintSet(simulation::Node* /*node*/, core::behavior::BaseConstraintSet* c)
{
    if (core::behavior::BaseLMConstraint* LMc=dynamic_cast<core::behavior::BaseLMConstraint* >(c))
    {
        LMc->writeConstraintEquations(order);
        datasC.push_back(LMc);
    }
    return RESULT_CONTINUE;
}

#endif

Visitor::Result MechanicalAccumulateConstraint::fwdConstraintSet(simulation::Node* /*node*/, core::behavior::BaseConstraintSet* c)
{
    c->serr << "buildConstraintMatrix " << contactId << c->sendl;
    c->buildConstraintMatrix(contactId, position);
    return RESULT_CONTINUE;
}


void MechanicalAccumulateConstraint::bwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    map->accumulateConstraint();
}


Visitor::Result MechanicalApplyConstraintsVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setDx(res);
    //mm->projectResponse();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalApplyConstraintsVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    //mm->projectResponse();
    return RESULT_CONTINUE;
}


void MechanicalApplyConstraintsVisitor::bwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectResponse();
    if (W != NULL)
    {
        c->projectResponse(W);
    }
}


Visitor::Result MechanicalBeginIntegrationVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->beginIntegration(dt);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalBeginIntegrationVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->beginIntegration(dt);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalEndIntegrationVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->endIntegration(dt);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalEndIntegrationVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->endIntegration(dt);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeComplianceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* ms)
{
    ms->getCompliance(_W);
    return RESULT_PRUNE;
}


Visitor::Result MechanicalComputeComplianceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* ms)
{
    ms->getCompliance(_W);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeContactForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->setF(res);
    mm->accumulateForce();
    return RESULT_PRUNE;
}


Visitor::Result MechanicalComputeContactForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->accumulateForce();
    return RESULT_CONTINUE;
}


void MechanicalComputeContactForceVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    ForceMaskActivate(map->getMechFrom() );
    ForceMaskActivate(map->getMechTo() );
    map->accumulateForce();
    ForceMaskDeactivate(map->getMechTo() );
}


Visitor::Result MechanicalAddSeparateGravityVisitor::fwdMass(simulation::Node* node, core::behavior::BaseMass* mass)
{
    if( mass->m_separateGravity.getValue() )
    {
        if (! (res == VecId::velocity())) dynamic_cast<core::behavior::BaseMechanicalState*>(node->getMechanicalState())->setV(res);
        mass->addGravityToV(dt);
        if (! (res == VecId::velocity())) dynamic_cast<core::behavior::BaseMechanicalState*>(node->getMechanicalState())->setV(VecId::velocity());
    }
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPickParticlesVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    //std::cout << "Picking particles on state " << mm->getName() << " within radius " << radius0 << " + dist * " << dRadius << std::endl;

    //We deactivate the Picking with static objects (not simulated)
    core::CollisionModel *c;
    mm->getContext()->get(c, core::objectmodel::BaseContext::Local);
    if (c && !c->isSimulated()) //If it is an obstacle, we don't try to pick
    {
        return RESULT_CONTINUE;
    }

    mm->pickParticles(rayOrigin[0], rayOrigin[1], rayOrigin[2], rayDirection[0], rayDirection[1], rayDirection[2], radius0, dRadius, particles);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPickParticlesVisitor::fwdMappedMechanicalState(simulation::Node* node, core::behavior::BaseMechanicalState* mm)
{
    if (node->mechanicalMapping  && !node->mechanicalMapping->isMechanical())
        return RESULT_PRUNE;
    mm->pickParticles(rayOrigin[0], rayOrigin[1], rayOrigin[2], rayDirection[0], rayDirection[1], rayDirection[2], radius0, dRadius, particles);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPickParticlesVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::behavior::BaseMechanicalMapping* map)
{
    if (!map->isMechanical())
        return RESULT_PRUNE;
    return RESULT_CONTINUE;
}

} // namespace simulation

} // namespace sofa

