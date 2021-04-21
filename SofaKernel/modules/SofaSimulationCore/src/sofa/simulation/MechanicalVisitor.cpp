/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#define SOFA_SIMULATION_MECHANICALVISITOR_CPP
#include <sofa/simulation/MechanicalVisitor.h>
#include <sofa/core/MechanicalParams.h>
#include <sofa/simulation/Node.h>
#include <sofa/simulation/LocalStorage.h>
#include <sofa/core/behavior/BaseMass.h>
#include <sofa/core/behavior/ConstraintSolver.h>
#include <sofa/core/behavior/BaseInteractionConstraint.h>
#include <sofa/core/behavior/BaseInteractionForceField.h>
#include <sofa/core/behavior/BaseProjectiveConstraintSet.h>
#include <sofa/core/behavior/BaseInteractionProjectiveConstraintSet.h>
#include <sofa/core/behavior/BaseConstraintSet.h>
#include <sofa/core/behavior/OdeSolver.h>
#include <sofa/core/CollisionModel.h>
#include <iostream>

namespace sofa
{

namespace simulation
{
using namespace sofa::core;

Visitor::Result MechanicalGetDimensionVisitor::fwdMechanicalState(VisitorContext* ctx, core::behavior::BaseMechanicalState* mm)
{
    const std::size_t n = mm->getMatrixSize();
    *ctx->nodeData += (SReal)n;
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalIntegrationVisitor::fwdOdeSolver(simulation::Node* node, core::behavior::OdeSolver* obj)
{
    SReal nextTime = node->getTime() + dt;
    MechanicalBeginIntegrationVisitor beginVisitor( this->params, dt );
    node->execute(&beginVisitor);

    sofa::core::MechanicalParams mparams(*this->params);
    mparams.setDt(dt);

    {
        unsigned int constraintId=0;
        core::ConstraintParams cparams;
        simulation::MechanicalAccumulateConstraint(&cparams, core::MatrixDerivId::constraintJacobian(), constraintId).execute(node);

    }
    obj->solve(params, dt);

    MechanicalProjectPositionAndVelocityVisitor(&mparams, nextTime,VecCoordId::position(),VecDerivId::velocity()
            ).execute( node );

    MechanicalPropagateOnlyPositionAndVelocityVisitor(&mparams, nextTime,VecCoordId::position(),VecDerivId::velocity(), true).execute( node );

    MechanicalEndIntegrationVisitor endVisitor( this->params, dt );
    node->execute(&endVisitor);

    return RESULT_PRUNE;
}


Visitor::Result MechanicalIntegrationVisitor::fwdInteractionForceField(simulation::Node* /*node*/, core::behavior::BaseInteractionForceField* obj)
{
    MultiVecDerivId   ffId      = VecDerivId::externalForce();
    MechanicalParams m_mparams(*this->params);
    m_mparams.setDt(this->dt);

    obj->addForce(&m_mparams, ffId);
    return RESULT_CONTINUE;
}

template< VecType vtype>
Visitor::Result MechanicalVInitVisitor<vtype>::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState *mm)
{
    mm->vInit(this->params, vDest.getId(mm), vSrc.getId(mm));
    return RESULT_CONTINUE;
}

template< VecType vtype>
Visitor::Result MechanicalVInitVisitor<vtype>::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    if (m_propagate)
    {
        mm->vInit(this->params, vDest.getId(mm), vSrc.getId(mm));
    }

    return RESULT_CONTINUE;
}


template< VecType vtype>
std::string  MechanicalVInitVisitor<vtype>::getInfos() const
{
    std::string name = "[" + vDest.getName() + "]";
    return name;
}

template< VecType vtype>
Visitor::Result  MechanicalVAvailVisitor<vtype>::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->vAvail( this->params, v );
    this->states.insert(mm);
    return RESULT_CONTINUE;
}

template< VecType vtype>
std::string  MechanicalVAvailVisitor<vtype>::getInfos() const
{
    std::string name="[" + v.getName() + "]";
    return name;
}



template< VecType vtype>
Visitor::Result MechanicalVAllocVisitor<vtype>::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->vAlloc(this->params, v.getId(mm) );
    return RESULT_CONTINUE;
}


template< VecType vtype>
std::string  MechanicalVAllocVisitor<vtype>::getInfos() const
{
    std::string name="[" + v.getName() + "]";
    return name;
}

template< VecType vtype>
Visitor::Result MechanicalVReallocVisitor<vtype>::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState *mm)
{
    mm->vRealloc( this->params, this->getId(mm) );
    return RESULT_CONTINUE;
}

template< VecType vtype>
Visitor::Result MechanicalVReallocVisitor<vtype>::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    if (m_propagate)
    {
        mm->vRealloc(this->params, this->getId(mm) );
    }

    return RESULT_CONTINUE;
}

template< VecType vtype>
Visitor::Result MechanicalVReallocVisitor<vtype>::fwdInteractionForceField(simulation::Node* /*node*/, core::behavior::BaseInteractionForceField* ff)
{
    if (m_interactionForceField)
    {
        core::behavior::BaseMechanicalState* mm = ff->getMechModel1();
        mm->vRealloc( this->params, this->getId(mm) );
        mm = ff->getMechModel2();
        mm->vRealloc( this->params, this->getId(mm) );
    }

    return RESULT_CONTINUE;
}

template< VecType vtype>
typename MechanicalVReallocVisitor<vtype>::MyVecId MechanicalVReallocVisitor<vtype>::getId( core::behavior::BaseMechanicalState* mm )
{
    MyVecId vid = v->getId(mm);
    if( vid.isNull() ) // not already allocated
    {
        vid = MyVecId(MyVecId::V_FIRST_DYNAMIC_INDEX);
        mm->vAvail( this->params, vid );
        v->setId( mm, vid );
    }
    return vid;
}

template< VecType vtype>
std::string  MechanicalVReallocVisitor<vtype>::getInfos() const
{
    std::string name = "[" + v->getName() + "]";
    return name;
}


template< VecType vtype>
Visitor::Result MechanicalVFreeVisitor<vtype>::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->vFree( this->params, v.getId(mm) );
    return RESULT_CONTINUE;
}

template< VecType vtype>
Visitor::Result MechanicalVFreeVisitor<vtype>::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->vFree( this->params, v.getId(mm) );
    return RESULT_CONTINUE;
}

template< VecType vtype>
Visitor::Result MechanicalVFreeVisitor<vtype>::fwdInteractionForceField(simulation::Node* /*node*/, core::behavior::BaseInteractionForceField* ff)
{
    if( interactionForceField )
    {
        core::behavior::BaseMechanicalState* mm = ff->getMechModel1();
        mm->vFree( this->params, v.getId(mm) );
        mm = ff->getMechModel2();
        mm->vFree( this->params, v.getId(mm) );
    }
    return RESULT_CONTINUE;
}

template< VecType vtype>
std::string  MechanicalVFreeVisitor<vtype>::getInfos() const
{
    std::string name="[" + v.getName() + "]";
    return name;
}

Visitor::Result MechanicalVOpVisitor::fwdMechanicalState(VisitorContext* ctx, core::behavior::BaseMechanicalState* mm)
{
    if (!only_mapped)
        mm->vOp(this->params, v.getId(mm) ,a.getId(mm),b.getId(mm),((ctx->nodeData && *ctx->nodeData != 1.0) ? *ctx->nodeData * f : f) );
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVOpVisitor::fwdMappedMechanicalState(VisitorContext* ctx, core::behavior::BaseMechanicalState* mm)
{
    if (mapped || only_mapped)
    {
        mm->vOp(this->params, v.getId(mm) ,a.getId(mm),b.getId(mm),((ctx->nodeData && *ctx->nodeData != 1.0) ? *ctx->nodeData * f : f) );
    }
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVMultiOpVisitor::fwdMechanicalState(VisitorContext* /*ctx*/, core::behavior::BaseMechanicalState* mm)
{
    mm->vMultiOp(this->params, ops );
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVMultiOpVisitor::fwdMappedMechanicalState(VisitorContext* ctx, core::behavior::BaseMechanicalState* mm)
{
    if (mapped)
    {
        if (ctx->nodeData && *ctx->nodeData != 1.0)
        {
            VMultiOp ops2 = ops;
            const SReal fact = *ctx->nodeData;
            for (VMultiOp::iterator it = ops2.begin(), itend = ops2.end(); it != itend; ++it)
                for (unsigned int i = 1; i < it->second.size(); ++i)
                    it->second[i].second *= fact;
            mm->vMultiOp(this->params, ops2 );
        }
        else
        {
            mm->vMultiOp(this->params, ops );
        }
    }
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalVDotVisitor::fwdMechanicalState(VisitorContext* ctx, core::behavior::BaseMechanicalState* mm)
{
    *ctx->nodeData += mm->vDot(this->params, a.getId(mm),b.getId(mm) );
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalVNormVisitor::fwdMechanicalState(VisitorContext* /*ctx*/, core::behavior::BaseMechanicalState* mm)
{
    if( l>0 ) accum += mm->vSum(this->params, a.getId(mm), l );
    else {
        SReal mmax = mm->vMax(this->params, a.getId(mm) );
        if( mmax>accum ) accum=mmax;
    }
    return RESULT_CONTINUE;
}

SReal MechanicalVNormVisitor::getResult() const
{
    if( l>1 )
        return exp( log(accum) / l);
    else return accum;
}

Visitor::Result MechanicalPropagateDxVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    //<TO REMOVE>
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateDxVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom());
        ForceMaskActivate(map->getMechTo());
    }
    map->applyJ(mparams, dx, dx);

    if (!ignoreMask)
    {
        ForceMaskDeactivate(map->getMechTo());
    }

    return RESULT_CONTINUE;
}


void MechanicalPropagateDxVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    if (!ignoreMask)
    {
        mm->forceMask.activate(false);
    }
}


Visitor::Result MechanicalPropagateDxAndResetForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetForce(this->params, f.getId(mm));
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateDxAndResetForceVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
    }
    map->applyJ(mparams, dx, dx);

    if (!ignoreMask)
    {
        ForceMaskDeactivate(map->getMechTo() );
    }

    return RESULT_CONTINUE;
}


void MechanicalPropagateDxAndResetForceVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalPropagateDxAndResetForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetForce(this->params, f.getId(mm));
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateOnlyPositionAndResetForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetForce(this->params, f.getId(mm));
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPropagateOnlyPositionAndResetForceVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
    }
    map->apply(mparams, x, x);
    if (!ignoreMask)
    {
        ForceMaskDeactivate(map->getMechTo() );
    }


    return RESULT_CONTINUE;
}

Visitor::Result MechanicalPropagateOnlyPositionAndResetForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetForce(this->params, f.getId(mm));
    return RESULT_CONTINUE;
}

void MechanicalPropagateOnlyPositionAndResetForceVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalAddMDxVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAddMDxVisitor::fwdMass(simulation::Node* /*node*/, core::behavior::BaseMass* mass)
{
    mass->addMDx(mparams, res, factor);
    return RESULT_PRUNE;
}


Visitor::Result MechanicalAccFromFVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    /////<TO REMOVE>
    /// \todo Check presence of Mass
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAccFromFVisitor::fwdMass(simulation::Node* /*node*/, core::behavior::BaseMass* mass)
{
    mass->accFromF(mparams, a);
    return RESULT_CONTINUE;
}

MechanicalPropagateOnlyPositionAndVelocityVisitor::MechanicalPropagateOnlyPositionAndVelocityVisitor(
    const sofa::core::MechanicalParams* mparams,
    SReal time, MultiVecCoordId x, MultiVecDerivId v, bool m)
    : MechanicalVisitor(mparams), currentTime(time), x(x), v(v), ignoreMask(m)
{
#ifdef SOFA_DUMP_VISITOR_INFO
    setReadWriteVectors();
#endif
}

Visitor::Result MechanicalAddMDxVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* /*map*/)
{
    return RESULT_PRUNE;
}

Visitor::Result MechanicalAddMDxVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_PRUNE;
}

Visitor::Result MechanicalProjectJacobianMatrixVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* /*map*/)
{
    return RESULT_PRUNE;
}
Visitor::Result MechanicalProjectJacobianMatrixVisitor::fwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectJacobianMatrix(mparams, cId);
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalProjectVelocityVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* /*map*/)
{
    return RESULT_PRUNE;
}
Visitor::Result MechanicalProjectVelocityVisitor::fwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectVelocity(mparams, vel);
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalProjectPositionVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* /*map*/)
{
    return RESULT_PRUNE;
}
Visitor::Result MechanicalProjectPositionVisitor::fwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectPosition(mparams, pos);
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalProjectPositionAndVelocityVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* /*map*/)
{
    return RESULT_PRUNE;
}
Visitor::Result MechanicalProjectPositionAndVelocityVisitor::fwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectPosition(mparams /* PARAMS FIRST */, pos);
    c->projectVelocity(mparams /* PARAMS FIRST */, vel);
    return RESULT_CONTINUE;
}

MechanicalPropagateOnlyPositionVisitor::MechanicalPropagateOnlyPositionVisitor(const sofa::core::MechanicalParams* mparams, SReal t, MultiVecCoordId x, bool m )
    : MechanicalVisitor(mparams) , t(t), x(x), ignoreMask(m)
{
#ifdef SOFA_DUMP_VISITOR_INFO
    setReadWriteVectors();
#endif
}


Visitor::Result MechanicalPropagateOnlyPositionVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalPropagateOnlyPositionVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
    }
    map->apply(mparams, x, x);

    if (!ignoreMask)
    {
        ForceMaskDeactivate( map->getMechTo() );
    }

    return RESULT_CONTINUE;
}

void MechanicalPropagateOnlyPositionVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}

Visitor::Result MechanicalPropagateOnlyPositionAndVelocityVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalPropagateOnlyPositionAndVelocityVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
    }
    map->apply(mparams, x, x);
    map->applyJ(mparams, v, v);
    if (!ignoreMask)
    {
        ForceMaskDeactivate( map->getMechTo() );
    }
    return RESULT_CONTINUE;
}

void MechanicalPropagateOnlyPositionAndVelocityVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


MechanicalPropagateOnlyVelocityVisitor::MechanicalPropagateOnlyVelocityVisitor(
    const sofa::core::MechanicalParams* mparams,
    SReal time, MultiVecDerivId v
    , bool m)
    : MechanicalVisitor(mparams), currentTime(time), v(v)
    , ignoreMask(m)
{
#ifdef SOFA_DUMP_VISITOR_INFO
    setReadWriteVectors();
#endif
}

Visitor::Result MechanicalPropagateOnlyVelocityVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalPropagateOnlyVelocityVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
    }
    map->applyJ(mparams, v, v);
    if (!ignoreMask)
    {
        ForceMaskDeactivate( map->getMechTo() );
    }
    return RESULT_CONTINUE;
}

void MechanicalPropagateOnlyVelocityVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}

MechanicalSetPositionAndVelocityVisitor::MechanicalSetPositionAndVelocityVisitor(const sofa::core::MechanicalParams* mparams ,
        SReal time, MultiVecCoordId x, MultiVecDerivId v)
    : MechanicalVisitor(mparams) , t(time), x(x), v(v)
{
#ifdef SOFA_DUMP_VISITOR_INFO
    setReadWriteVectors();
#endif
}

Visitor::Result MechanicalSetPositionAndVelocityVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{

    return RESULT_CONTINUE;
}



Visitor::Result MechanicalResetForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    if (!onlyMapped)
        mm->resetForce(this->params, res.getId(mm));
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalResetForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetForce(this->params, res.getId(mm));
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->accumulateForce(this->params, res.getId(mm));
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->accumulateForce(this->params, res.getId(mm));
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeForceVisitor::fwdForceField(simulation::Node* /*node*/, core::behavior::BaseForceField* ff)
{
    if( !neglectingCompliance || !ff->isCompliance.getValue() ) ff->addForce(this->mparams, res);
    else ff->updateForceMask(); // compliances must update the force mask too
    return RESULT_CONTINUE;
}

void MechanicalComputeForceVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (accumulate)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );

        map->applyJT(mparams, res, res);

        ForceMaskDeactivate( map->getMechTo() );
    }
}


void MechanicalComputeForceVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}


Visitor::Result MechanicalComputeDfVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /* mm */)
{
    //<TO REMOVE>
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeDfVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeDfVisitor::fwdForceField(simulation::Node* /*node*/, core::behavior::BaseForceField* ff)
{
    if( !ff->isCompliance.getValue() ) ff->addDForce(this->mparams, res);
    return RESULT_CONTINUE;
}


void MechanicalComputeDfVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (accumulate)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
        map->applyJT(mparams, res, res);  // apply material stiffness: variation of force below the mapping
        if( mparams->kFactor() ) map->applyDJT(mparams, res, res); // apply geometric stiffness: variation due to a change of mapping, with a constant force below the mapping
        ForceMaskDeactivate( map->getMechTo() );
    }
}


void MechanicalComputeDfVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}

Visitor::Result MechanicalComputeGeometricStiffness::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    map->updateK( mparams, childForce );
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAddMBKdxVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAddMBKdxVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAddMBKdxVisitor::fwdForceField(simulation::Node* /*node*/, core::behavior::BaseForceField* ff)
{
    if( !ff->isCompliance.getValue() )
        ff->addMBKdx( this->mparams, res);
    else
        ff->addMBKdx( &mparamsWithoutStiffness, res);
    return RESULT_CONTINUE;
}


void MechanicalAddMBKdxVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (accumulate)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );

        map->applyJT(mparams, res, res);
        if( mparams->kFactor() ) map->applyDJT(mparams, res, res);
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
    mm->resetConstraint(m_cparams);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalResetConstraintVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->resetConstraint(m_cparams);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalResetConstraintVisitor::fwdConstraintSet(simulation::Node* /*node*/, core::behavior::BaseConstraintSet* c)
{
    c->resetConstraint();
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalAccumulateConstraint::fwdConstraintSet(simulation::Node* node, core::behavior::BaseConstraintSet* c)
{
    ctime_t t0 = begin(node, c);
    c->buildConstraintMatrix(cparams, res, contactId);
    end(node, c, t0);
    return RESULT_CONTINUE;
}

void MechanicalAccumulateConstraint::bwdMechanicalMapping(simulation::Node* node, core::BaseMapping* map)
{
    ctime_t t0 = begin(node, map);
    map->applyJT(cparams, res, res);
    end(node, map, t0);
}


Visitor::Result MechanicalBuildConstraintMatrix::fwdConstraintSet(simulation::Node* node, core::behavior::BaseConstraintSet* c)
{
    ctime_t t0 = begin(node, c);
    c->buildConstraintMatrix(cparams, res, contactId);
    end(node, c, t0);
    return RESULT_CONTINUE;
}


void MechanicalAccumulateMatrixDeriv::bwdMechanicalMapping(simulation::Node* node, core::BaseMapping* map)
{
    ctime_t t0 = begin(node, map);
    map->applyJT(cparams, res, res);
    end(node, map, t0);
}


Visitor::Result MechanicalApplyConstraintsVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalApplyConstraintsVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_CONTINUE;
}


void MechanicalApplyConstraintsVisitor::bwdProjectiveConstraintSet(simulation::Node* /*node*/, core::behavior::BaseProjectiveConstraintSet* c)
{
    c->projectResponse(mparams, res);
    if (W != nullptr)
    {
        c->projectResponse(mparams, W);
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
    mm->endIntegration(params, dt);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalEndIntegrationVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->endIntegration(params, dt);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalComputeContactForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->accumulateForce(this->params, res.getId(mm));
    return RESULT_PRUNE;
}


Visitor::Result MechanicalComputeContactForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->accumulateForce(this->params, res.getId(mm));
    return RESULT_CONTINUE;
}


void MechanicalComputeContactForceVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    ForceMaskActivate(map->getMechFrom() );
    ForceMaskActivate(map->getMechTo() );
    map->applyJT(mparams, res, res);
    ForceMaskDeactivate(map->getMechTo() );
}


Visitor::Result MechanicalAddSeparateGravityVisitor::fwdMass(simulation::Node* /*node*/, core::behavior::BaseMass* mass)
{
    if( mass->m_separateGravity.getValue() )
    {
        //<TO REMOVE>
        mass->addGravityToV(this->mparams, res);
    }
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPickParticlesVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    if (mm->hasTag(tagNoPicking)) // picking disabled for this model
        return RESULT_CONTINUE;

    //We deactivate the Picking with static objects (not simulated)
    core::CollisionModel *c;
    mm->getContext()->get(c, core::objectmodel::BaseContext::Local);
    if (c && !c->isSimulated()) //If it is an obstacle, we don't try to pick
    {
        return RESULT_CONTINUE;
    }
    mm->pickParticles(this->params, rayOrigin[0], rayOrigin[1], rayOrigin[2], rayDirection[0], rayDirection[1], rayDirection[2], radius0, dRadius, particles);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPickParticlesVisitor::fwdMappedMechanicalState(simulation::Node* node, core::behavior::BaseMechanicalState* mm)
{
    if (mm->hasTag(tagNoPicking)) // picking disabled for this model
        return RESULT_CONTINUE;

    if (node->mechanicalMapping  && !node->mechanicalMapping->isMechanical())
        return RESULT_PRUNE;
    mm->pickParticles(this->params, rayOrigin[0], rayOrigin[1], rayOrigin[2], rayDirection[0], rayDirection[1], rayDirection[2], radius0, dRadius, particles);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPickParticlesVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (!map->isMechanical())
        return RESULT_PRUNE;
    return RESULT_CONTINUE;
}



/// get the closest pickable particle
void MechanicalPickParticlesVisitor::getClosestParticle( core::behavior::BaseMechanicalState*& mstate, sofa::Index& indexCollisionElement, defaulttype::Vector3& point, SReal& rayLength )
{
    mstate = nullptr;

    if( particles.empty() ) return;

    rayLength = std::numeric_limits<SReal>::max();

    core::behavior::BaseMechanicalState* mstatei;
    unsigned int indexCollisionElementi;
    defaulttype::Vector3 pointi;
    SReal rayLengthi;

    // particles are sorted from their distance to the ray
    // threshold for valid particles is the shortest distance + small tolerance relative to ray length
    SReal dmax = particles.begin()->first + radius0*1e-10;

    for( Particles::const_iterator it=particles.begin(), itend=particles.end() ; it!=itend ; ++it )
    {
        if( it->first > dmax ) break; // from now on, particles are too far from the ray

        // get current valid particle
        mstatei = it->second.first;
        indexCollisionElementi = it->second.second;
        pointi[0] = mstatei->getPX(indexCollisionElementi);
        pointi[1] = mstatei->getPY(indexCollisionElementi);
        pointi[2] = mstatei->getPZ(indexCollisionElementi);
        rayLengthi = (pointi-rayOrigin)*rayDirection;

        if( rayLengthi < rayLength ) // keep the closest particle to the ray origin
        {
            mstate = mstatei;
            indexCollisionElement = indexCollisionElementi;
            point = pointi;
            rayLength = rayLengthi;
        }
    }
}

Visitor::Result MechanicalPickParticlesWithTagsVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    if(!isComponentTagIncluded(mm))
        return RESULT_CONTINUE;

    //We deactivate the Picking with static objects (not simulated)
    core::CollisionModel *c;
    mm->getContext()->get(c, core::objectmodel::BaseContext::Local);
    if (c && !c->isSimulated()) //If it is an obstacle, we don't try to pick
    {
        return RESULT_CONTINUE;
    }
    mm->pickParticles(this->params, rayOrigin[0], rayOrigin[1], rayOrigin[2], rayDirection[0], rayDirection[1], rayDirection[2], radius0, dRadius, particles);

    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPickParticlesWithTagsVisitor::fwdMappedMechanicalState(simulation::Node* node, core::behavior::BaseMechanicalState* mm)
{
    if (node->mechanicalMapping  && !node->mechanicalMapping->isMechanical())
        return RESULT_PRUNE;

    if(!isComponentTagIncluded(mm))
        return RESULT_CONTINUE;

    mm->pickParticles(this->params, rayOrigin[0], rayOrigin[1], rayOrigin[2], rayDirection[0], rayDirection[1], rayDirection[2], radius0, dRadius, particles);
    return RESULT_CONTINUE;
}


Visitor::Result MechanicalPickParticlesWithTagsVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (!map->isMechanical())
        return RESULT_PRUNE;
    return RESULT_CONTINUE;
}

bool MechanicalPickParticlesWithTagsVisitor::isComponentTagIncluded(const behavior::BaseMechanicalState *mm)
{
    bool tagOk = mustContainAllTags || tags.empty();
    for(std::list<sofa::core::objectmodel::Tag>::const_iterator tagIt = tags.begin(); tags.end() != tagIt; ++tagIt)
    {
        if (!mm->hasTag(*tagIt)) // picking disabled for this model
        {
            if(mustContainAllTags)
            {
                tagOk = false;
                break;
            }
        }
        else if (mm->hasTag(*tagIt)) // picking disabled for this model
        {
            tagOk = true;

            if(!mustContainAllTags)
                break;
        }
    }
    return tagOk;
}

/// get the closest pickable particle
void MechanicalPickParticlesWithTagsVisitor::getClosestParticle( core::behavior::BaseMechanicalState*& mstate, unsigned int& indexCollisionElement, defaulttype::Vector3& point, SReal& rayLength )
{
    mstate = nullptr;

    if( particles.empty() ) return;

    rayLength = std::numeric_limits<SReal>::max();

    core::behavior::BaseMechanicalState* mstatei;
    unsigned int indexCollisionElementi;
    defaulttype::Vector3 pointi;
    SReal rayLengthi;

    // particles are sorted from their distance to the ray
    // threshold for valid particles is the shortest distance + small tolerance relative to ray length
    SReal dmax = particles.begin()->first + radius0*1e-10;

    for( Particles::const_iterator it=particles.begin(), itend=particles.end() ; it!=itend ; ++it )
    {
        if( it->first > dmax ) break; // from now on, particles are too far from the ray

        // get current valid particle
        mstatei = it->second.first;
        indexCollisionElementi = it->second.second;
        pointi[0] = mstatei->getPX(indexCollisionElementi);
        pointi[1] = mstatei->getPY(indexCollisionElementi);
        pointi[2] = mstatei->getPZ(indexCollisionElementi);
        rayLengthi = (pointi-rayOrigin)*rayDirection;

        if( rayLengthi < rayLength ) // keep the closest particle to the ray origin
        {
            mstate = mstatei;
            indexCollisionElement = indexCollisionElementi;
            point = pointi;
            rayLength = rayLengthi;
        }
    }
}



Visitor::Result MechanicalVSizeVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    ConstVecId id = v.getId(mm);
    if( !id.isNull() )
        *result += mm->vSize(this->params, id );
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalVSizeVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    ConstVecId id = v.getId(mm);
    if( !id.isNull() )
        *result += mm->vSize(this->params, id );
    return RESULT_CONTINUE;
}

std::string MechanicalVOpVisitor::getInfos() const
{
    std::string info="v=";
    std::string aLabel;
    std::string bLabel;
    std::string fLabel;

    std::ostringstream out;
    out << "f["<<f<<"]";
    fLabel+= out.str();

    if (!a.isNull())
    {
        info+="a";
        aLabel="a[" + a.getName() + "] ";
        if (!b.isNull())
        {
            info += "+b*f";
            bLabel += "b[" + b.getName() + "] ";
        }
    }
    else
    {
        if (!b.isNull())
        {
            info += "b*f";
            bLabel += "b[" + b.getName() + "] ";
        }
        else
        {
            info+="zero"; fLabel.clear();
        }
    }
    info += " : with v[" + v.getName() + "] " + aLabel + bLabel + fLabel;
    return info;
}


std::string MechanicalVMultiOpVisitor::getInfos() const
{
    std::ostringstream out;
    for(VMultiOp::const_iterator it = ops.begin(), itend = ops.end(); it != itend; ++it)
    {
        if (it != ops.begin())
            out << " ;   ";
        core::MultiVecId r = it->first;
        out << r.getName();
        const helper::vector< std::pair< core::ConstMultiVecId, SReal > >& operands = it->second;
        int nop = (int)operands.size();
        if (nop==0)
        {
            out << " = 0";
        }
        else if (nop==1)
        {
            if (operands[0].first.getName() == r.getName())
                out << " *= " << operands[0].second;
            else
            {
                out << " = " << operands[0].first.getName();
                if (operands[0].second != 1.0)
                    out << "*"<<operands[0].second;
            }
        }
        else
        {
            int i;
            if (operands[0].first.getName() == r.getName() && operands[0].second == 1.0)
            {
                out << " +=";
                i = 1;
            }
            else
            {
                out << " =";
                i = 0;
            }
            for (; i<nop; ++i)
            {
                out << " " << operands[i].first.getName();
                if (operands[i].second != 1.0)
                    out << "*"<<operands[i].second;
                if (i < nop-1)
                    out << " +";
            }
        }
    }
    return out.str();
}

std::string MechanicalVNormVisitor::getInfos() const
{
   std::string name("v= norm(a) with a[");
   name += a.getName() + "]";
   return name;
}

std::string MechanicalAccFromFVisitor::getInfos() const
{
    std::string name="a["+a.getName()+"] f["+mparams->f().getName()+"]"; return name;
}


template class SOFA_SIMULATION_CORE_API MechanicalVAvailVisitor<V_COORD>;
template class SOFA_SIMULATION_CORE_API MechanicalVAvailVisitor<V_DERIV>;
template class SOFA_SIMULATION_CORE_API MechanicalVAllocVisitor<V_COORD>;
template class SOFA_SIMULATION_CORE_API MechanicalVAllocVisitor<V_DERIV>;
template class SOFA_SIMULATION_CORE_API MechanicalVReallocVisitor<V_COORD>;
template class SOFA_SIMULATION_CORE_API MechanicalVReallocVisitor<V_DERIV>;
template class SOFA_SIMULATION_CORE_API MechanicalVFreeVisitor<V_COORD>;
template class SOFA_SIMULATION_CORE_API MechanicalVFreeVisitor<V_DERIV>;
template class SOFA_SIMULATION_CORE_API MechanicalVInitVisitor<V_COORD>;
template class SOFA_SIMULATION_CORE_API MechanicalVInitVisitor<V_DERIV>;


} // namespace simulation

} // namespace sofa

