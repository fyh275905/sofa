/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2017 INRIA, USTL, UJF, CNRS, MGH                    *
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
//
// C++ Interface: Node
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SOFA_SIMULATION_CORE_NODE_H
#define SOFA_SIMULATION_CORE_NODE_H

#include <sofa/simulation/Node_fwd.h>
#include <sofa/simulation/Sequence.h>
#include <sofa/simulation/MutationListener_fwd.h>
#include <sofa/simulation/VisitorScheduler_fwd.h>

#include <sofa/core/BehaviorModel_fwd.h>
#include <sofa/core/CollisionModel_fwd.h>
#include <sofa/core/ExecParams_fwd.h>
#include <sofa/core/BaseMapping_fwd.h>
#include <sofa/core/objectmodel/Context.h>
#include <sofa/core/objectmodel/BaseNode.h>
#include <sofa/core/objectmodel/BaseObjectDescription_fwd.h>
#include <sofa/core/objectmodel/ConfigurationSetting_fwd.h>
#include <sofa/core/objectmodel/ContextObject_fwd.h>
#include <sofa/core/visual/VisualModel_fwd.h>
#include <sofa/core/visual/VisualManager_fwd.h>
#include <sofa/core/visual/VisualParams_fwd.h>
#include <sofa/core/visual/Shader_fwd.h>
#include <sofa/core/behavior/BaseMechanicalState_fwd.h>
#include <sofa/core/behavior/LinearSolver_fwd.h>
#include <sofa/core/behavior/BaseForceField_fwd.h>
#include <sofa/core/behavior/BaseInteractionForceField_fwd.h>
#include <sofa/core/behavior/BaseMass_fwd.h>
#include <sofa/core/behavior/BaseProjectiveConstraintSet_fwd.h>
#include <sofa/core/behavior/BaseConstraintSet_fwd.h>
#include <sofa/core/topology/Topology_fwd.h>
#include <sofa/core/topology/BaseTopologyObject_fwd.h>
#include <sofa/core/topology/BaseMeshTopology_fwd.h>
#include <sofa/core/behavior/OdeSolver_fwd.h>
#include <sofa/core/behavior/ConstraintSolver_fwd.h>
#include <sofa/core/behavior/BaseAnimationLoop_fwd.h>
#include <sofa/core/visual/VisualLoop_fwd.h>
#include <sofa/core/visual/VisualParams_fwd.h>
#include <sofa/core/collision/Pipeline_fwd.h>
#include <sofa/core/objectmodel/Event_fwd.h>

#include <sofa/simulation/simulationcore.h>

#include <string>
#include <stack>

/// EXTERN TEMPLATE INSTANTIATION.
namespace sofa
{
namespace simulation
{

//      extern template class SOFA_SIMULATION_CORE_API Sequence<Node,true> ;
//      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::objectmodel::BaseObject,true> ;
      extern template class SOFA_SIMULATION_CORE_API Single<sofa::core::behavior::BaseAnimationLoop> ;
      extern template class SOFA_SIMULATION_CORE_API Single<sofa::core::visual::VisualLoop> ;

      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::BehaviorModel> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::BaseMapping> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::behavior::OdeSolver> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::behavior::ConstraintSolver> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::behavior::BaseLinearSolver> ;

      extern template class SOFA_SIMULATION_CORE_API Single<sofa::core::topology::Topology> ;
      extern template class SOFA_SIMULATION_CORE_API Single<sofa::core::topology::BaseMeshTopology> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::topology::BaseTopologyObject> ;
      extern template class SOFA_SIMULATION_CORE_API Single<sofa::core::BaseState> ;
      extern template class SOFA_SIMULATION_CORE_API Single<sofa::core::behavior::BaseMechanicalState> ;
      extern template class SOFA_SIMULATION_CORE_API Single<sofa::core::BaseMapping> ;
      extern template class SOFA_SIMULATION_CORE_API Single<sofa::core::behavior::BaseMass>;

      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::behavior::BaseForceField> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::behavior::BaseInteractionForceField> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::behavior::BaseProjectiveConstraintSet> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::behavior::BaseConstraintSet> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::objectmodel::ContextObject> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::objectmodel::ConfigurationSetting> ;

      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::visual::Shader> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::visual::VisualModel> ;
      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::visual::VisualManager> ;

      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::CollisionModel> ;
      extern template class SOFA_SIMULATION_CORE_API Single<sofa::core::collision::Pipeline> ;

      extern template class SOFA_SIMULATION_CORE_API Sequence<sofa::core::objectmodel::BaseObject> ;
}
}


namespace sofa {
namespace simulation {
        class Visitor;
    }
}


namespace sofa
{
namespace simulation
{

/**
   Implements the object (component) management of the core::Context.
   Contains objects in lists and provides accessors.
   The other nodes are not visible (unknown scene graph).

   @author The SOFA team </www.sofa-framework.org>
 */
class SOFA_SIMULATION_CORE_API Node : public sofa::core::objectmodel::BaseNode, public sofa::core::objectmodel::Context
{

public:
    SOFA_ABSTRACT_CLASS2(Node, BaseNode, Context);

    typedef sofa::core::visual::DisplayFlags DisplayFlags;
protected:
    Node(const std::string& name="");

    virtual ~Node();
public:
    /// Create, add, then return the new child of this Node
    virtual sofa::simulation::NodeSPtr createChild(const std::string& nodeName)=0;

    /// @name High-level interface
    /// @{

    /// Parse the given description to assign values to this object's fields and potentially other parameters
    virtual void parse ( sofa::core::objectmodel::BaseObjectDescription* arg ) override;

    /// Initialize the components
    void init(const sofa::core::ExecParams* params);
    bool isInitialized() {return initialized;}
    /// Apply modifications to the components
    void reinit(const sofa::core::ExecParams* params);

    /// Draw the objects (using visual visitors)
    void draw(sofa::core::visual::VisualParams* params);
    /// @}

    /// @name Visitor handling
    /// @param precomputedOrder is not used by default but could allow optimization on certain Node specializations
    /// @warning when calling with precomputedOrder=true, the fonction "precomputeTraversalOrder" must be called before executing the visitor and the user must ensure by himself that the simulation graph has done been modified since the last call to "precomputeTraversalOrder"
    /// @{

    /// Execute a recursive action starting from this node.
    virtual void doExecuteVisitor(Visitor* action, bool precomputedOrder=false)=0;

    /// Execute a recursive action starting from this node
    void executeVisitor( simulation::Visitor* action, bool precomputedOrder=false) override;

    /// Execute a recursive action starting from this node
    void execute(simulation::Visitor& action, bool precomputedOrder=false)
    {
        simulation::Visitor* p = &action;
        executeVisitor(p, precomputedOrder);
    }

    /// Execute a recursive action starting from this node
    void execute(simulation::Visitor* p, bool precomputedOrder=false)
    {
        executeVisitor(p, precomputedOrder);
    }

    /// Execute a recursive action starting from this node
    template<class Act, class Params = sofa::core::visual::VisualParams* >
    void execute(const Params* params, bool precomputedOrder=false)
    {
        Act action(params);
        simulation::Visitor* p = &action;
        executeVisitor(p, precomputedOrder);
    }

    /// Execute a recursive action starting from this node
    template<class Act, class Params = sofa::core::visual::VisualParams* >
    void execute(Params* params, bool precomputedOrder=false)
    {
        Act action(params);
        simulation::Visitor* p = &action;
        executeVisitor(p, precomputedOrder);
    }



    /// Possible optimization with traversal precomputation, not mandatory and does nothing by default
    virtual void precomputeTraversalOrder( const sofa::core::ExecParams* ) {}

    /// @}

    /// @name Component containers
    /// @{
    // methods moved from GNode (27/04/08)



    Sequence<Node,true> child;
    typedef Sequence<Node,true>::iterator ChildIterator;

    Sequence<sofa::core::objectmodel::BaseObject,true> object;
    typedef Sequence<sofa::core::objectmodel::BaseObject,true>::iterator ObjectIterator;
    typedef Sequence<sofa::core::objectmodel::BaseObject,true>::reverse_iterator ObjectReverseIterator;

    Single<sofa::core::behavior::BaseAnimationLoop> animationManager;
    Single<sofa::core::visual::VisualLoop> visualLoop;

    Sequence<sofa::core::BehaviorModel> behaviorModel;
    Sequence<sofa::core::BaseMapping> mapping;

    Sequence<sofa::core::behavior::OdeSolver> solver;
    Sequence<sofa::core::behavior::ConstraintSolver> constraintSolver;
    Sequence<sofa::core::behavior::BaseLinearSolver> linearSolver;

    Single<sofa::core::topology::Topology> topology;
    Single<sofa::core::topology::BaseMeshTopology> meshTopology;
    Sequence<sofa::core::topology::BaseTopologyObject> topologyObject;

    Single<sofa::core::BaseState> state;
    Single<sofa::core::behavior::BaseMechanicalState> mechanicalState;
    Single<sofa::core::BaseMapping> mechanicalMapping;
    Single<sofa::core::behavior::BaseMass> mass;
    Sequence<sofa::core::behavior::BaseForceField> forceField;
    Sequence<sofa::core::behavior::BaseInteractionForceField> interactionForceField;
    Sequence<sofa::core::behavior::BaseProjectiveConstraintSet> projectiveConstraintSet;
    Sequence<sofa::core::behavior::BaseConstraintSet> constraintSet;
    Sequence<sofa::core::objectmodel::ContextObject> contextObject;
    Sequence<sofa::core::objectmodel::ConfigurationSetting> configurationSetting;

    Sequence<sofa::core::visual::Shader> shaders;
    Sequence<sofa::core::visual::VisualModel> visualModel;
    Sequence<sofa::core::visual::VisualManager> visualManager;

    Sequence<sofa::core::CollisionModel> collisionModel;
    Single<sofa::core::collision::Pipeline> collisionPipeline;

    Sequence<sofa::core::objectmodel::BaseObject> unsorted;

    /// @}


    /// @name Set/get objects
    /// @{

    /// Add an object and return this. Detect the implemented interfaces and add the object to the corresponding lists.
    virtual bool addObject(sofa::core::objectmodel::BaseObject::SPtr obj) override;

    /// Remove an object
    virtual bool removeObject(sofa::core::objectmodel::BaseObject::SPtr obj) override;

    /// Move an object from another node
    virtual void moveObject(sofa::core::objectmodel::BaseObject::SPtr obj) override;

    /// Find an object given its name
    sofa::core::objectmodel::BaseObject* getObject(const std::string& name) const;

    void* findLinkDestClass(const sofa::core::objectmodel::BaseClass* destType, const std::string& path, const sofa::core::objectmodel::BaseLink* link) override;


    /// Generic object access, given a set of required tags, possibly searching up or down from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void* getObject(const sofa::core::objectmodel::ClassInfo& class_info, const sofa::core::objectmodel::TagSet& tags, SearchDirection dir = SearchUp) const override = 0;

    /// Generic object access, possibly searching up or down from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    void* getObject(const sofa::core::objectmodel::ClassInfo& class_info, SearchDirection dir = SearchUp) const override
    {
        return getObject(class_info, sofa::core::objectmodel::TagSet(), dir);
    }

    /// Generic object access, given a path from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void* getObject(const sofa::core::objectmodel::ClassInfo& class_info, const std::string& path) const override = 0;

    /// Generic list of objects access, given a set of required tags, possibly searching up or down from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void getObjects(const sofa::core::objectmodel::ClassInfo& class_info, GetObjectsCallBack& container, const sofa::core::objectmodel::TagSet& tags, SearchDirection dir = SearchUp) const  override = 0;

    /// Generic list of objects access, possibly searching up or down from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    void getObjects(const sofa::core::objectmodel::ClassInfo& class_info, GetObjectsCallBack& container, SearchDirection dir = SearchUp) const override
    {
        getObjects(class_info, container, sofa::core::objectmodel::TagSet(), dir);
    }

    /// List all objects of this node deriving from a given class
    template<class Object, class Container>
    void getNodeObjects(Container* list)
    {
       return BaseContext::getObjects<Object, Container>(list, Local) ;
    }

    /// Returns a list of object of type passed as a parameter.
    template<class Container>
    Container* getNodeObjects(Container* result)
    {
        return BaseContext::getObjects(result, Local) ;
    }

    /// Returns a list of object of type passed as a parameter
    template<class Container>
    Container& getNodeObjects(Container& result)
    {
        return BaseContext::getObjects(result, Local);
    }

    /// Returns a list of object of type passed as a parameter.
    /// This function is return object by copy but should be compatible with
    /// Return Value Optimization so the copy should be removed by the compiler.
    /// Eg:
    ///     for( BaseObject* o : node->getNodeObjects() ) { ... }
    ///     for( VisualModel* v : node->getNodeObjects<VisualModel>() ) { ... }
    template<class Object=sofa::core::objectmodel::BaseObject>
    std::vector<Object*> getNodeObjects()
    {
        std::vector<Object*> tmp ;
        BaseContext::getObjects(tmp, Local);
        return tmp;
    }

    /// Return an object of this node deriving from a given class, or NULL if not found.
    /// Note that only the first object is returned.
    template<class Object>
    void getNodeObject(Object*& result)
    {
        result = this->get<Object>(Local);
    }

    template<class Object>
    Object* getNodeObject()
    {
        return this->get<Object>(Local);
    }

    /// List all objects of this node and sub-nodes deriving from a given class
    template<class Object, class Container>
    void getTreeObjects(Container* list)
    {
        this->get<Object, Container>(list, SearchDown);
    }

    /// List all objects of this node and sub-nodes deriving from a given class
    template<class Container>
    Container* getTreeObjects(Container* result)
    {
        return BaseContext::getObjects(result,  SearchDown);
    }

    /// List all objects of this node and sub-nodes deriving from a given class
    template<class Container>
    Container& getTreeObjects(Container& result)
    {
        return BaseContext::getObjects(result,  SearchDown);
    }

    /// List all objects of this node and sub-nodes deriving from a given class
    /// This function is return a std::vector by copy but should be compatible with
    /// Return Value Optimization so the copy should be removed by the compiler.
    /// Eg:
    ///     for( BaseObject* o : node->getTreeObjects() ) { ... }
    ///     for( VisualModel* v : node->getTreeObjects<VisualModel>() ) { ... }
    template<class Object=sofa::core::objectmodel::BaseObject>
    std::vector<Object*> getTreeObjects()
    {
        std::vector<Object*> tmp ;
        BaseContext::getObjects(tmp, SearchDown);
        return tmp;
    }



    /// Return an object of this node and sub-nodes deriving from a given class, or NULL if not found.
    /// Note that only the first object is returned.
    template<class Object>
    void getTreeObject(Object*& result)
    {
        result = this->get<Object>(SearchDown);
    }

    template<class Object>
    Object* getTreeObject()
    {
        return this->get<Object>(SearchDown);
    }

    /// Topology
    virtual sofa::core::topology::Topology* getTopology() const override;

    /// Mesh Topology (unified interface for both static and dynamic topologies)
    virtual sofa::core::topology::BaseMeshTopology* getMeshTopology() const override;

    /// Mesh Topology that is local to this context (i.e. not within parent contexts)
    virtual core::topology::BaseMeshTopology* getLocalMeshTopology() const override;

    /// Degrees-of-Freedom
    virtual sofa::core::BaseState* getState() const override;

    /// Mechanical Degrees-of-Freedom
    virtual sofa::core::behavior::BaseMechanicalState* getMechanicalState() const override;

    /// Shader
    virtual sofa::core::visual::Shader* getShader() const override;
    virtual sofa::core::visual::Shader* getShader(const sofa::core::objectmodel::TagSet& t) const;

    /// @name Solvers and main algorithms
    /// @{

    virtual sofa::core::behavior::BaseAnimationLoop* getAnimationLoop() const override;
    virtual sofa::core::behavior::OdeSolver* getOdeSolver() const override;
    virtual sofa::core::collision::Pipeline* getCollisionPipeline() const override;
    virtual sofa::core::visual::VisualLoop* getVisualLoop() const override;

    /// @}

    /// Remove odesolvers and mastercontroler
    virtual void removeControllers();

    /// Find a child node given its name
    Node* getChild(const std::string& name) const;

    /// Get a descendant node given its name
    Node* getTreeNode(const std::string& name) const;

    /// Get children nodes
    virtual Children getChildren() const override;

    BaseContext* getRootContext() const override
    {
        return getRoot()->getContext();
    }

    Node* setDebug(bool);
    bool getDebug() const;
    // debug
    void printComponents();

    const BaseContext* getContext() const override;
    BaseContext* getContext() override;

    /// Update the whole context values, based on parent and local ContextObjects
    virtual void updateContext();

    /// Update the simulation context values(gravity, time...), based on parent and local ContextObjects
    virtual void updateSimulationContext();

    /// Called during initialization to corectly propagate the visual context to the children
    virtual void initVisualContext() {}

    /// Propagate an event
    virtual void propagateEvent(const sofa::core::ExecParams* params, sofa::core::objectmodel::Event* event) override;

    /// Update the visual context values, based on parent and local ContextObjects
    virtual void updateVisualContext();

    // VisitorScheduler can use doExecuteVisitor() method
    friend class VisitorScheduler;

    /// Must be called after each graph modification. Do not call it directly, apply an InitVisitor instead.
    virtual void initialize();

    virtual void bwdInit();

    /// Called after initialization to set the default value of the visual context.
    virtual void setDefaultVisualContextValue();

    template <class RealObject>
    static NodeSPtr create(RealObject*, core::objectmodel::BaseObjectDescription* arg);


    static NodeSPtr create( const std::string& name );

    /// return the smallest common parent between this and node2 (returns NULL if separated sub-graphes)
    virtual Node* findCommonParent( simulation::Node* node2 ) = 0;

	/// override context setSleeping to add notification.
	virtual void setSleeping(bool /*val*/) override;

protected:
    bool debug_;
    bool initialized;

    virtual void doAddObject(sofa::core::objectmodel::BaseObject::SPtr obj);
    virtual void doRemoveObject(sofa::core::objectmodel::BaseObject::SPtr obj);


    std::stack<Visitor*> actionStack;

    virtual void notifyAddChild(NodeSPtr node);
    virtual void notifyRemoveChild(NodeSPtr node);
    virtual void notifyMoveChild(NodeSPtr node, Node* prev);
    virtual void notifyAddObject(sofa::core::objectmodel::BaseObject::SPtr obj);
    virtual void notifyRemoveObject(sofa::core::objectmodel::BaseObject::SPtr obj);
    virtual void notifyMoveObject(sofa::core::objectmodel::BaseObject::SPtr obj, Node* prev);
    virtual void notifySleepChanged();


    BaseContext* _context;

    helper::vector<MutationListener*> listener;


public:

    virtual void notifyAddSlave(sofa::core::objectmodel::BaseObject* master, sofa::core::objectmodel::BaseObject* slave) override;
    virtual void notifyRemoveSlave(sofa::core::objectmodel::BaseObject* master, sofa::core::objectmodel::BaseObject* slave) override;
    virtual void notifyMoveSlave(sofa::core::objectmodel::BaseObject* previousMaster, sofa::core::objectmodel::BaseObject* master, sofa::core::objectmodel::BaseObject* slave) override;

    virtual void addListener(MutationListener* obj);
    virtual void removeListener(MutationListener* obj);

    // Added by FF to model component dependencies
    /// Pairs representing component dependencies. First must be initialized before second.
    Data < sofa::helper::vector < std::string > > depend;

    /// Sort the components according to the dependencies expressed in Data depend.
    void sortComponents();



    /// @name virtual functions to add/remove some special components direclty in the right Sequence
    /// @{

#define NODE_ADD_IN_SEQUENCE( CLASSNAME, FUNCTIONNAME, SEQUENCENAME ) \
    virtual void add##FUNCTIONNAME( CLASSNAME* obj ) override { SEQUENCENAME.add(obj); } \
    virtual void remove##FUNCTIONNAME( CLASSNAME* obj ) override { SEQUENCENAME.remove(obj); }

    // WARNINGS subtilities:
    // an InteractioFF is NOT in the FF Sequence
    // a MechanicalMapping is NOT in the Mapping Sequence
    // a Mass is in the FF Sequence
    // a MeshTopology is in the topology Sequence

public:

    NODE_ADD_IN_SEQUENCE( sofa::core::behavior::BaseAnimationLoop, AnimationLoop, animationManager )
    NODE_ADD_IN_SEQUENCE( sofa::core::visual::VisualLoop, VisualLoop, visualLoop )
    NODE_ADD_IN_SEQUENCE( sofa::core::BehaviorModel, BehaviorModel, behaviorModel )
    NODE_ADD_IN_SEQUENCE( sofa::core::BaseMapping, Mapping, mapping )
    NODE_ADD_IN_SEQUENCE( sofa::core::behavior::OdeSolver, OdeSolver, solver )
    NODE_ADD_IN_SEQUENCE( sofa::core::behavior::ConstraintSolver, ConstraintSolver, constraintSolver )
    NODE_ADD_IN_SEQUENCE( sofa::core::behavior::BaseLinearSolver, LinearSolver, linearSolver )
    NODE_ADD_IN_SEQUENCE( sofa::core::topology::Topology, Topology, topology )
    NODE_ADD_IN_SEQUENCE( sofa::core::topology::BaseMeshTopology, MeshTopology, meshTopology )
    NODE_ADD_IN_SEQUENCE( sofa::core::topology::BaseTopologyObject, TopologyObject, topologyObject )
    NODE_ADD_IN_SEQUENCE( sofa::core::BaseState, State, state )
    NODE_ADD_IN_SEQUENCE( sofa::core::behavior::BaseMechanicalState,MechanicalState, mechanicalState )
    NODE_ADD_IN_SEQUENCE( sofa::core::BaseMapping, MechanicalMapping, mechanicalMapping )
    NODE_ADD_IN_SEQUENCE( sofa::core::behavior::BaseMass, Mass, mass )
    NODE_ADD_IN_SEQUENCE( sofa::core::behavior::BaseForceField, ForceField, forceField )
    NODE_ADD_IN_SEQUENCE( sofa::core::behavior::BaseInteractionForceField, InteractionForceField, interactionForceField )
    NODE_ADD_IN_SEQUENCE( sofa::core::behavior::BaseProjectiveConstraintSet, ProjectiveConstraintSet, projectiveConstraintSet )
    NODE_ADD_IN_SEQUENCE( sofa::core::behavior::BaseConstraintSet, ConstraintSet, constraintSet )
    NODE_ADD_IN_SEQUENCE( sofa::core::objectmodel::ContextObject, ContextObject, contextObject )
    NODE_ADD_IN_SEQUENCE( sofa::core::objectmodel::ConfigurationSetting, ConfigurationSetting, configurationSetting )
    NODE_ADD_IN_SEQUENCE( sofa::core::visual::Shader, Shader, shaders )
    NODE_ADD_IN_SEQUENCE( sofa::core::visual::VisualModel, VisualModel, visualModel )
    NODE_ADD_IN_SEQUENCE( sofa::core::visual::VisualManager, VisualManager, visualManager )
    NODE_ADD_IN_SEQUENCE( sofa::core::CollisionModel, CollisionModel, collisionModel )
    NODE_ADD_IN_SEQUENCE( sofa::core::collision::Pipeline, CollisionPipeline, collisionPipeline )

#undef NODE_ADD_IN_SEQUENCE

    /// @}

};

}

}

#endif
