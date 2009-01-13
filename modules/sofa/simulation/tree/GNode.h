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
#ifndef SOFA_SIMULATION_TREE_GNODE_H
#define SOFA_SIMULATION_TREE_GNODE_H

#include <sofa/simulation/common/Node.h>
#include <sofa/simulation/tree/tree.h>
#include <sofa/simulation/tree/MutationListener.h>
#include <sofa/simulation/tree/xml/NodeElement.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>


using std::cout;
using std::endl;

namespace sofa
{

namespace simulation
{
class Visitor;
class VisitorScheduler;

namespace tree
{

class MutationListener;

/** Define the structure of the scene. Contains (as pointer lists) Component objects and children GNode objects.
*/
class SOFA_SIMULATION_TREE_API GNode : public simulation::Node, public core::objectmodel::BaseNode
{
public:
    GNode( const std::string& name="", GNode* parent=NULL  );

    virtual ~GNode();

    //virtual const char* getTypeName() const { return "GNODE"; }
    void reinit();

    /// Add a child node
    virtual void addChild(GNode* node);

    /// Remove a child
    virtual void removeChild(GNode* node);

    /// Add a child node
    virtual void addChild(BaseNode* node);

    /// Remove a child node
    virtual void removeChild(BaseNode* node);

    /// Move a node from another node
    virtual void moveChild(GNode* obj);


    /// @name Visitors and graph traversal
    /// @{

    /// Execute a recursive action starting from this node
//    virtual void executeVisitor(Visitor* action);

    /// Get parent node (or NULL if no hierarchy or for root node)
    core::objectmodel::BaseNode* getParent();

    /// Get parent node (or NULL if no hierarchy or for root node)
    const core::objectmodel::BaseNode* getParent() const;

    /// Get parent node (or NULL if no hierarchy or for root node)
    sofa::helper::vector< core::objectmodel::BaseNode* > getChildren();

    /// Get parent node (or NULL if no hierarchy or for root node)
    const sofa::helper::vector< core::objectmodel::BaseNode* > getChildren() const;


    /// Find a child node given its name
    GNode* getChild(const std::string& name) const;


    /// Get a descendant node given its name
    GNode* getTreeNode(const std::string& name) const;

    /// @}

    /// @name Components
    /// @{

    /// Add an object and return this. Detect the implemented interfaces and add the object to the corresponding lists.
    virtual bool addObject(core::objectmodel::BaseObject* obj) { return simulation::Node::addObject(obj); }

    /// Remove an object
    virtual bool removeObject(core::objectmodel::BaseObject* obj) { return simulation::Node::removeObject(obj); }

    /// Import an object
    virtual void moveObject(core::objectmodel::BaseObject* obj) { simulation::Node::moveObject(obj); }

    /// Mechanical Degrees-of-Freedom
    virtual core::objectmodel::BaseObject* getMechanicalState() const;

    /// Topology
    virtual core::componentmodel::topology::Topology* getTopology() const;

    /// Mesh Topology (unified interface for both static and dynamic topologies)
    virtual core::componentmodel::topology::BaseMeshTopology* getMeshTopology() const;

    /// Shader
    virtual core::objectmodel::BaseObject* getShader() const;

    const BaseContext* getContext() const { return simulation::Node::getContext(); }
    BaseContext* getContext() { return simulation::Node::getContext(); }


    /// @}


    /// Called during initialization to corectly propagate the visual context to the children
    virtual void initVisualContext();

    /// Update the whole context values, based on parent and local ContextObjects
    virtual void updateContext();

    /// Update the visual context values, based on parent and local ContextObjects
    virtual void updateVisualContext(int FILTER=0);

    /// Update the simulation context values(gravity, time...), based on parent and local ContextObjects
    virtual void updateSimulationContext();


    /// Log time spent on an action category and the concerned object
    void addTime(ctime_t t, const std::string& s, core::objectmodel::BaseObject* obj);

    /// Log time spent given a start time, an action category, and the concerned object
    ctime_t endTime(ctime_t t0, const std::string& s, core::objectmodel::BaseObject* obj);

    /// Log time spent on an action category, and the concerned object, plus remove the computed time from the parent caller object
    void addTime(ctime_t t, const std::string& s, core::objectmodel::BaseObject* obj, core::objectmodel::BaseObject* parent);

    /// Log time spent given a start time, an action category, and the concerned object, plus remove the computed time from the parent caller object
    ctime_t endTime(ctime_t t0, const std::string& s, core::objectmodel::BaseObject* obj, core::objectmodel::BaseObject* parent);




    /// Return the full path name of this node
    std::string getPathName() const;

    /// Generic object access, possibly searching up or down from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void* getObject(const sofa::core::objectmodel::ClassInfo& class_info, SearchDirection dir = SearchUp) const;

    /// Generic object access, given a path from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void* getObject(const sofa::core::objectmodel::ClassInfo& class_info, const std::string& path) const;

    /// Generic list of objects access, possibly searching up or down from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void getObjects(const sofa::core::objectmodel::ClassInfo& class_info, GetObjectsCallBack& container, SearchDirection dir = SearchUp) const;

    void addListener(MutationListener* obj);

    void removeListener(MutationListener* obj);


    // should this be public ?
    Single<GNode> parent;
    Sequence<GNode> child;
    typedef Sequence<GNode>::iterator ChildIterator;



    static void create(GNode*& obj, xml::Element<core::objectmodel::BaseNode>* arg)
    {
        obj = new GNode();
        obj->parse(arg);
    }

protected:


    virtual void doAddChild(GNode* node);
    void doRemoveChild(GNode* node);

    void notifyAddChild(GNode* node);
    void notifyRemoveChild(GNode* node);
    void notifyMoveChild(GNode* node, GNode* prev);

    /// Execute a recursive action starting from this node.
    /// This method bypass the actionScheduler of this node if any.
    void doExecuteVisitor(simulation::Visitor* action);
    // VisitorScheduler can use doExecuteVisitor() method
    friend class simulation::VisitorScheduler;

    Sequence<MutationListener> listener;


protected:
    /*    virtual void doAddObject(core::objectmodel::BaseObject* obj);
        virtual void doRemoveObject(core::objectmodel::BaseObject* obj);*/
    void notifyAddObject(core::objectmodel::BaseObject* obj);
    void notifyRemoveObject(core::objectmodel::BaseObject* obj);
    void notifyMoveObject(core::objectmodel::BaseObject* obj, GNode* prev);
};

} // namespace tree

} // namespace simulation

} // namespace sofa

#endif
