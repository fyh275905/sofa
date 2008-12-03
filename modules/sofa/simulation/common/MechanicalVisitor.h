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
#include <iostream>
using std::cerr;
using std::endl;

namespace sofa
{

namespace simulation
{


using namespace sofa::defaulttype;
/** Base class for easily creating new actions for mechanical simulation.

During the first traversal (top-down), method processNodeTopDown(simulation::Node*) is applied to each simulation::Node. Each component attached to this node is processed using the appropriate method, prefixed by fwd.

During the second traversal (bottom-up), method processNodeBottomUp(simulation::Node*) is applied to each simulation::Node. Each component attached to this node is processed using the appropriate method, prefixed by bwd.

The default behavior of the fwd* and bwd* is to do nothing. Derived actions typically overload these methods to implement the desired processing.

*/
class MechanicalVisitor : public Visitor
{
public:
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

    /// Process the OdeSolver
    virtual Result fwdOdeSolver(simulation::Node* /*node*/, core::componentmodel::behavior::OdeSolver* /*solver*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMechanicalMapping
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMechanicalState if it is mapped from the parent level
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMechanicalState if it is not mapped from the parent level
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMass
    virtual Result fwdMass(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMass* /*mass*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process all the BaseForceField
    virtual Result fwdForceField(simulation::Node* /*node*/, core::componentmodel::behavior::BaseForceField* /*ff*/)
    {
        return RESULT_CONTINUE;
    }


    /// Process all the InteractionForceField
    virtual Result fwdInteractionForceField(simulation::Node* node, core::componentmodel::behavior::InteractionForceField* ff)
    {
        return fwdForceField(node, ff);
    }

    /// Process all the BaseConstraint
    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* /*c*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process all the BaseLMConstraint
    virtual Result fwdLMConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseLMConstraint* /*c*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process all the InteractionConstraint
    virtual Result fwdInteractionConstraint(simulation::Node* node, core::componentmodel::behavior::InteractionConstraint* c)
    {
        return fwdConstraint(node, c);
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

    /// Process the BaseMechanicalState when it is not mapped from parent level
    virtual void bwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {}

    /// Process the BaseMechanicalState when it is mapped from parent level
    virtual void bwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {}

    /// Process the BaseMechanicalMapping
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {}

    /// Process the OdeSolver
    virtual void bwdOdeSolver(simulation::Node* /*node*/, core::componentmodel::behavior::OdeSolver* /*solver*/)
    {}


    /// Process all the BaseConstraint
    virtual void bwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* /*c*/)
    {}

    ///@}


    /// Return a category name for this action.
    /// Only used for debugging / profiling purposes
    virtual const char* getCategoryName() const
    {
        return "animate";
    }

    ctime_t beginProcess(simulation::Node* node, core::objectmodel::BaseObject* obj);
    void endProcess(simulation::Node* node, core::objectmodel::BaseObject* obj, ctime_t t0);

#ifdef DUMP_VISITOR_INFO
    virtual void setReadWriteVectors()=0;
    virtual void addReadVector(VecId &id) {readVector.push_back(id);}
    virtual void addWriteVector(VecId &id) {writeVector.push_back(id);}
    virtual void addReadWriteVector(VecId &id) {readVector.push_back(id); writeVector.push_back(id);}
    void printReadVectors(core::componentmodel::behavior::BaseMechanicalState* mm, std::string &info);
    void printReadVectors(simulation::Node* node, core::objectmodel::BaseObject* obj);
    void printWriteVectors(core::componentmodel::behavior::BaseMechanicalState* mm, std::string &info);
    void printWriteVectors(simulation::Node* node, core::objectmodel::BaseObject* obj);
protected:
    sofa::helper::vector< VecId > readVector;
    sofa::helper::vector< VecId > writeVector;
#endif
};

/** Find the first available index for a VecId
*/
class MechanicalVAvailVisitor : public MechanicalVisitor
{
public:
    VecId& v;
    MechanicalVAvailVisitor(VecId& v) : v(v)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalVAvailVisitor"; }
    virtual const char* getInfos() const { std::string name="[" + v.getName() + "]"; return name.c_str();  }
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return false;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(v);
    }
#endif
};

/** Reserve an auxiliary vector identified by a symbolic constant.
*/
class MechanicalVAllocVisitor : public MechanicalVisitor
{
public:
    VecId v;
    MechanicalVAllocVisitor(VecId v) : v(v)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalVAllocVisitor"; }
    virtual const char* getInfos() const {std::string name="[" + v.getName() + "]"; return name.c_str();}
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(v);
    }
#endif
};

/** Free an auxiliary vector identified by a symbolic constant */
class MechanicalVFreeVisitor : public MechanicalVisitor
{
public:
    VecId v;
    MechanicalVFreeVisitor(VecId v) : v(v)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalVFreeVisitor"; }
    virtual const char* getInfos() const {std::string name="[" + v.getName() + "]"; return name.c_str();}
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

/** Perform a vector operation v=a+b*f
*/
class MechanicalVOpVisitor : public MechanicalVisitor
{
public:
    VecId v;
    VecId a;
    VecId b;
    double f;
    MechanicalVOpVisitor(VecId v, VecId a = VecId::null(), VecId b = VecId::null(), double f=1.0)
        : v(v), a(a), b(b), f(f)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/);

    virtual const char* getClassName() const { return "MechanicalVOpVisitor";}
    virtual const char* getInfos() const
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
        return info.c_str();
    }
    //virtual void processNodeBottomUp(simulation::Node* node);

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
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
class MechanicalVMultiOpVisitor : public MechanicalVisitor
{
public:
    typedef core::componentmodel::behavior::BaseMechanicalState::VMultiOp VMultiOp;
//     MechanicalVMultiOpVisitor()
//     {}
    MechanicalVMultiOpVisitor(const VMultiOp& o)
        : ops(o)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/);

    //virtual void processNodeBottomUp(simulation::Node* node);

    virtual const char* getClassName() const { return "MechanicalVMultiOpVisitor"; }
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
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
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
protected:
    VMultiOp ops;
};

/** Compute the dot product of two vectors */
class MechanicalVDotVisitor : public MechanicalVisitor
{
public:
    VecId a;
    VecId b;
    double* total;
    MechanicalVDotVisitor(VecId a, VecId b, double* t) : a(a), b(b), total(t)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    /// Sequential code
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalVDotVisitor";}
    virtual const char* getInfos() const
    {
        std::string name="v=a*b with a[" + a.getName() + "] and b[" + b.getName() + "]";
        return name.c_str();
    }
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
    /// Parallel code
    virtual Result processNodeTopDown(simulation::Node* node, LocalStorage* stack);

    /// Parallel code
    virtual void processNodeBottomUp(simulation::Node* /*node*/, LocalStorage* stack);

#ifdef DUMP_VISITOR_INFO
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
class MechanicalPropagateDxVisitor : public MechanicalVisitor
{
public:
    VecId dx;
    MechanicalPropagateDxVisitor(VecId dx) : dx(dx)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateDxVisitor"; }
    virtual const char* getInfos() const
    {
        std::string name="["+dx.getName()+"]"; return name.c_str();
    }
    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(dx);
    }
#endif
};


/** Same as MechanicalPropagateDxVisitor followed by MechanicalResetForceVisitor
 */
class MechanicalPropagateDxAndResetForceVisitor : public MechanicalVisitor
{
public:
    VecId dx,f;
    MechanicalPropagateDxAndResetForceVisitor(VecId dx, VecId f) : dx(dx), f(f)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateDxAndResetForceVisitor";}
    virtual const char* getInfos() const { std::string name= "dx["+dx.getName()+"] f["+f.getName()+"]"; return name.c_str();}

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(dx);
        addWriteVector(f);
    }
#endif
};


class MechanicalPropagateAndAddDxVisitor : public MechanicalVisitor
{
public:
    VecId dx, v;
    MechanicalPropagateAndAddDxVisitor(VecId dx = VecId::dx(), VecId v =VecId::velocity()) : dx(dx) , v(v)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateAndAddDxVisitor"; }
    virtual const char* getInfos() const { std::string name= "["+dx.getName()+"]"; return name.c_str(); }


    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(dx);
    }
#endif
};


/** Accumulate the product of the mass matrix by a given vector.
Typically used in implicit integration solved by a Conjugate Gradient algorithm.
Note that if a dx vector is given, it is used and propagated by the mappings, Otherwise the current value is used.
*/
class MechanicalAddMDxVisitor : public MechanicalVisitor
{
public:
    VecId res;
    VecId dx;
    double factor;
    MechanicalAddMDxVisitor(VecId res, VecId dx=VecId(), double factor = 1.0)
        : res(res), dx(dx), factor(factor)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMass(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMass* mass);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAddMDxVisitor"; }
    virtual const char* getInfos() const { std::string name="dx["+dx.getName()+"] in res[" + res.getName()+"]"; return name.c_str(); }

#ifdef SOFA_SUPPORT_MAPPED_MASS
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
#else
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/);
#endif

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
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
class MechanicalAccFromFVisitor : public MechanicalVisitor
{
public:
    VecId a;
    VecId f;
    MechanicalAccFromFVisitor(VecId a, VecId f) : a(a), f(f)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMass(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMass* mass);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAccFromFVisitor"; }
    virtual const char* getInfos() const { std::string name="a["+a.getName()+"] f["+f.getName()+"]"; return name.c_str(); }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
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
class MechanicalPropagatePositionVisitor : public MechanicalVisitor
{
public:
    double t;
    VecId x;
    MechanicalPropagatePositionVisitor(double time=0, VecId x = VecId::position());

    virtual Result processNodeTopDown(simulation::Node* node);
    virtual void processNodeBottomUp(simulation::Node* node);

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c);


    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagatePositionVisitor";}
    virtual const char* getInfos() const { std::string name="x["+x.getName()+"]"; return name.c_str(); }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(x);
    }
#endif
};
/** Propagate positions and velocities to all the levels of the hierarchy.
At each level, the mappings form the parent to the child is applied.
After the execution of this action, all the (mapped) degrees of freedom are consistent with the independent degrees of freedom.
This action is typically applied after time integration of the independent degrees of freedom.
 */
class MechanicalPropagatePositionAndVelocityVisitor : public MechanicalVisitor
{
public:
    double t;
    VecId x;
    VecId v;
#ifdef SOFA_SUPPORT_MAPPED_MASS
    // compute the acceleration created by the input velocity and the derivative of the mapping
    VecId a;
    MechanicalPropagatePositionAndVelocityVisitor(double time=0, VecId x = VecId::position(), VecId v = VecId::velocity(), VecId a = VecId::dx()); //
#else
    MechanicalPropagatePositionAndVelocityVisitor(double time=0, VecId x = VecId::position(), VecId v = VecId::velocity());
#endif
    virtual Result processNodeTopDown(simulation::Node* node);
    virtual void processNodeBottomUp(simulation::Node* node);

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c);


    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagatePositionAndVelocityVisitor";}
    virtual const char* getInfos() const { std::string name="x["+x.getName()+"] v["+v.getName()+"]"; return name.c_str(); }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(x);
        addWriteVector(v);
    }
#endif
};


/** Propagate free positions to all the levels of the hierarchy.
At each level, the mappings form the parent to the child is applied.
After the execution of this action, all the (mapped) degrees of freedom are consistent with the independent degrees of freedom.
This action is typically applied after time integration of the independent degrees of freedom.
 */
class MechanicalPropagateFreePositionVisitor : public MechanicalVisitor
{
public:
    double t;
    VecId x;
    VecId v;
    MechanicalPropagateFreePositionVisitor(double time=0, VecId x = VecId::freePosition(), VecId v = VecId::freeVelocity()): t(time), x(x), v(v)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result processNodeTopDown(simulation::Node* node);
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);
    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalPropagateFreePositionVisitor";}
    virtual const char* getInfos() const { std::string name="x["+x.getName()+"] v["+v.getName()+"]"; return name.c_str(); }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(x);
        addWriteVector(v);
    }
#endif
};


/** Reset the force in all the MechanicalModel
This action is typically applied before accumulating all the forces.
 */
class MechanicalResetForceVisitor : public MechanicalVisitor
{
public:
    VecId res;
    bool onlyMapped;
    MechanicalResetForceVisitor(VecId res, bool onlyMapped = false) : res(res), onlyMapped(onlyMapped)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const {  return "MechanicalResetForceVisitor";}
    virtual const char* getInfos() const
    {
        std::string name="["+res.getName()+"]";
        if (onlyMapped) name+= " Only Mapped";
        return name.c_str();
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(res);
    }
#endif
};

/** Accumulate the forces (internal and interactions).
This action is typically called after a MechanicalResetForceVisitor.
 */
class MechanicalComputeForceVisitor : public MechanicalVisitor
{
public:
    VecId res;
    bool accumulate; ///< Accumulate everything back to the DOFs through the mappings
    MechanicalComputeForceVisitor(VecId res, bool accumulate = true) : res(res), accumulate(accumulate)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdForceField(simulation::Node* /*node*/, core::componentmodel::behavior::BaseForceField* ff);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);


    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const {return "MechanicalComputeForceVisitor";}
    virtual const char* getInfos() const
    {
        std::string name="["+res.getName()+"]";
        if (accumulate) name+= " Accumulating";
        else            name+= " Not Accumulating";
        return name.c_str();
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(res);
    }
#endif
};

/** Compute the variation of force corresponding to a variation of position.
This action is typically called after a MechanicalPropagateDxVisitor.
 */
class MechanicalComputeDfVisitor : public MechanicalVisitor
{
public:
    VecId res;
    bool useV;
    bool accumulate; ///< Accumulate everything back to the DOFs through the mappings
    MechanicalComputeDfVisitor(VecId res, bool useV=false, bool accumulate=true) : res(res), useV(useV), accumulate(accumulate)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdForceField(simulation::Node* /*node*/, core::componentmodel::behavior::BaseForceField* ff);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const {return "MechanicalComputeDfVisitor";}
    virtual const char* getInfos() const
    {
        std::string name="["+res.getName()+"]";
        if (useV) name+= " Using V";
        else      name+= " Not Using V";
        if (accumulate) name+= " Accumulating";
        else            name+= " Not Accumulating";
        return name.c_str();
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
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
class MechanicalAddMBKdxVisitor : public MechanicalVisitor
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
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdForceField(simulation::Node* /*node*/, core::componentmodel::behavior::BaseForceField* ff);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAddMBKdxVisitor"; }
    virtual const char* getInfos() const { std::string name= "["+res.getName()+"]"; return name.c_str(); }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addWriteVector(res);
    }
#endif
};

class MechanicalResetConstraintVisitor : public MechanicalVisitor
{
public:
    //VecId res;
    MechanicalResetConstraintVisitor(/*VecId res*/) //: res(res)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);


    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalResetConstraintVisitor"; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

class MechanicalAccumulateLMConstraint : public MechanicalVisitor
{
public:
    struct ConstraintData
    {
        core::componentmodel::behavior::BaseLMConstraint *data;
        //independent dofs associated
        core::componentmodel::behavior::BaseMechanicalState *independentMState[2]; //independentMechanicalState
    };


    MechanicalAccumulateLMConstraint()
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    };

    virtual Result fwdLMConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseLMConstraint* c);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);


    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAccumulateConstraint"; }


    virtual void clear() {datasC.clear();}
    virtual ConstraintData &getConstraint(unsigned int i) {return datasC[i];}
    virtual unsigned int numConstraintDatas() {return datasC.size();}


    virtual bool isThreadSafe() const
    {
        return false;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif

protected:
    std::vector< ConstraintData > datasC;
};

class MechanicalAccumulateConstraint : public MechanicalVisitor
{
public:
    MechanicalAccumulateConstraint(unsigned int &_contactId)
        :contactId(_contactId)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    virtual Result fwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c);
    virtual void bwdMechanicalMapping(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map);


    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAccumulateConstraint"; }

    virtual bool isThreadSafe() const
    {
        return false;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif

protected:
    unsigned int &contactId;
};

/** Apply the constraints as filters to the given vector.
This works for simple independent constraints, like maintaining a fixed point.
*/
class MechanicalApplyConstraintsVisitor : public MechanicalVisitor
{
public:
    VecId res;
    double **W;
    MechanicalApplyConstraintsVisitor(VecId res, double **W = NULL) : res(res), W(W)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/);
    virtual void bwdConstraint(simulation::Node* /*node*/, core::componentmodel::behavior::BaseConstraint* c);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalApplyConstraintsVisitor"; }
    virtual const char* getInfos() const { std::string name= "["+res.getName()+"]"; return name.c_str(); }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(res);
    }
#endif
};

/** Visitor used to prepare a time integration step. Typically, does nothing.
*/
class MechanicalBeginIntegrationVisitor : public MechanicalVisitor
{
public:
    double dt;
    MechanicalBeginIntegrationVisitor (double dt)
        : dt(dt)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalBeginIntegrationVisitor"; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

/** Visitor applied after a time step has been applied. Does typically nothing.
*/
class MechanicalEndIntegrationVisitor : public MechanicalVisitor
{
public:
    double dt;
    MechanicalEndIntegrationVisitor (double dt)
        : dt(dt)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalEndIntegrationVisitor"; }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};

/** Visitor used to do a time integration step using OdeSolvers
*/
class MechanicalIntegrationVisitor : public MechanicalVisitor
{
public:
    double dt;
    MechanicalIntegrationVisitor (double dt)
        : dt(dt)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdOdeSolver(simulation::Node* node, core::componentmodel::behavior::OdeSolver* obj);
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
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
    }
#endif
};



// ACTION : Compute Compliance on mechanical models
class MechanicalComputeComplianceVisitor : public MechanicalVisitor
{
public:
    MechanicalComputeComplianceVisitor( double **W):_W(W)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }
    virtual Result fwdMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* ms);
    virtual Result fwdMappedMechanicalState(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMechanicalState* ms);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalComputeComplianceVisitor"; }

#ifdef DUMP_VISITOR_INFO
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
class MechanicalComputeContactForceVisitor : public MechanicalVisitor
{
public:
    VecId res;
    MechanicalComputeContactForceVisitor(VecId res) : res(res)
    {
#ifdef DUMP_VISITOR_INFO
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
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(res);
    }
#endif
};

/** Add dt*mass*Gravity to the velocity
	This is called if the mass wants to be added separately to the mm from the other forces
 */
class MechanicalAddSeparateGravityVisitor : public MechanicalVisitor
{
public:

    double dt;
    VecId res;
    MechanicalAddSeparateGravityVisitor(double dt, VecId res) : dt(dt), res(res)
    {
#ifdef DUMP_VISITOR_INFO
        setReadWriteVectors();
#endif
    }

    /// Process the BaseMass
    virtual Result fwdMass(simulation::Node* /*node*/, core::componentmodel::behavior::BaseMass* mass);

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    virtual const char* getClassName() const { return "MechanicalAddSeparateGravityVisitor"; }
    virtual const char* getInfos() const { std::string name= "["+res.getName()+"]"; return name.c_str(); }
#ifdef DUMP_VISITOR_INFO
    void setReadWriteVectors()
    {
        addReadWriteVector(res);
    }
#endif
};



} // namespace simulation

} // namespace sofa

#endif
