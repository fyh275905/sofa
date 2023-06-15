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
#pragma once

#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/objectmodel/TypeOfInsertion.h>

namespace sofa::core::objectmodel
{

/**
 *  \brief Base class for simulation nodes.
 *
 *  A Node is a class defining the main scene data structure of a simulation.
 *  It defined hierarchical relations between elements.
 *  Each node can have parent and child nodes (potentially defining a tree),
 *  as well as attached objects (the leaves of the tree).
 *
 * \author Jeremie Allard
 */
class SOFA_CORE_API BaseNode : public virtual Base
{
public:
    SOFA_ABSTRACT_CLASS(BaseNode, Base);
    SOFA_BASE_CAST_IMPLEMENTATION(BaseNode)

protected:
    BaseNode() ;
    ~BaseNode() override;

private:
    BaseNode(const BaseNode& n) ;
    BaseNode& operator=(const BaseNode& n) ;
public:
    /// @name Scene hierarchy
    /// @{

    typedef sofa::type::vector< BaseNode* > Children;
    /// Get a list of child node
    virtual Children getChildren() const = 0;

    typedef sofa::type::vector< BaseNode* > Parents;
    /// Get a list of parent node
    /// @warning a temporary is created, this can be really inefficient
    virtual Parents getParents() const = 0;

    /// returns number of parents
    virtual size_t getNbParents() const = 0;

    /// return the first parent (returns nullptr if no parent)
    virtual BaseNode* getFirstParent() const = 0;

    /// returns the root by following up the first parent for multinodes
    virtual BaseNode* getRoot() const;

    /// Add a child node
    virtual void addChild(BaseNode::SPtr node) = 0;

    /// Remove a child node
    virtual void removeChild(BaseNode::SPtr node) = 0;

    /// Move a node from another node
    virtual void moveChild(BaseNode::SPtr node, BaseNode::SPtr prev_parent) = 0;
    /// Move a node in this & remove it from its parents
    virtual void moveChild(BaseNode::SPtr node) = 0;

    /// Add a generic object
    virtual bool addObject(BaseObject::SPtr obj, TypeOfInsertion = TypeOfInsertion::AtEnd) = 0;

    /// Remove a generic object
    virtual bool removeObject(BaseObject::SPtr obj) = 0;

    /// Move an object from a node to another node
    virtual void moveObject(BaseObject::SPtr obj) = 0;

    /// Test if the given node is a parent of this node.
    virtual bool hasParent(const BaseNode* node) const = 0;

    /// Test if the given node is an ancestor of this node.
    /// An ancestor is a parent or (recursively) the parent of an ancestor.
    virtual bool hasAncestor(const BaseNode* node) const = 0;

    /// Remove the current node from the graph: depending on the type of Node, it can have one or several parents.
    /// \warning You must have a SPtr on the node you detach if you want to keep it or the smart pointer mechanism will remove it !
    virtual void detachFromGraph() = 0;

    /// Get this node context
    virtual BaseContext* getContext() = 0;

    /// Get this node context
    virtual const BaseContext* getContext() const = 0;

    /// Return the full path name of this node
    virtual std::string getPathName() const override;

    /// Return the path from this node to the root node
    virtual std::string getRootPath() const;

    Base* findLinkDestClass(const BaseClass* destType, const std::string& path, const BaseLink* link) override = 0;

    /// @}

    /// @name Solvers and main algorithms
    /// @{

    virtual core::behavior::BaseAnimationLoop* getAnimationLoop() const;
    virtual core::behavior::OdeSolver* getOdeSolver() const;
    virtual core::collision::Pipeline* getCollisionPipeline() const;
    virtual core::visual::VisualLoop* getVisualLoop() const;

private:
    virtual std::string internalGetPathName() const;

    /// @}
protected:
    /// Set the context of an object to this
    void setObjectContext(BaseObject::SPtr obj);

    /// Reset the context of an object
    void clearObjectContext(BaseObject::SPtr obj);

public:

    /// @name virtual functions to add/remove special components direclty in the right Sequence
    /// Note it is useful for Node, but is not mandatory for every BaseNode Inheritances
    /// so the default implementation does nothing
    /// @{

    virtual void addAnimationLoop(core::behavior::BaseAnimationLoop*) {}
    virtual void removeAnimationLoop(core::behavior::BaseAnimationLoop*) {}

    virtual void addVisualLoop(core::visual::VisualLoop*) {}
    virtual void removeVisualLoop(core::visual::VisualLoop*) {}

    virtual void addBehaviorModel(core::BehaviorModel*) {}
    virtual void removeBehaviorModel(core::BehaviorModel*) {}

    virtual void addMapping(core::BaseMapping*) {}
    virtual void removeMapping(core::BaseMapping*) {}

    virtual void addOdeSolver(core::behavior::OdeSolver*) {}
    virtual void removeOdeSolver(core::behavior::OdeSolver*) {}

    virtual void addConstraintSolver(core::behavior::ConstraintSolver*) {}
    virtual void removeConstraintSolver(core::behavior::ConstraintSolver*) {}

    virtual void addLinearSolver(core::behavior::BaseLinearSolver*) {}
    virtual void removeLinearSolver(core::behavior::BaseLinearSolver*) {}

    virtual void addTopology(core::topology::Topology*) {}
    virtual void removeTopology(core::topology::Topology*) {}

    virtual void addMeshTopology(core::topology::BaseMeshTopology*) {}
    virtual void removeMeshTopology(core::topology::BaseMeshTopology*) {}

    virtual void addTopologyObject(core::topology::BaseTopologyObject*) {}
    virtual void removeTopologyObject(core::topology::BaseTopologyObject*) {}

    virtual void addState(core::BaseState*) {}
    virtual void removeState(core::BaseState*) {}

    virtual void addMechanicalState(core::behavior::BaseMechanicalState*) {}
    virtual void removeMechanicalState(core::behavior::BaseMechanicalState*) {}

    virtual void addMechanicalMapping(core::BaseMapping*) {}
    virtual void removeMechanicalMapping(core::BaseMapping*) {}

    virtual void addMass(core::behavior::BaseMass*) {}
    virtual void removeMass(core::behavior::BaseMass*) {}

    virtual void addForceField(core::behavior::BaseForceField*) {}
    virtual void removeForceField(core::behavior::BaseForceField*) {}

    virtual void addInteractionForceField(core::behavior::BaseInteractionForceField*) {}
    virtual void removeInteractionForceField(core::behavior::BaseInteractionForceField*) {}

    virtual void addProjectiveConstraintSet(core::behavior::BaseProjectiveConstraintSet*) {}
    virtual void removeProjectiveConstraintSet(core::behavior::BaseProjectiveConstraintSet*) {}

    virtual void addConstraintSet(core::behavior::BaseConstraintSet*) {}
    virtual void removeConstraintSet(core::behavior::BaseConstraintSet*) {}

    virtual void addContextObject(core::objectmodel::ContextObject*) {}
    virtual void removeContextObject(core::objectmodel::ContextObject*) {}

    virtual void addConfigurationSetting(core::objectmodel::ConfigurationSetting*) {}
    virtual void removeConfigurationSetting(core::objectmodel::ConfigurationSetting*) {}

    virtual void addShader(core::visual::Shader*) {}
    virtual void removeShader(core::visual::Shader*) {}

    virtual void addVisualModel(core::visual::VisualModel*) {}
    virtual void removeVisualModel(core::visual::VisualModel*) {}

    virtual void addVisualManager(core::visual::VisualManager*) {}
    virtual void removeVisualManager(core::visual::VisualManager*) {}

    virtual void addCollisionModel(core::CollisionModel*) {}
    virtual void removeCollisionModel(core::CollisionModel*) {}

    virtual void addCollisionPipeline(core::collision::Pipeline*) {}
    virtual void removeCollisionPipeline(core::collision::Pipeline*) {}
    /// @}

};
} // namespace sofa::core::objectmodel

