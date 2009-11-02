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
#include <sofa/simulation/tree/GNode.h>
#include <sofa/simulation/common/Visitor.h>
#include <sofa/simulation/common/xml/NodeElement.h>
#include <sofa/helper/Factory.inl>

namespace sofa
{

namespace simulation
{

namespace tree
{


using helper::system::thread::CTime;

GNode::GNode(const std::string& name, GNode* parent)
    : simulation::Node(name)
{
    if( parent )
        parent->addChild((Node*)this);
}

GNode::~GNode()
{}

/// Add a child node
void GNode::doAddChild(GNode* node)
{
    child.add(node);
    node->parent.add(this);
}

/// Remove a child
void GNode::doRemoveChild(GNode* node)
{
    child.remove(node);
    node->parent.remove(this);
}


/// Add a child node
void GNode::addChild(core::objectmodel::BaseNode* node)
{
    GNode *gnode = static_cast<GNode*>(node);
    notifyAddChild(gnode);
    doAddChild(gnode);
}

/// Remove a child
void GNode::removeChild(core::objectmodel::BaseNode* node)
{
    GNode *gnode = dynamic_cast<GNode*>(node);
    notifyRemoveChild(gnode);
    doRemoveChild(gnode);
}


/// Move a node from another node
void GNode::moveChild(BaseNode* node)
{
    GNode* gnode=dynamic_cast<GNode*>(node);
    if (!gnode) return;
    GNode* prev = gnode->parent;
    if (prev==NULL)
    {
        addChild(node);
    }
    else
    {
        notifyMoveChild(gnode,prev);
        prev->doRemoveChild(gnode);
        doAddChild(gnode);
    }
}


/// Remove a child
void GNode::detachFromGraph()
{
    if (parent)
        parent->removeChild(this);
}

/// Generic object access, possibly searching up or down from the current context
///
/// Note that the template wrapper method should generally be used to have the correct return type,
void* GNode::getObject(const sofa::core::objectmodel::ClassInfo& class_info, const sofa::core::objectmodel::TagSet& tags, SearchDirection dir) const
{
    if (dir == SearchRoot)
    {
        if (parent != NULL) return parent->getObject(class_info, tags, dir);
        else dir = SearchDown; // we are the root, search down from here.
    }
    void *result = NULL;

    for (ObjectIterator it = this->object.begin(); it != this->object.end(); ++it)
    {
        if (tags.empty() || (*it)->getTags().includes(tags))
        {
            result = class_info.dynamicCast(*it);
            if (result != NULL)
                break;
        }
    }

    if (result == NULL)
    {
        switch(dir)
        {
        case Local:
            break;
        case SearchUp:
            if (parent) result = parent->getObject(class_info, tags, dir);
            break;
        case SearchDown:
            for(ChildIterator it = child.begin(); it != child.end(); ++it)
            {
                result = (*it)->getObject(class_info, tags, dir);
                if (result != NULL) break;
            }
            break;
        case SearchRoot:
            std::cerr << "SearchRoot SHOULD NOT BE POSSIBLE HERE!\n";
            break;
        }
    }

    return result;
}

/// Generic object access, given a path from the current context
///
/// Note that the template wrapper method should generally be used to have the correct return type,
void* GNode::getObject(const sofa::core::objectmodel::ClassInfo& class_info, const std::string& path) const
{
    if (path.empty())
    {
        return Node::getObject(class_info, Local);
    }
    else if (path[0] == '/')
    {
        if (parent) return parent->getObject(class_info, path);
        else return getObject(class_info,std::string(path,1));
    }
    else if (std::string(path,0,2)==std::string("./"))
    {
        std::string newpath = std::string(path, 2);
        while (!newpath.empty() && path[0] == '/')
            newpath.erase(0);
        return getObject(class_info,newpath);
    }
    else if (std::string(path,0,3)==std::string("../"))
    {
        std::string newpath = std::string(path, 3);
        while (!newpath.empty() && path[0] == '/')
            newpath.erase(0);
        if (parent) return parent->getObject(class_info,newpath);
        else return getObject(class_info,newpath);
    }
    else
    {
        std::string::size_type pend = path.find('/');
        if (pend == std::string::npos) pend = path.length();
        std::string name ( path, 0, pend );
        Node* child = getChild(name);
        if (child)
        {
            while (pend < path.length() && path[pend] == '/')
                ++pend;
            return child->getObject(class_info, std::string(path, pend));
        }
        else if (pend < path.length())
        {
            std::cerr << "ERROR: child node "<<name<<" not found in "<<getPathName()<<std::endl;
            return NULL;
        }
        else
        {
            core::objectmodel::BaseObject* obj = simulation::Node::getObject(name);
            if (obj == NULL)
            {
                std::cerr << "ERROR: object "<<name<<" not found in "<<getPathName()<<std::endl;
                return NULL;
            }
            else
            {
                void* result = class_info.dynamicCast(obj);
                if (result == NULL)
                {
                    std::cerr << "ERROR: object "<<name<<" in "<<getPathName()<<" does not implement class "<<class_info.name()<<std::endl;
                    return NULL;
                }
                else
                {
                    return result;
                }
            }
        }
    }
}


/// Generic list of objects access, possibly searching up or down from the current context
///
/// Note that the template wrapper method should generally be used to have the correct return type,
void GNode::getObjects(const sofa::core::objectmodel::ClassInfo& class_info, GetObjectsCallBack& container, const sofa::core::objectmodel::TagSet& tags, SearchDirection dir) const
{
    if (dir == SearchRoot)
    {
        if (parent != NULL)
        {
            if (parent->isActive())
            {
                parent->getObjects(class_info, container, tags, dir);
                return;
            }
            else return;
        }
        else dir = SearchDown; // we are the root, search down from here.
    }
    for (ObjectIterator it = this->object.begin(); it != this->object.end(); ++it)
    {
        void* result = class_info.dynamicCast(*it);
        if (result != NULL && (tags.empty() || (*it)->getTags().includes(tags)))
            container(result);
    }

    {
        switch(dir)
        {
        case Local:
            break;
        case SearchUp:
            if (parent) parent->getObjects(class_info, container, tags, dir);
            break;
        case SearchDown:
            for(ChildIterator it = child.begin(); it != child.end(); ++it)
            {
                if ((*it)->isActive())
                    (*it)->getObjects(class_info, container, tags, dir);
            }
            break;
        case SearchRoot:
            std::cerr << "SearchRoot SHOULD NOT BE POSSIBLE HERE!\n";
            break;
        }
    }
}




/// Get parent node (or NULL if no hierarchy or for root node)
core::objectmodel::BaseNode* GNode::getParent()
{
    return parent;
}

/// Get parent node (or NULL if no hierarchy or for root node)
const core::objectmodel::BaseNode* GNode::getParent() const
{
    return parent;
}

/// Test if the given context is an ancestor of this context.
/// An ancestor is a parent or (recursively) the parent of an ancestor.
bool GNode::hasAncestor(const BaseContext* context) const
{
    GNode* p = parent;
    while (p)
    {
        if (p==context) return true;
        p = p->parent;
    }
    return false;
}

/// Execute a recursive action starting from this node
/// This method bypass the actionScheduler of this node if any.
void GNode::doExecuteVisitor(simulation::Visitor* action)
{
#ifdef SOFA_DUMP_VISITOR_INFO
    action->setNode(this);
    action->printInfo(getContext(), true);
#endif
    if (getLogTime())
    {
        const ctime_t t0 = CTime::getTime();
        ctime_t tChild = 0;
        actionStack.push(action);

        if(action->processNodeTopDown(this) != simulation::Visitor::RESULT_PRUNE)
        {
            ctime_t ct0 = CTime::getTime();
            for(ChildIterator it = child.begin(); it != child.end(); ++it)
            {
                (*it)->executeVisitor(action);
            }
            tChild = CTime::getTime() - ct0;
        }
        action->processNodeBottomUp(this);

#ifdef SOFA_DUMP_VISITOR_INFO
        action->printInfo(getContext(), false);
#endif
        actionStack.pop();
        ctime_t tTree = CTime::getTime() - t0;
        ctime_t tNode = tTree - tChild;
        if (actionStack.empty())
        {
            totalTime.tNode += tNode;
            totalTime.tTree += tTree;
            ++totalTime.nVisit;
        }
        NodeTimer& t = actionTime[action->getCategoryName()];
        t.tNode += tNode;
        t.tTree += tTree;
        ++t.nVisit;
        if (!actionStack.empty())
        {
            // remove time from calling action log
            simulation::Visitor* prev = actionStack.top();
            NodeTimer& t = actionTime[prev->getCategoryName()];
            t.tNode -= tTree;
            t.tTree -= tTree;
        }
    }
    else
    {
        if(action->processNodeTopDown(this) != simulation::Visitor::RESULT_PRUNE)
        {
            for(unsigned int i = 0; i<child.size(); ++i)
            {
                child[i]->executeVisitor(action);
            }
        }

        action->processNodeBottomUp(this);
#ifdef SOFA_DUMP_VISITOR_INFO
        action->printInfo(getContext(), false);
#endif
    }
}


/// Return the full path name of this node
std::string GNode::getPathName() const
{
    std::string str;
    if (parent!=NULL) str = parent->getPathName();
    str += '/';
    str += getName();
    return str;
}



void GNode::initVisualContext()
{
    if (getParent() != NULL)
    {
        this->worldGravity_.setDisplayed(false); //only display gravity for the root: it will be propagated at each time step
        if (showVisualModels_.getValue() == -1)
            showVisualModels_.setValue(static_cast<GNode *>(getParent())->showVisualModels_.getValue());
        if (showBehaviorModels_.getValue() == -1)
            showBehaviorModels_.setValue(static_cast<GNode *>(getParent())->showBehaviorModels_.getValue());
        if (showCollisionModels_.getValue() == -1)
            showCollisionModels_.setValue(static_cast<GNode *>(getParent())->showCollisionModels_.getValue());
        if (showBoundingCollisionModels_.getValue() == -1)
            showBoundingCollisionModels_.setValue(static_cast<GNode *>(getParent())->showBoundingCollisionModels_.getValue());
        if (showMappings_.getValue() == -1)
            showMappings_.setValue(static_cast<GNode *>(getParent())->showMappings_.getValue());
        if (showMechanicalMappings_.getValue() == -1)
            showMechanicalMappings_.setValue(static_cast<GNode *>(getParent())->showMechanicalMappings_.getValue());
        if (showForceFields_.getValue() == -1)
            showForceFields_.setValue(static_cast<GNode *>(getParent())->showForceFields_.getValue());
        if (showInteractionForceFields_.getValue() == -1)
            showInteractionForceFields_.setValue(static_cast<GNode *>(getParent())->showInteractionForceFields_.getValue());
        if (showWireFrame_.getValue() == -1)
            showWireFrame_.setValue(static_cast<GNode *>(getParent())->showWireFrame_.getValue());
        if (showNormals_.getValue() == -1)
            showNormals_.setValue(static_cast<GNode *>(getParent())->showNormals_.getValue());
    }
}

void GNode::updateContext()
{
    if ( getParent() != NULL )
    {
        if( debug_ )
        {
            std::cerr<<"GNode::updateContext, node = "<<getName()<<", incoming context = "<< *getParent()->getContext() << std::endl;
        }
        copyContext(*parent);
    }
    simulation::Node::updateContext();
}

void GNode::updateSimulationContext()
{
    if ( getParent() != NULL )
    {
        if( debug_ )
        {
            std::cerr<<"GNode::updateContext, node = "<<getName()<<", incoming context = "<< *getParent()->getContext() << std::endl;
        }
        copySimulationContext(*parent);
    }
    simulation::Node::updateSimulationContext();
}

void GNode::updateVisualContext(VISUAL_FLAG FILTER)
{
    if ( getParent() != NULL )
    {
        switch (FILTER)
        {
        case VISUALMODELS:
            showVisualModels_.setValue((*parent).showVisualModels_.getValue());
            break;
        case BEHAVIORMODELS:
            showBehaviorModels_.setValue((*parent).showBehaviorModels_.getValue());
            break;
        case COLLISIONMODELS:
            showCollisionModels_.setValue((*parent).showCollisionModels_.getValue());
            break;
        case BOUNDINGCOLLISIONMODELS:
            showBoundingCollisionModels_.setValue((*parent).showBoundingCollisionModels_.getValue());
            break;
        case MAPPINGS:
            showMappings_.setValue((*parent).showMappings_.getValue());
            break;
        case MECHANICALMAPPINGS:
            showMechanicalMappings_.setValue((*parent).showMechanicalMappings_.getValue());
            break;
        case FORCEFIELDS:
            showForceFields_.setValue((*parent).showForceFields_.getValue());
            break;
        case INTERACTIONFORCEFIELDS:
            showInteractionForceFields_.setValue((*parent).showInteractionForceFields_.getValue());
            break;
        case WIREFRAME:
            showWireFrame_.setValue((*parent).showWireFrame_.getValue());
            break;
        case NORMALS:
            showNormals_.setValue((*parent).showNormals_.getValue());
            break;
        case ALLFLAGS:
            copyVisualContext(*parent);
            break;
        }
    }
    simulation::Node::updateVisualContext(FILTER);
}

/// Log time spent on an action category, and the concerned object, plus remove the computed time from the parent caller object
void GNode::addTime(ctime_t t, const std::string& s, core::objectmodel::BaseObject* obj, core::objectmodel::BaseObject* parent)
{
    ObjectTimer& timer = objectTime[s][obj];
    timer.tObject += t;
    ++ timer.nVisit;
    objectTime[s][parent].tObject -= t;
}

/// Log time spent given a start time, an action category, and the concerned object
Node::ctime_t GNode::endTime(ctime_t t0, const std::string& s, core::objectmodel::BaseObject* obj, core::objectmodel::BaseObject* parent)
{
    if (!getLogTime()) return 0;
    const ctime_t t1 = CTime::getTime();
    const ctime_t t = t1 - t0;
    addTime(t, s, obj, parent);
    return t1;
}

SOFA_DECL_CLASS(GNode)

//helper::Creator<xml::NodeElement::Factory, GNode> GNodeDefaultClass("default");
helper::Creator<xml::NodeElement::Factory, GNode> GNodeClass("GNode");

} // namespace tree

} // namespace simulation

} // namespace sofa

