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
#ifndef SOFA_SIMULATION_MECHANICALACTION_H
#define SOFA_SIMULATION_MECHANICALACTION_H
#define SOFA_SUPPORT_MAPPED_MASS
#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif


#include <sofa/simulation/common/Visitor.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalState.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalMapping.h>
#include <sofa/core/componentmodel/behavior/Mass.h>
#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/behavior/InteractionForceField.h>
#include <sofa/core/componentmodel/behavior/InteractionConstraint.h>
#include <sofa/core/componentmodel/behavior/Constraint.h>
//#include <sofa/defaulttype/BaseMatrix.h>
//#include <sofa/defaulttype/BaseVector.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/helper/map.h>
#include <iostream>

namespace sofa
{

namespace simulation
{

using std::cerr;
using std::endl;

using namespace sofa::defaulttype;

typedef std::map<core::objectmodel::BaseContext*, double> MultiNodeDataMap;

/** Base class for easily creating new actions for mechanical simulation.

During the first traversal (top-down), method processNodeTopDown(simulation::Node*) is applied to each simulation::Node. Each component attached to this node is processed using the appropriate method, prefixed by fwd.

During the second traversal (bottom-up), method processNodeBottomUp(simulation::Node*) is applied to each simulation::Node. Each component attached to this node is processed using the appropriate method, prefixed by bwd.

The default behavior of the fwd* and bwd* is to do nothing. Derived actions typically overload these methods to implement the desired processing.

*/
class SOFA_SIMULATION_COMMON_API MechanicalVisitor : virtual public Visitor
{

protected:
    bool prefetching;
    simulation::Node* root; ///< root node from which the visitor was executed
    double* rootData; ///< data for root node
    MultiNodeDataMap* nodeMap;

    /// Temporary node -> double* map to sequential traversals requiring node-specific data
    std::map<simulation::Node*, double*> tmpNodeDataMap;

    virtual Result processNodeTopDown(simulation::Node* node, VisitorContext* ctx);
    virtual void processNodeBottomUp(simulation::Node* node, VisitorContext* ctx);

public:

    MechanicalVisitor() : prefetching(false), root(NULL), rootData(NULL), nodeMap(NULL) {}

    MechanicalVisitor& setNodeMap(MultiNodeDataMap* m) { nodeMap = m; return *this; }

    MultiNodeDataMap* getNodeMap() { return nodeMap; }

    /// Return true if this visitor need to read the node-specific data if given
    virtual bool readNodeData() const
    { return false; }

    /// Return true if this visitor need to write to the node-specific data if given
    virtual bool writeNodeData() const
    { return false; }

    virtual void setNodeData(simulation::Node* /*node*/, double* nodeData, const double* parentData)
    {
        *nodeData = (parentData == NULL) ? 0.0 : *parentData;
    }

    virtual void addNodeData(simulation::Node* /*node*/, double* parentData, const double* nodeData)
    {
        if (parentData)
            *parentData += *nodeData;
    }

    //virtual void execute(core::objectmodel::BaseContext* node, bool doPrefetch) { Visitor::execute(node, doPrefetch); }
    //virtual void execute(core::objectmodel::BaseContext* node) { Visitor::execute(node, true); }

    typedef sofa::core::componentmodel::behavior::BaseMechanicalState::VecId VecId;

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalVisitor"; }

    /**@name Forward processing
    Methods called during the forward (top-down) traversal of the data structure.
    Method processNodeTopDown(simulation::Node*) calls the fwd* methods in the order given here. When there is a mapping, it is processed first, then method fwdMappedMechanicalState is applied to the BaseMechanicalState.
    When there is no mapping, the BaseMechanicalState is processed first using method fwdMechanicalState.
    Then, the other fwd* methods are applied in the given order.
     */
    ///@{

    /// This method calls the fwd* methods during the forward traversal. You typically do not overload it.
    virtual Result processNodeTopDown(simulation::Node* node);

    /// Parallel version of processNodeTopDown.
    /// This method calls the fwd* methods during the forward traversal. You typically do not overload it.
    virtual Result processNodeTopDown(simulation::Node* node, LocalStorage* stack);

    /// Process the OdeSolver
    virtual Result fwdOdeSolver(simulation::Node* /*node*/, core::componentmodel::behavior::OdeSolver* /*solver*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the OdeSolver
    virtual Result fwdOdeSolver(VisitorContext* ctx, core::componentmodel::behavior::OdeSolver* solver)
    {
        return fwdOdeSolver(ctx->node, solver);
    }

    /// Process the ConstraintSolver
    virtual Result fwdConstraintSolver(simulation::Node* /*node*/, core::componentmodel::behavior::ConstraintSolver* /*solver*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the ConstraintSolver
    virtual Result fwdConstraintSolver(VisitorContext* ctx, core::componentmodel::behavior::ConstraintSolver* solver)
    {
        return fwdConstraintSolver(ctx->node, solver);
    }

    /// Process the BaseMechanicalMapping
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMechanicalMapping
    virtual Result fwdMechanicalMapping(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalMapping* map)
    {
        return fwdMechanicalMapping(ctx->node, map);
    }

    /// Process the BaseMechanicalState if it is mapped from the parent level
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMechanicalState if it is mapped from the parent level
    virtual Result fwdMappedMechanicalState(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        return fwdMappedMechanicalState(ctx->node, mm);
    }

    /// Process the BaseMechanicalState if it is not mapped from the parent level
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMechanicalState if it is not mapped from the parent level
    virtual Result fwdMechanicalState(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        return fwdMechanicalState(ctx->node, mm);
    }

    /// Process the BaseMass
    virtual Result fwdMass(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMass* /*mass*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMass
    virtual Result fwdMass(VisitorContext* ctx, core::componentmodel::behavior::BaseMass* mass)
    {
        return fwdMass(ctx->node, mass);
    }

    /// Process all the BaseForceField
    virtual Result fwdForceField(simulation::Node* /*node*/, core::componentmodel::behavior::BaseForceField* /*ff*/)
    {
        return RESULT_CONTINUE;
    }


    /// Process all the BaseForceField
    virtual Result fwdForceField(VisitorContext* ctx, core::componentmodel::behavior::BaseForceField* ff)
    {
        return fwdForceField(ctx->node, ff);
    }

    /// Process all the InteractionForceField
    virtual Result fwdInteractionForceField(simulation::Node* node, core::componentmodel::behavior::InteractionForceField* ff)
    {
        return fwdForceField(node, ff);
    }

    /// Process all the InteractionForceField
    virtual Result fwdInteractionForceField(VisitorContext* ctx, core::componentmodel::behavior::InteractionForceField* ff)
    {
        return fwdInteractionForceField(ctx->node, ff);
    }

    /// Process all the BaseConstraint
    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* /*c*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process all the BaseConstraint
    virtual Result fwdConstraint(VisitorContext* ctx, core::componentmodel::behavior::BaseConstraint* c)
    {
        return fwdConstraint(ctx->node, c);
    }

    /// Process all the BaseLMConstraint
    virtual Result fwdLMConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseLMConstraint* /*c*/)
    {
        return RESULT_CONTINUE;
    }

    virtual Result fwdLMConstraint(VisitorContext* ctx, core::componentmodel::behavior::BaseLMConstraint* c)
    {
        return fwdLMConstraint(ctx->node, c);
    }

    /// Process all the InteractionConstraint
    virtual Result fwdInteractionConstraint(simulation::Node* node, core::componentmodel::behavior::InteractionConstraint* c)
    {
        return fwdConstraint(node, c);
    }

    /// Process all the InteractionConstraint
    virtual Result fwdInteractionConstraint(VisitorContext* ctx, core::componentmodel::behavior::InteractionConstraint* c)
    {
        return fwdInteractionConstraint(ctx->node, c);
    }

    ///@}

    /**@name Backward processing
    Methods called during the backward (bottom-up) traversal of the data structure.
    Method processNodeBottomUp(simulation::Node*) calls the bwd* methods.
    When there is a mapping, method bwdMappedMechanicalState is applied to the BaseMechanicalState.
    When there is no mapping, the BaseMechanicalState is processed using method bwdMechanicalState.
    Finally, the mapping (if any) is processed using method bwdMechanicalMapping.
     */
    ///@{

    /// This method calls the bwd* methods during the backward traversal. You typically do not overload it.
    virtual void processNodeBottomUp(simulation::Node* node);

    /// Parallel version of processNodeBottomUp.
    /// This method calls the bwd* methods during the backward traversal. You typically do not overload it.
    virtual void processNodeBottomUp(simulation::Node* /*node*/, LocalStorage* stack);

    /// Process the BaseMechanicalState when it is not mapped from parent level
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {}

    /// Process the BaseMechanicalState when it is not mapped from parent level
    virtual void bwdMechanicalState(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalState* mm)
    { bwdMechanicalState(ctx->node, mm); }

    /// Process the BaseMechanicalState when it is mapped from parent level
    virtual void bwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {}

    /// Process the BaseMechanicalState when it is mapped from parent level
    virtual void bwdMappedMechanicalState(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalState* mm)
    { bwdMappedMechanicalState(ctx->node, mm); }

    /// Process the BaseMechanicalMapping
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {}

    /// Process the BaseMechanicalMapping
    virtual void bwdMechanicalMapping(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalMapping* map)
    { bwdMechanicalMapping(ctx->node, map); }

    /// Process the OdeSolver
    virtual void bwdOdeSolver(simulation::Node* /*node*/, core::componentmodel::behavior::OdeSolver* /*solver*/)
    {}

    /// Process the OdeSolver
    virtual void bwdOdeSolver(VisitorContext* ctx, core::componentmodel::behavior::OdeSolver* solver)
    { bwdOdeSolver(ctx->node, solver); }

    /// Process the ConstraintSolver
    virtual void bwdConstraintSolver(simulation::Node* /*node*/, core::componentmodel::behavior::ConstraintSolver* /*solver*/)
    {}

    /// Process the ConstraintSolver
    virtual void bwdConstraintSolver(VisitorContext* ctx, core::componentmodel::behavior::ConstraintSolver* solver)
    { bwdConstraintSolver(ctx->node, solver); }

    /// Process all the BaseConstraint
    virtual void bwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* /*c*/)
    {}

    /// Process all the BaseConstraint
    virtual void bwdConstraint(VisitorContext* ctx, core::componentmodel::behavior::BaseConstraint* c)
    { bwdConstraint(ctx->node, c); }

    /// Process all the BaseLMConstraint
    virtual void bwdLMConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseLMConstraint* /*c*/)
    {}

    /// Process all the BaseLMConstraint
    virtual void bwdLMConstraint(VisitorContext* ctx, core::componentmodel::behavior::BaseLMConstraint* c)
    { bwdLMConstraint(ctx->node, c); }

    ///@}


    /// Return a category name for this action.
    /// Only used for debugging / profiling purposes
    virtual const char* getCategoryName() const
    {
        return "animate";
    }

    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map)
    {
        return !map->isMechanical();
    }

#ifdef SOFA_DUMP_VISITOR_INFO
    ctime_t begin(simulation::Node* node, core::objectmodel::BaseObject* obj, const std::string &info=std::string("type"));
    void end(simulation::Node* node, core::objectmodel::BaseObject* obj, ctime_t t0);
#endif

#ifdef SOFA_DUMP_VISITOR_INFO
    virtual void setReadWriteVectors()=0;
    virtual void addReadVector(VecId &id) {readVector.push_back(id);}
    virtual void addWriteVector(VecId &id) {writeVector.push_back(id);}
    virtual void addReadWriteVector(VecId &id) {readVector.push_back(id); writeVector.push_back(id);}
    void printReadVectors(core::componentmodel::behavior::BaseMechanicalState* mm);
    void printReadVectors(simulation::Node* node, core::objectmodel::BaseObject* obj);
    void printWriteVectors(core::componentmodel::behavior::BaseMechanicalState* mm);
    void printWriteVectors(simulation::Node* node, core::objectmodel::BaseObject* obj);
protected:
    sofa::helper::vector< VecId > readVector;
    sofa::helper::vector< VecId > writeVector;
#endif
};

/** Compute the total number of DOFs */
class SOFA_SIMULATION_COMMON_API MechanicalGetDimensionVisitor : public MechanicalVisitor
{
public:
    MechanicalGetDimensionVisitor(double* result)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
        rootData = result;
    }

    virtual Result fwdMechanicalState(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalGetDimensionVisitor";}

    virtual bool writeNodeData() const
    {
        return true;
    }

#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

/** Find the first available index for a VecId
*/
class SOFA_SIMULATION_COMMON_API MechanicalVAvailVisitor : public MechanicalVisitor
{
public:
    VecId& v;
    MechanicalVAvailVisitor(VecId& v) : v(v)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalVAvailVisitor"; }
    virtual std::string getInfos() const { std::string name="[" + v.getName() + "]"; return name;  }
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return false;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(v);
    }
#endif
};

/** Reserve an auxiliary vector identified by a symbolic constant.
*/
class SOFA_SIMULATION_COMMON_API MechanicalVAllocVisitor : public MechanicalVisitor
{
public:
    VecId v;
    MechanicalVAllocVisitor(VecId v) : v(v)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalVAllocVisitor"; }
    virtual std::string getInfos() const {std::string name="[" + v.getName() + "]"; return name;}
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(v);
    }
#endif
};

/** Free an auxiliary vector identified by a symbolic constant */
class SOFA_SIMULATION_COMMON_API MechanicalVFreeVisitor : public MechanicalVisitor
{
public:
    VecId v;
    MechanicalVFreeVisitor(VecId v) : v(v)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalVFreeVisitor"; }
    virtual std::string getInfos() const {std::string name="[" + v.getName() + "]"; return name;}
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

/** Perform a vector operation v=a+b*f
*/
class SOFA_SIMULATION_COMMON_API MechanicalVOpVisitor : public MechanicalVisitor
{
public:
    VecId v;
    VecId a;
    VecId b;
    double f;
    MechanicalVOpVisitor(VecId v, VecId a = VecId::null(), VecId b = VecId::null(), double f=1.0)
        : v(v), a(a), b(b), f(f)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdMechanicalState(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalState* mm);

    virtual const char* getClassName() const { return "MechanicalVOpVisitor";}
    virtual std::string getInfos() const
    {
        std::string info="v=";
        std::string aLabel;
        std::string bLabel;
        std::string fLabel;

        std::ostringstream out;
        out << "f["<<f<<"]";
        fLabel+= out.str();

        if (a != VecId::null())
        {
            info+="a";
            aLabel="a[" + a.getName() + "] ";
            if (b != VecId::null() )
            {
                info += "+b*f";
                bLabel += "b[" + b.getName() + "] ";
            }
        }
        else
        {
            if (b != VecId::null())
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
    //virtual void processNodeBottomUp(simulation::Node* node);

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
    virtual bool readNodeData() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        if (a!=VecId::null()) addReadVector(a);
        if (b!=VecId::null()) addReadVector(b);
        addWriteVector(v);
    }
#endif
};

/** Perform a sequence of linear vector accumulation operation $r_i = sum_j (v_j*f_{ij})
 *
 *  This is used to compute in on steps operations such as $v = v + a*dt, x = x + v*dt$.
 *  Note that if the result vector appears inside the expression, it must be the first operand.
 */
class SOFA_SIMULATION_COMMON_API MechanicalVMultiOpVisitor : public MechanicalVisitor
{
public:
    typedef core::componentmodel::behavior::BaseMechanicalState::VMultiOp VMultiOp;
//     MechanicalVMultiOpVisitor()
//     {}
    MechanicalVMultiOpVisitor(const VMultiOp& o)
        : ops(o)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdMechanicalState(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalState* mm);

    //virtual void processNodeBottomUp(simulation::Node* node);

    virtual const char* getClassName() const { return "MechanicalVMultiOpVisitor"; }
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
    virtual bool readNodeData() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        for (unsigned int i=0; i<ops.size(); ++i)
        {
            addWriteVector(ops[i].first);
            for (unsigned int j=0; j<ops[i].second.size(); ++j)
            {
                addReadVector(ops[i].second[j].first);
            }
        }
    }
#endif
    void setVMultiOp(VMultiOp &o)
    {
        ops = o;
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
protected:
    VMultiOp ops;
};

/** Compute the dot product of two vectors */
class SOFA_SIMULATION_COMMON_API MechanicalVDotVisitor : public MechanicalVisitor
{
public:
    VecId a;
    VecId b;
    MechanicalVDotVisitor(VecId a, VecId b, double* t) : a(a), b(b) //, total(t)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
        rootData = t;
    }

    virtual Result fwdMechanicalState(VisitorContext* ctx, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalVDotVisitor";}
    virtual std::string getInfos() const
    {
        std::string name("v= a*b with a[");
        name += a.getName() + "] and b[" + b.getName() + "]";
        return name;
    }
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
    virtual bool writeNodeData() const
    {
        return true;
    }

#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(a);
        addReadVector(b);
    }
#endif
};

/** Apply a hypothetical displacement.
This action does not modify the state (i.e. positions and velocities) of the objects.
It is typically applied before a MechanicalComputeDfVisitor, in order to compute the df corresponding to a given dx (i.e. apply stiffness).
Dx is propagated to all the layers through the mappings.
 */
class SOFA_SIMULATION_COMMON_API MechanicalPropagateDxVisitor : public MechanicalVisitor
{
public:
    VecId dx;
    bool ignoreMask;
    bool ignoreFlag;
    MechanicalPropagateDxVisitor(VecId dx, bool m, bool f = false) : dx(dx), ignoreMask(m), ignoreFlag(f)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map)
    {
        if (ignoreFlag)
            return false;
        else
            return !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateDxVisitor"; }
    virtual std::string getInfos() const
    {
        std::string name="["+dx.getName()+"]"; return name;
    }
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(dx);
    }
#endif
};

/** V is propagated to all the layers through the mappings.
 */
class SOFA_SIMULATION_COMMON_API MechanicalPropagateVVisitor : public MechanicalVisitor
{
public:
    VecId v;
    bool ignoreMask;

    MechanicalPropagateVVisitor(VecId _v, bool m) : v(_v), ignoreMask(m)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateVVisitor"; }
    virtual std::string getInfos() const
    {
        std::string name="["+v.getName()+"]"; return name;
    }
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(v);
    }
#endif
};


/** Same as MechanicalPropagateDxVisitor followed by MechanicalResetForceVisitor
 */
class SOFA_SIMULATION_COMMON_API MechanicalPropagateDxAndResetForceVisitor : public MechanicalVisitor
{
public:
    VecId dx,f;
    bool ignoreMask;

    MechanicalPropagateDxAndResetForceVisitor(VecId dx, VecId f, bool m) : dx(dx), f(f), ignoreMask(m)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateDxAndResetForceVisitor";}
    virtual std::string getInfos() const { std::string name= "dx["+dx.getName()+"] f["+f.getName()+"]"; return name;}

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(dx);
        addReadVector(f);
    }
#endif
};


class SOFA_SIMULATION_COMMON_API MechanicalPropagateXVisitor : public MechanicalVisitor
{
public:
    VecId x;
    bool ignoreMask;

    MechanicalPropagateXVisitor(VecId x, bool m) : x(x), ignoreMask(m)
    {}
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateXVisitor"; }
    virtual std::string getInfos() const { std::string name= "X["+x.getName()+"]"; return name;}


    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(x);
    }
#endif
};


/** Same as MechanicalPropagateXVisitor followed by MechanicalResetForceVisitor
 */
class SOFA_SIMULATION_COMMON_API MechanicalPropagateXAndResetForceVisitor : public MechanicalVisitor
{
public:
    VecId x,f;
    bool ignoreMask;

    MechanicalPropagateXAndResetForceVisitor(VecId x, VecId f, bool m) : x(x), f(f), ignoreMask(m)
    {}
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateXAndResetForceVisitor"; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(x);
        addReadWriteVector(f);
    }
#endif
};



class SOFA_SIMULATION_COMMON_API MechanicalPropagateAndAddDxVisitor : public MechanicalVisitor
{
public:
    VecId dx, v;
    bool ignoreMask;

    MechanicalPropagateAndAddDxVisitor(VecId dx = VecId::dx(), VecId v =VecId::velocity(), bool m=true) : dx(dx) , v(v),ignoreMask(m)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateAndAddDxVisitor"; }
    virtual std::string getInfos() const { std::string name= "["+dx.getName()+"]"; return name; }


    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(v);
        addReadWriteVector(dx);
    }
#endif
};


/** Accumulate the product of the mass matrix by a given vector.
Typically used in implicit integration solved by a Conjugate Gradient algorithm.
Note that if a dx vector is given, it is used and propagated by the mappings, Otherwise the current value is used.
*/
class SOFA_SIMULATION_COMMON_API MechanicalAddMDxVisitor : public MechanicalVisitor
{
public:
    VecId res;
    VecId dx;
    double factor;
    MechanicalAddMDxVisitor(VecId res, VecId dx=VecId(), double factor = 1.0)
        : res(res), dx(dx), factor(factor)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMass(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMass* mass);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAddMDxVisitor"; }
    virtual std::string getInfos() const { std::string name="dx["+dx.getName()+"] in res[" + res.getName()+"]"; return name; }

#ifdef SOFA_SUPPORT_MAPPED_MASS
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
#else
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/);
#endif
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(res);

#ifdef SOFA_SUPPORT_MAPPED_MASS
        if (dx != VecId::null()) addReadWriteVector(dx);
        else addReadVector(dx);
#else
        addReadVector(dx);
#endif
    }
#endif
};

/** Compute accelerations generated by given forces
 */
class SOFA_SIMULATION_COMMON_API MechanicalAccFromFVisitor : public MechanicalVisitor
{
public:
    VecId a;
    VecId f;
    MechanicalAccFromFVisitor(VecId a, VecId f) : a(a), f(f)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMass(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMass* mass);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAccFromFVisitor"; }
    virtual std::string getInfos() const { std::string name="a["+a.getName()+"] f["+f.getName()+"]"; return name; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(a);
        addReadVector(f);
    }
#endif
};

/** Propagate positions  to all the levels of the hierarchy.
At each level, the mappings form the parent to the child is applied.
 */
class SOFA_SIMULATION_COMMON_API MechanicalPropagatePositionVisitor : public MechanicalVisitor
{
public:
    double t;
    VecId x;
    bool ignoreMask;

    MechanicalPropagatePositionVisitor(double time=0, VecId x = VecId::position(), bool m=true);

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagatePositionVisitor";}
    virtual std::string getInfos() const
    {
        std::string name="x["+x.getName()+"]";
        if (ignoreMask) name += " Mask DISABLED";
        else            name += " Mask ENABLED";
        return name;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(x);
    }
#endif
};
/** Propagate positions and velocities to all the levels of the hierarchy.
At each level, the mappings form the parent to the child is applied.
After the execution of this action, all the (mapped) degrees of freedom are consistent with the independent degrees of freedom.
This action is typically applied after time integration of the independent degrees of freedom.
 */
class SOFA_SIMULATION_COMMON_API MechanicalPropagatePositionAndVelocityVisitor : public MechanicalVisitor
{
public:
    double t;
    VecId x;
    VecId v;

#ifdef SOFA_SUPPORT_MAPPED_MASS
    // compute the acceleration created by the input velocity and the derivative of the mapping
    VecId a;
    MechanicalPropagatePositionAndVelocityVisitor(double time=0, VecId x = VecId::position(), VecId v = VecId::velocity(), VecId a = VecId::dx(), bool m=true); //
#else
    MechanicalPropagatePositionAndVelocityVisitor(double time=0, VecId x = VecId::position(), VecId v = VecId::velocity(), bool m=true);
#endif
    bool ignoreMask;

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagatePositionAndVelocityVisitor";}
    virtual std::string getInfos() const { std::string name="x["+x.getName()+"] v["+v.getName()+"]"; return name; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(x);
        addReadVector(v);
    }
#endif
};


/**
 * @brief Visitor class used to set positions and velocities of the top level MechanicalStates of the hierarchy.
 */
class SOFA_SIMULATION_COMMON_API MechanicalSetPositionAndVelocityVisitor : public MechanicalVisitor
{
public:
    double t;
    VecId x;
    VecId v;

#ifdef SOFA_SUPPORT_MAPPED_MASS
    // compute the acceleration created by the input velocity and the derivative of the mapping
    VecId a;
    MechanicalSetPositionAndVelocityVisitor(double time=0, VecId x = VecId::position(), VecId v = VecId::velocity(), VecId a = VecId::dx()); //
#else
    MechanicalSetPositionAndVelocityVisitor(double time=0, VecId x = VecId::position(), VecId v = VecId::velocity());
#endif

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagatePositionAndVelocityVisitor";}
    virtual std::string getInfos() const { std::string name="x["+x.getName()+"] v["+v.getName()+"]"; return name; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(x);
        addReadVector(v);
    }
#endif
};


/** Propagate free positions to all the levels of the hierarchy.
At each level, the mappings form the parent to the child is applied.
After the execution of this action, all the (mapped) degrees of freedom are consistent with the independent degrees of freedom.
This action is typically applied after time integration of the independent degrees of freedom.
 */
class SOFA_SIMULATION_COMMON_API MechanicalPropagateFreePositionVisitor : public MechanicalVisitor
{
public:
    double t;
    VecId x;
    VecId v;
    bool ignoreMask;
    MechanicalPropagateFreePositionVisitor(double time=0, VecId x = VecId::freePosition(), VecId v = VecId::freeVelocity(), bool m=true): t(time), x(x), v(v), ignoreMask(m)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateFreePositionVisitor";}
    virtual std::string getInfos() const { std::string name="x["+x.getName()+"] v["+v.getName()+"]"; return name; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadVector(x);
        addReadVector(v);
    }
#endif
};


/** Reset the force in all the MechanicalModel
This action is typically applied before accumulating all the forces.
 */
class SOFA_SIMULATION_COMMON_API MechanicalResetForceVisitor : public MechanicalVisitor
{
public:
    VecId res;
    bool onlyMapped;

    MechanicalResetForceVisitor(VecId res, bool onlyMapped = false) : res(res), onlyMapped(onlyMapped)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const {  return "MechanicalResetForceVisitor";}
    virtual std::string getInfos() const
    {
        std::string name="["+res.getName()+"]";
        if (onlyMapped) name+= " Only Mapped";
        return name;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(res);
    }
#endif
};

/** Accumulate the forces (internal and interactions).
This action is typically called after a MechanicalResetForceVisitor.
 */
class SOFA_SIMULATION_COMMON_API MechanicalComputeForceVisitor : public MechanicalVisitor
{
public:
    VecId res;
    bool accumulate; ///< Accumulate everything back to the DOFs through the mappings

    MechanicalComputeForceVisitor(VecId res, bool accumulate = true) : res(res), accumulate(accumulate)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdForceField(simulation::Node* /*node*/, core::componentmodel::behavior::BaseForceField* ff);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);


    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const {return "MechanicalComputeForceVisitor";}
    virtual std::string getInfos() const
    {
        std::string name=std::string("[")+res.getName()+std::string("]");
        if (accumulate) name+= " Accumulating";
        else            name+= " Not Accumulating";
        return name;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(res);
    }
#endif
};

/** Compute the variation of force corresponding to a variation of position.
This action is typically called after a MechanicalPropagateDxVisitor.
 */
class SOFA_SIMULATION_COMMON_API MechanicalComputeDfVisitor : public MechanicalVisitor
{
public:
    VecId res;
    bool useV;
    bool accumulate; ///< Accumulate everything back to the DOFs through the mappings
    MechanicalComputeDfVisitor(VecId res, bool useV=false, bool accumulate=true) : res(res), useV(useV), accumulate(accumulate)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdForceField(simulation::Node* /*node*/, core::componentmodel::behavior::BaseForceField* ff);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const {return "MechanicalComputeDfVisitor";}
    virtual std::string getInfos() const
    {
        std::string name="["+res.getName()+"]";
        if (useV) name+= " Using V";
        else      name+= " Not Using V";
        if (accumulate) name+= " Accumulating";
        else            name+= " Not Accumulating";
        return name;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(res);
    }
#endif
};


/** Accumulate the product of the system matrix by a given vector.
Typically used in implicit integration solved by a Conjugate Gradient algorithm.
The current value of the dx vector is used.
This action is typically called after a MechanicalPropagateDxAndResetForceVisitor.
*/
class SOFA_SIMULATION_COMMON_API MechanicalAddMBKdxVisitor : public MechanicalVisitor
{
public:
    VecId res;
    double mFactor;
    double bFactor;
    double kFactor;
    bool useV;
    bool accumulate; ///< Accumulate everything back to the DOFs through the mappings
    MechanicalAddMBKdxVisitor(VecId res, double mFactor, double bFactor, double kFactor, bool useV=false, bool accumulate = true)
        : res(res), mFactor(mFactor), bFactor(bFactor), kFactor(kFactor), useV(useV), accumulate(accumulate)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdForceField(simulation::Node* /*node*/, core::componentmodel::behavior::BaseForceField* ff);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAddMBKdxVisitor"; }
    virtual std::string getInfos() const { std::string name= "["+res.getName()+"]"; return name; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(res);
    }
#endif
};

class SOFA_SIMULATION_COMMON_API MechanicalResetConstraintVisitor : public MechanicalVisitor
{
public:
    //VecId res;
    MechanicalResetConstraintVisitor(/*VecId res*/) //: res(res)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdLMConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseLMConstraint* c);

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalResetConstraintVisitor"; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

#ifdef SOFA_HAVE_EIGEN2

class SOFA_SIMULATION_COMMON_API MechanicalExpressJacobianVisitor: public MechanicalVisitor
{
public:
    MechanicalExpressJacobianVisitor(simulation::Node* n);
    virtual void bwdMechanicalMapping(simulation::Node* node, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdLMConstraint(simulation::Node* node, core::componentmodel::behavior::BaseLMConstraint* c);

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }
    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalExpressJacobianVisitor"; }
    virtual bool isThreadSafe() const { return false;}
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
protected:
    unsigned int constraintId;
};



class SOFA_SIMULATION_COMMON_API MechanicalSolveLMConstraintVisitor: public MechanicalVisitor
{
public:
    MechanicalSolveLMConstraintVisitor(VecId v,bool priorStatePropagation,bool updateVelocity=true):state(v), propagateState(priorStatePropagation),isPositionChangeUpdateVelocity(updateVelocity)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    };


    virtual Result fwdConstraintSolver(simulation::Node* /*node*/, core::componentmodel::behavior::ConstraintSolver* s);
    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalSolveLMConstraintVisitor"; }
    virtual std::string getInfos() const { std::string name= "["+state.getName()+"]"; return name; }

    virtual bool isThreadSafe() const
    {
        return false;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(state);
    }
#endif
    VecId state;
    bool propagateState;
    bool isPositionChangeUpdateVelocity;
};

class SOFA_SIMULATION_COMMON_API MechanicalWriteLMConstraint : public MechanicalVisitor
{
public:
    MechanicalWriteLMConstraint()
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    };

    virtual Result fwdLMConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseLMConstraint* c);
    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalWriteLMConstraint"; }
    virtual std::string getInfos() const
    {
        std::string name;
        if      (order == core::componentmodel::behavior::BaseLMConstraint::ACC)
            name= "["+VecId::dx().getName()+"]";
        else if (order == core::componentmodel::behavior::BaseLMConstraint::VEL)
            name= "["+VecId::velocity().getName()+"]";
        else if (order == core::componentmodel::behavior::BaseLMConstraint::POS)
            name= "["+VecId::position().getName()+"]";
        return name;
    }


    virtual void clear() {datasC.clear();}
    virtual const std::vector< core::componentmodel::behavior::BaseLMConstraint *> &getConstraints() const {return datasC;}
    virtual unsigned int numConstraint() {return datasC.size();}

    virtual void setOrder(core::componentmodel::behavior::BaseLMConstraint::ConstOrder i) {order=i;}
    core::componentmodel::behavior::BaseLMConstraint::ConstOrder getOrder() const { return order; }

    virtual bool isThreadSafe() const
    {
        return false;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif

protected:
    core::componentmodel::behavior::BaseLMConstraint::ConstOrder order;
    std::vector< core::componentmodel::behavior::BaseLMConstraint *> datasC;

};

#endif

class SOFA_SIMULATION_COMMON_API MechanicalAccumulateConstraint : public MechanicalVisitor
{
public:
    MechanicalAccumulateConstraint(unsigned int &_contactId)
        :contactId(_contactId)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c);
    virtual Result fwdLMConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseLMConstraint* c);

    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }


    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAccumulateConstraint"; }

    virtual bool isThreadSafe() const
    {
        return false;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif

protected:
    unsigned int &contactId;
};

class SOFA_SIMULATION_COMMON_API MechanicalRenumberConstraint : public MechanicalVisitor
{
public:
    MechanicalRenumberConstraint(const sofa::helper::vector<unsigned> &renumbering)
        : renumbering(renumbering)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->renumberConstraintId(renumbering);
        return RESULT_PRUNE;
    }
    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }


    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalRenumberConstraint"; }

    virtual bool isThreadSafe() const
    {
        return false;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif

protected:
    const sofa::helper::vector<unsigned> &renumbering;
};

/** Apply the constraints as filters to the given vector.
This works for simple independent constraints, like maintaining a fixed point.
*/
class SOFA_SIMULATION_COMMON_API MechanicalApplyConstraintsVisitor : public MechanicalVisitor
{
public:
    VecId res;
    double **W;
    MechanicalApplyConstraintsVisitor(VecId res, double **W = NULL) : res(res), W(W)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/);
    virtual void bwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c);
    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalApplyConstraintsVisitor"; }
    virtual std::string getInfos() const { std::string name= "["+res.getName()+"]"; return name; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(res);
    }
#endif
};

/** Visitor used to prepare a time integration step. Typically, does nothing.
*/
class SOFA_SIMULATION_COMMON_API MechanicalBeginIntegrationVisitor : public MechanicalVisitor
{
public:
    double dt;
    MechanicalBeginIntegrationVisitor (double dt)
        : dt(dt)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalBeginIntegrationVisitor"; }

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

/** Visitor applied after a time step has been applied. Does typically nothing.
*/
class SOFA_SIMULATION_COMMON_API MechanicalEndIntegrationVisitor : public MechanicalVisitor
{
public:
    double dt;
    MechanicalEndIntegrationVisitor (double dt)
        : dt(dt)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalEndIntegrationVisitor"; }

    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

/** Visitor used to do a time integration step using OdeSolvers
*/
class SOFA_SIMULATION_COMMON_API MechanicalIntegrationVisitor : public MechanicalVisitor
{
public:
    double dt;
    MechanicalIntegrationVisitor (double dt)
        : dt(dt)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdOdeSolver(simulation::Node* node, core::componentmodel::behavior::OdeSolver* obj);
    virtual Result fwdInteractionForceField(simulation::Node*, core::componentmodel::behavior::InteractionForceField* obj);
    virtual void bwdOdeSolver(simulation::Node* /*node*/, core::componentmodel::behavior::OdeSolver* /*obj*/)
    {
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalIntegrationVisitor"; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};



// ACTION : Compute Compliance on mechanical models
class SOFA_SIMULATION_COMMON_API MechanicalComputeComplianceVisitor : public MechanicalVisitor
{
public:
    MechanicalComputeComplianceVisitor( double **W):_W(W)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* ms);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* ms);
    // This visitor must go through all mechanical mappings, even if isMechanical flag is disabled
    virtual bool stopAtMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return false; // !map->isMechanical();
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalComputeComplianceVisitor"; }

#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
private:
    double **_W;
};


/** Accumulate only the contact forces computed in applyContactForce.
This action is typically called after a MechanicalResetForceVisitor.
 */
class SOFA_SIMULATION_COMMON_API MechanicalComputeContactForceVisitor : public MechanicalVisitor
{
public:
    VecId res;
    MechanicalComputeContactForceVisitor(VecId res) : res(res)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { std::string name= "MechanicalComputeContactForceVisitor["+res.getName()+"]"; return name.c_str(); }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(res);
    }
#endif
};

/** Add dt*mass*Gravity to the velocity
	This is called if the mass wants to be added separately to the mm from the other forces
 */
class SOFA_SIMULATION_COMMON_API MechanicalAddSeparateGravityVisitor : public MechanicalVisitor
{
public:

    double dt;
    VecId res;
    MechanicalAddSeparateGravityVisitor(double dt, VecId res) : dt(dt), res(res)
    {
#ifdef SOFA_DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    /// Process the BaseMass
    virtual Result fwdMass(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMass* mass);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAddSeparateGravityVisitor"; }
    virtual std::string getInfos() const { std::string name= "["+res.getName()+"]"; return name; }
#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(res);
    }
#endif
};

/** Find mechanical particles hit by the given ray.
 *
 *  A mechanical particle is defined as a 2D or 3D, position or rigid DOF
 *  which is linked to the free mechanical DOFs by mechanical mappings
 */
class SOFA_SIMULATION_COMMON_API MechanicalPickParticlesVisitor : public MechanicalVisitor
{
public:
    defaulttype::Vec3d rayOrigin, rayDirection;
    double radius0, dRadius;
    std::multimap< double, std::pair<sofa::core::componentmodel::behavior::BaseMechanicalState*, int> > particles;
    MechanicalPickParticlesVisitor(const defaulttype::Vec3d& origin, const defaulttype::Vec3d& direction, double r0=0.001, double dr=0.0)
        : rayOrigin(origin), rayDirection(direction), radius0(r0), dRadius(dr)
    {
    }

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPickParticles"; }

#ifdef SOFA_DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

} // namespace simulation

} // namespace sofa

#endif
