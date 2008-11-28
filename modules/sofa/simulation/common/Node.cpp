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
//
// C++ Implementation: Node
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "Node.h"
#include <sofa/simulation/common/PropagateEventVisitor.h>
#include <sofa/simulation/common/AnimateVisitor.h>
#include <sofa/simulation/common/DesactivatedNodeVisitor.h>
#include <sofa/simulation/common/InitVisitor.h>
#include <sofa/simulation/common/VisualVisitor.h>
#include <sofa/simulation/common/UpdateMappingVisitor.h>
#include <iostream>
using std::cerr;
using std::endl;

#include <boost/graph/adjacency_list.hpp>
//#include <sofa/core/objectmodel/BaseObject.h>
#include <boost/graph/topological_sort.hpp>


namespace sofa
{

namespace simulation
{
using core::objectmodel::BaseObject;
using helper::system::thread::CTime;

Node::Node(const std::string& name)
    : sofa::core::objectmodel::Context()
    , debug_(false), logTime_(false)
    , depend(initData(&depend,"depend","Dependencies between the nodes.\nname 1 name 2 name3 name4 means that name1 must be initialized before name2 and name3 before name4"))
{
    _context = this;
    totalTime.nVisit = 0;
    totalTime.tNode = 0;
    totalTime.tTree = 0;
    setName(name);
}


Node::~Node()
{
}

/// Initialize the components of this node and all the nodes which depend on it.
void Node::init()
{
    //cerr<<"Node::init() begin node "<<getName()<<endl;
    execute<simulation::InitVisitor>();
    //cerr<<"Node::init() end node "<<getName()<<endl;
}

/// ReInitialize the components of this node and all the nodes which depend on it.
void Node::reinit()
{
    sofa::simulation::DesactivationVisitor desactivate(isActive());
    desactivate.execute( this );
}

/// Do one step forward in time
void Node::animate( double dt )
{
    simulation::AnimateVisitor vis(dt);
    //cerr<<"Node::animate, start execute"<<endl;
    execute(vis);
    //cerr<<"Node::animate, end execute"<<endl;
    execute<simulation::UpdateMappingVisitor>();
}

void Node::glDraw()
{
    execute<simulation::VisualUpdateVisitor>();
    execute<simulation::VisualDrawVisitor>();
}





/// Add an object. Detect the implemented interfaces and add the object to the corresponding lists.
bool Node::addObject(BaseObject* obj)
{
    notifyAddObject(obj);
    doAddObject(obj);
    return true;
}

/// Remove an object
bool Node::removeObject(BaseObject* obj)
{
    notifyRemoveObject(obj);
    doRemoveObject(obj);
    return true;
}

/// Move an object from another node
void Node::moveObject(BaseObject* obj)
{
    Node* prev = dynamic_cast<Node*>(obj->getContext());
    if (prev==NULL)
    {
        obj->getContext()->removeObject(obj);
        addObject(obj);
    }
    else
    {
        notifyMoveObject(obj,prev);
        prev->doRemoveObject(obj);
        doAddObject(obj);
    }
}

/// Find an object given its name
core::objectmodel::BaseObject* Node::getObject(const std::string& name) const
{
    for (ObjectIterator it = object.begin(), itend = object.end(); it != itend; ++it)
        if ((*it)->getName() == name)
            return *it;
    return NULL;
}



/// Add an object. Detect the implemented interfaces and add the object to the corresponding lists.
void Node::doAddObject(BaseObject* obj)
{
    notifyAddObject(obj);
    obj->setContext(this);
    object.add(obj);
    int inserted=0;
    inserted+= masterSolver.add(dynamic_cast< core::componentmodel::behavior::MasterSolver* >(obj));
    inserted+= solver.add(dynamic_cast< core::componentmodel::behavior::OdeSolver* >(obj));
    inserted+= linearSolver.add(dynamic_cast< core::componentmodel::behavior::LinearSolver* >(obj));
    inserted+= mechanicalState.add(dynamic_cast< core::componentmodel::behavior::BaseMechanicalState* >(obj));
    bool isMechanicalMapping = mechanicalMapping.add(dynamic_cast< core::componentmodel::behavior::BaseMechanicalMapping* >(obj));
    inserted+= isMechanicalMapping;
    if (!isMechanicalMapping)
        inserted+= mapping.add(dynamic_cast< core::BaseMapping* >(obj));
    inserted+= mass.add(dynamic_cast< core::componentmodel::behavior::BaseMass* >(obj));
    inserted+= topology.add(dynamic_cast< core::componentmodel::topology::Topology* >(obj));
    inserted+= meshTopology.add(dynamic_cast< core::componentmodel::topology::BaseMeshTopology* >(obj));
    inserted+= shader.add(dynamic_cast< sofa::core::Shader* >(obj));

    bool isInteractionForceField = interactionForceField.add(dynamic_cast< core::componentmodel::behavior::InteractionForceField* >(obj));
    inserted+= isInteractionForceField;
    if (!isInteractionForceField)
        forceField.add(dynamic_cast< core::componentmodel::behavior::BaseForceField* >(obj));
    inserted+= constraint.add(dynamic_cast< core::componentmodel::behavior::BaseConstraint* >(obj));
    inserted+= LMConstraint.add(dynamic_cast< core::componentmodel::behavior::BaseLMConstraint* >(obj));
    inserted+= behaviorModel.add(dynamic_cast< core::BehaviorModel* >(obj));
    inserted+= visualModel.add(dynamic_cast< core::VisualModel* >(obj));
    inserted+= collisionModel.add(dynamic_cast< core::CollisionModel* >(obj));
    inserted+= contextObject.add(dynamic_cast< core::objectmodel::ContextObject* >(obj));
    inserted+= collisionPipeline.add(dynamic_cast< core::componentmodel::collision::Pipeline* >(obj));
    inserted+= actionScheduler.add(dynamic_cast< VisitorScheduler* >(obj));

    if ( inserted==0 )
    {
        //cerr<<"Node::doAddObject, object "<<obj->getName()<<" is unsorted"<<endl;
        unsorted.add(obj);
    }

}

/// Remove an object
void Node::doRemoveObject(BaseObject* obj)
{
    if (obj->getContext()==this)
    {
        obj->setContext(NULL);
    }
    object.remove(obj);
    masterSolver.remove(dynamic_cast< core::componentmodel::behavior::MasterSolver* >(obj));
    solver.remove(dynamic_cast< core::componentmodel::behavior::OdeSolver* >(obj));
    linearSolver.remove(dynamic_cast< core::componentmodel::behavior::LinearSolver* >(obj));
    mechanicalState.remove(dynamic_cast< core::componentmodel::behavior::BaseMechanicalState* >(obj));
    mechanicalMapping.remove(dynamic_cast< core::componentmodel::behavior::BaseMechanicalMapping* >(obj));
    mass.remove(dynamic_cast< core::componentmodel::behavior::BaseMass* >(obj));
    topology.remove(dynamic_cast< core::componentmodel::topology::Topology* >(obj));
    meshTopology.remove(dynamic_cast< core::componentmodel::topology::BaseMeshTopology* >(obj));
    shader.remove(dynamic_cast<sofa::core::Shader* >(obj));

    forceField.remove(dynamic_cast< core::componentmodel::behavior::BaseForceField* >(obj));
    interactionForceField.remove(dynamic_cast< core::componentmodel::behavior::InteractionForceField* >(obj));
    constraint.remove(dynamic_cast< core::componentmodel::behavior::BaseConstraint* >(obj));
    LMConstraint.remove(dynamic_cast< core::componentmodel::behavior::BaseLMConstraint* >(obj));
    mapping.remove(dynamic_cast< core::BaseMapping* >(obj));
    behaviorModel.remove(dynamic_cast< core::BehaviorModel* >(obj));
    visualModel.remove(dynamic_cast< core::VisualModel* >(obj));
    collisionModel.remove(dynamic_cast< core::CollisionModel* >(obj));
    contextObject.remove(dynamic_cast<core::objectmodel::ContextObject* >(obj));
    collisionPipeline.remove(dynamic_cast< core::componentmodel::collision::Pipeline* >(obj));

    actionScheduler.remove(dynamic_cast< VisitorScheduler* >(obj));
    // Remove references to this object in time log tables
    if (!objectTime.empty())
    {
        for (std::map<std::string, std::map<core::objectmodel::BaseObject*, ObjectTimer> >::iterator it = objectTime.begin(); it != objectTime.end(); ++it)
        {
            it->second.erase(obj);
        }
    }
}



/// Topology
core::componentmodel::topology::Topology* Node::getTopology() const
{
    return this->topology;
}

/// Mesh Topology (unified interface for both static and dynamic topologies)
core::componentmodel::topology::BaseMeshTopology* Node::getMeshTopology() const
{
    return this->meshTopology;
}

/// Shader
core::objectmodel::BaseObject* Node::getShader() const
{
    return shader;
}

/// Mechanical Degrees-of-Freedom
core::objectmodel::BaseObject* Node::getMechanicalState() const
{
    return this->mechanicalState;
}


void Node::setLogTime(bool b)
{
    logTime_=b;
}

Node::ctime_t Node::getTimeFreq() const
{
    return CTime::getTicksPerSec();
}

void Node::resetTime()
{
    totalTime.nVisit = 0;
    totalTime.tNode = 0;
    totalTime.tTree = 0;
    actionTime.clear();
    objectTime.clear();
}

/// Measure start time
Node::ctime_t Node::startTime() const
{
    if (!getLogTime()) return 0;
    return CTime::getTime();
}

/// Log time spent on an action category and the concerned object
void Node::addTime(ctime_t t, const std::string& s, core::objectmodel::BaseObject* obj)
{
    ObjectTimer& timer = objectTime[s][obj];
    timer.tObject += t;
    ++ timer.nVisit;
}

/// Log time spent given a start time, an action category, and the concerned object
Node::ctime_t Node::endTime(ctime_t t0, const std::string& s, core::objectmodel::BaseObject* obj)
{
    if (!getLogTime()) return 0;
    const ctime_t t1 = CTime::getTime();
    const ctime_t t = t1 - t0;
    addTime(t, s, obj);
    return t1;
}

/// Log time spent on an action category, and the concerned object, plus remove the computed time from the parent caller object
void Node::addTime(ctime_t t, const std::string& s, core::objectmodel::BaseObject* obj, core::objectmodel::BaseObject* /*parent*/)
{
    ObjectTimer& timer = objectTime[s][obj];
    timer.tObject += t;
    ++ timer.nVisit;
    //objectTime[s][parent].tObject -= t;
    cerr<<"Warning: Node::addTime(ctime_t t, const std::string& s, core::objectmodel::BaseObject* obj, core::objectmodel::BaseObject* parent) does not remove the computed time from the parent caller object (parent is ndefined)"<<endl;
}

/// Log time spent given a start time, an action category, and the concerned object
Node::ctime_t Node::endTime(ctime_t /*t0*/, const std::string& /*s*/, core::objectmodel::BaseObject* /*obj*/, core::objectmodel::BaseObject* /*parent*/)
{
    if (!getLogTime()) return 0;
    const ctime_t t1 = CTime::getTime();
    //const ctime_t t = t1 - t0;
    //addTime(t, s, obj, parent);
    cerr<<"Warning: Node::endTime(ctime_t t0, const std::string& s, core::objectmodel::BaseObject* obj, core::objectmodel::BaseObject* parent) does not add parent time (parent is ndefined)"<<endl;
    return t1;
}

Node* Node::setDebug(bool b)
{
    debug_=b;
    return this;
}

bool Node::getDebug() const
{
    return debug_;
}




void Node::removeControllers()
{
    removeObject(masterSolver);
    typedef Sequence<core::componentmodel::behavior::OdeSolver> Solvers;
    Solvers solverRemove = solver;
    for ( Solvers::iterator i=solverRemove.begin(), iend=solverRemove.end(); i!=iend; i++ )
        removeObject( *i );
}


core::objectmodel::BaseContext* Node::getContext()
{
    return _context;
}
const core::objectmodel::BaseContext* Node::getContext() const
{
    return _context;
}

// void Node::setContext( core::objectmodel::BaseContext* c )
// {
//     _context=c;
// 	for( ObjectIterator i=object.begin(), iend=object.end(); i!=iend; i++ )
// 		(*i)->setContext(c);
// }


void Node::setDefaultVisualContextValue()
{
    if (showVisualModels_.getValue() == -1)            showVisualModels_.setValue(true);
    if (showBehaviorModels_.getValue() == -1)          showBehaviorModels_.setValue(false);
    if (showCollisionModels_.getValue() == -1)         showCollisionModels_.setValue(false);
    if (showBoundingCollisionModels_.getValue() == -1) showBoundingCollisionModels_.setValue(false);
    if (showMappings_.getValue() == -1)                showMappings_.setValue(false);
    if (showMechanicalMappings_.getValue() == -1)      showMechanicalMappings_.setValue(false);
    if (showForceFields_.getValue() == -1)             showForceFields_.setValue(false);
    if (showInteractionForceFields_.getValue() == -1)  showInteractionForceFields_.setValue(false);
    if (showWireFrame_.getValue() == -1)               showWireFrame_.setValue(false);
    if (showNormals_.getValue() == -1)                 showNormals_.setValue(false);
}

void Node::initialize()
{
    //cerr<<"Node::initialize()"<<endl;

    initVisualContext();
    sortComponents();
//     // Put the OdeSolver, if any, in first position. This makes sure that the OdeSolver component is initialized only when all its sibling and children components are already initialized.
//     /// @todo Putting the solver first means that it will be initialized *before* any sibling or childrens. Is that what we want? -- Jeremie A.
//     Sequence<BaseObject>::iterator i=object.begin(), iend=object.end();
//     for ( ; i!=iend && dynamic_cast<core::componentmodel::behavior::OdeSolver*>(*i)==NULL; i++ ) // find the OdeSolver
//         {}
//     if ( i!=iend && !object.empty() ) // found
//     {
//         // put it first
//         // BUGFIX 01/12/06 (Jeremie A.): do not modify the order of the other objects
//         // object.swap( i, object.begin() );
//         while (i!=object.begin())
//         {
//             Sequence<BaseObject>::iterator i2 = i;
//             --i;
//             object.swap(i, i2);
//         }
//     }

    //
    updateSimulationContext();

    // this is now done by the InitVisitor
    //for (Sequence<Node>::iterator it = child.begin(); it != child.end(); it++) {
    //    (*it)->init();
    //}
}

void Node::updateContext()
{

    for ( unsigned i=0; i<contextObject.size(); ++i )
    {
        contextObject[i]->init();
        contextObject[i]->apply();
        //cerr<<"Node::updateContext, modified by node = "<<contextObject[i]->getName()<<endl;
    }

    if ( debug_ ) std::cerr<<"Node::updateContext, node = "<<getName()<<", updated context = "<< *static_cast<core::objectmodel::Context*>(this) << endl;
}

void Node::updateSimulationContext()
{
    updateContext();
}

void Node::updateVisualContext(int/* FILTER*/)
{
    // Apply local modifications to the context
    if (getLogTime())
    {
        for ( unsigned i=0; i<contextObject.size(); ++i )
        {
            contextObject[i]->init();
            contextObject[i]->apply();
        }
    }
    else
    {
        for ( unsigned i=0; i<contextObject.size(); ++i )
        {
            contextObject[i]->init();
            contextObject[i]->apply();
        }
    }
    if ( debug_ ) std::cerr<<"Node::updateVisualContext, node = "<<getName()<<", updated context = "<< *static_cast<core::objectmodel::Context*>(this) << endl;
}

/// Execute a recursive action starting from this node
void Node::executeVisitor(Visitor* action)
{
    if (!this->is_activated.getValue()) return;
    if (actionScheduler)
        actionScheduler->executeVisitor(this,action);
    else
        doExecuteVisitor(action);
}

/// Propagate an event
void Node::propagateEvent( core::objectmodel::Event* event )
{
    simulation::PropagateEventVisitor act(event);
    this->executeVisitor(&act);
}





void Node::printComponents()
{
    using namespace sofa::core::componentmodel::behavior;
    using core::BaseMapping;
    using core::componentmodel::topology::Topology;
    using core::componentmodel::topology::BaseTopology;
    using core::componentmodel::topology::BaseMeshTopology;
    using core::Shader;
    using core::BehaviorModel;
    using core::VisualModel;
    using core::CollisionModel;
    using core::objectmodel::ContextObject;
    using core::componentmodel::collision::Pipeline;

    cerr<<"MasterSolver: ";
    for ( Single<MasterSolver>::iterator i=masterSolver.begin(), iend=masterSolver.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"OdeSolver: ";
    for ( Sequence<OdeSolver>::iterator i=solver.begin(), iend=solver.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"LinearSolver: ";
    for ( Sequence<LinearSolver>::iterator i=linearSolver.begin(), iend=linearSolver.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"InteractionForceField: ";
    for ( Sequence<InteractionForceField>::iterator i=interactionForceField.begin(), iend=interactionForceField.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"ForceField: ";
    for ( Sequence<BaseForceField>::iterator i=forceField.begin(), iend=forceField.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"State: ";
    for ( Single<BaseMechanicalState>::iterator i=mechanicalState.begin(), iend=mechanicalState.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"Mechanical Mapping: ";
    for ( Single<BaseMechanicalMapping>::iterator i=mechanicalMapping.begin(), iend=mechanicalMapping.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"Mapping: ";
    for ( Sequence<BaseMapping>::iterator i=mapping.begin(), iend=mapping.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"Topology: ";
    for ( Single<Topology>::iterator i=topology.begin(), iend=topology.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"MeshTopology: ";
    for ( Single<BaseMeshTopology>::iterator i=meshTopology.begin(), iend=meshTopology.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"Shader: ";
    for ( Single<Shader>::iterator i=shader.begin(), iend=shader.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"Constraint: ";
    for ( Sequence<BaseConstraint>::iterator i=constraint.begin(), iend=constraint.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"LMConstraint: ";
    for ( Sequence<BaseLMConstraint>::iterator i=LMConstraint.begin(), iend=LMConstraint.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"BehaviorModel: ";
    for ( Sequence<BehaviorModel>::iterator i=behaviorModel.begin(), iend=behaviorModel.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"VisualModel: ";
    for ( Sequence<VisualModel>::iterator i=visualModel.begin(), iend=visualModel.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"CollisionModel: ";
    for ( Sequence<CollisionModel>::iterator i=collisionModel.begin(), iend=collisionModel.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"ContextObject: ";
    for ( Sequence<ContextObject>::iterator i=contextObject.begin(), iend=contextObject.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"Pipeline: ";
    for ( Single<Pipeline>::iterator i=collisionPipeline.begin(), iend=collisionPipeline.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl<<"VisitorScheduler: ";
    for ( Single<VisitorScheduler>::iterator i=actionScheduler.begin(), iend=actionScheduler.end(); i!=iend; i++ )
        cerr<<(*i)->getName()<<" ";
    cerr<<endl;
}

/** @name Dependency graph
This graph reflects the dependencies between the components. It is used internally to ensure that the initialization order is comform to the dependencies.
*/
/// @{
// Vertices
struct component_t
{
    typedef boost::vertex_property_tag kind;
};
typedef boost::property<component_t, BaseObject*> VertexProperty;

// Graph
typedef ::boost::adjacency_list < ::boost::vecS, ::boost::vecS, ::boost::bidirectionalS, VertexProperty > DependencyGraph;

void Node::sortComponents()
{
    typedef DependencyGraph::vertex_descriptor Vertex;
    DependencyGraph dependencyGraph;
    // map vertex->component
    boost::property_map<DependencyGraph, component_t>::type  component_from_vertex = boost::get( component_t(), dependencyGraph );
    // map component->vertex
    std::map<BaseObject*,Vertex> vertex_from_component;

    // build the graph
    for ( int i=object.size()-1; i>=0; i-- ) // in the reverse order for a final order more similar to the current one
    {
        Vertex v = add_vertex( dependencyGraph );
        component_from_vertex[v] = object[i];
        vertex_from_component[object[i]] = v;
    }
    assert( depend.getValue().size()%2 == 0 ); // must contain only pairs
    for ( unsigned i=0; i<depend.getValue().size(); i+=2 )
    {
        BaseObject* o1 = getObject( depend.getValue()[i] );
        BaseObject* o2 = getObject( depend.getValue()[i+1] );
        if ( o1==NULL ) cerr<<"Node::sortComponent, could not fin object called "<<depend.getValue()[i]<<endl;
        else if ( o2==NULL ) cerr<<"Node::sortComponent, could not fin object called "<<depend.getValue()[i+1]<<endl;
        else
        {
            boost::add_edge( vertex_from_component[o1], vertex_from_component[o2], dependencyGraph );
            //cerr<<"Node::sortComponents, added edge "<<o1->getName()<<" -> "<<o2->getName()<<endl;
        }
    }

    // sort the components according to the dependencies
    typedef std::vector< Vertex > container;
    container c;
    boost::topological_sort(dependencyGraph, std::back_inserter(c));

    // remove all the components
    for ( container::reverse_iterator ii=c.rbegin(); ii!=c.rend(); ++ii)
    {
        removeObject(component_from_vertex[*ii]);
    }

    // put the components in the right order
    //cerr << "Node::sortComponents, New component order: ";
    for ( container::reverse_iterator ii=c.rbegin(); ii!=c.rend(); ++ii)
    {
        addObject(component_from_vertex[*ii]);
        //cerr << component_from_vertex[*ii]->getName() << " ";
    }
    //cerr << endl;
}


}

}


