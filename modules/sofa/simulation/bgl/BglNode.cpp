/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc., 51  *
* Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
*******************************************************************************
*                            SOFA :: Applications                             *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
//
// C++ Implementation: BglNode
//
// Description:
//
//
// Author: Francois Faure in The SOFA team </www.sofa-framework.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <sofa/simulation/bgl/BglNode.h>
#include <sofa/simulation/bgl/GetObjectsVisitor.h>
#include <sofa/simulation/bgl/BglGraphManager.inl>
#include <sofa/simulation/common/xml/NodeElement.h>

//Components of the core to detect during the addition of objects in a node
#include <sofa/core/componentmodel/behavior/InteractionForceField.h>
#include <sofa/core/componentmodel/behavior/InteractionConstraint.h>


#include <boost/vector_property_map.hpp>
#include <sofa/simulation/bgl/dfv_adapter.h>
#include <sofa/helper/Factory.inl>

#define BREADTH_FIRST_VISIT

namespace sofa
{
namespace simulation
{
namespace bgl
{

unsigned int BglNode::uniqueId=0;
std::deque<unsigned int> BglNode::freeId;

BglNode::BglNode(const std::string& name)
    : sofa::simulation::Node(name)
{
    id=getUniqueId();
    BglGraphManager::getInstance()->addVertex(this);
}

BglNode::~BglNode()
{
    BglGraphManager::getInstance()->removeVertex(this);
    freeId.push_back(id);
}

unsigned int BglNode::getUniqueId()
{
    if (freeId.empty())
        return uniqueId++;
    else
    {
        int unique=freeId.front();
        freeId.pop_front();
        return unique;
    }
}


bool BglNode::addObject(core::objectmodel::BaseObject* obj)
{
    using sofa::core::objectmodel::Tag;
    if (sofa::core::componentmodel::behavior::BaseMechanicalMapping* mm = dynamic_cast<sofa::core::componentmodel::behavior::BaseMechanicalMapping*>(obj))
    {
        sofa::core::componentmodel::behavior::BaseMechanicalState
        *msFrom=mm->getMechFrom(),
         *msTo  =mm->getMechTo();

        if (msFrom && msTo)
        {
            Node *from=(Node*)msFrom->getContext();
            Node *to=(Node*)  msTo  ->getContext();
            BglGraphManager::getInstance()->addInteraction( from, to, mm);
        }
    }
    else if (sofa::core::componentmodel::behavior::InteractionForceField* iff = dynamic_cast<sofa::core::componentmodel::behavior::InteractionForceField*>(obj))
    {
        sofa::core::componentmodel::behavior::BaseMechanicalState
        *ms1=iff->getMechModel1(),
         *ms2=iff->getMechModel2();

        if (ms1 && ms2)
        {
            Node *m1=(Node*)ms1->getContext();
            Node *m2=(Node*)ms2->getContext();
            if (m1!=m2) BglGraphManager::getInstance()->addInteraction( m1, m2, iff);
        }
    }
    else if (sofa::core::componentmodel::behavior::InteractionConstraint* ic = dynamic_cast<sofa::core::componentmodel::behavior::InteractionConstraint*>(obj))
    {
        sofa::core::componentmodel::behavior::BaseMechanicalState
        *ms1=ic->getMechModel1(),
         *ms2=ic->getMechModel2();

        if (ms1 && ms2)
        {
            Node *m1=(Node*)ms1->getContext();
            Node *m2=(Node*)ms2->getContext();
            if (m1!=m2) BglGraphManager::getInstance()->addInteraction( m1, m2, ic);
        }
    }
    return Node::addObject(obj);
}

bool BglNode::removeObject(core::objectmodel::BaseObject* obj)
{
    if (sofa::core::componentmodel::behavior::BaseMechanicalMapping* mm = dynamic_cast<sofa::core::componentmodel::behavior::BaseMechanicalMapping*>(obj))
    {
        BglGraphManager::getInstance()->removeInteraction(mm);
    }
    else if (sofa::core::componentmodel::behavior::InteractionForceField* iff = dynamic_cast<sofa::core::componentmodel::behavior::InteractionForceField*>(obj))
    {
        BglGraphManager::getInstance()->removeInteraction(iff);
    }
    else if (sofa::core::componentmodel::behavior::InteractionConstraint* ic = dynamic_cast<sofa::core::componentmodel::behavior::InteractionConstraint*>(obj))
    {
        BglGraphManager::getInstance()->removeInteraction(ic);
    }
    return Node::removeObject(obj);
}

void BglNode::addParent(BglNode *node)
{
    parents.add(node);
}
void BglNode::removeParent(BglNode *node)
{
    parents.remove(node);
}


void BglNode::addChild(core::objectmodel::BaseNode* c)
{
    BglNode *childNode = static_cast< BglNode *>(c);

    notifyAddChild(childNode);
    doAddChild(childNode);
}
/// Add a child node
void BglNode::doAddChild(BglNode* node)
{
    child.add(node);
    node->addParent(this);
    BglGraphManager::getInstance()->addEdge(this,node);
}


/// Remove a child
void BglNode::removeChild(core::objectmodel::BaseNode* c)
{
    BglNode *childNode = static_cast< BglNode *>(c);
    notifyRemoveChild(childNode);
    doRemoveChild(childNode);
}

void BglNode::doRemoveChild(BglNode* node)
{
    child.remove(node);
    node->removeParent(this);
    BglGraphManager::getInstance()->removeEdge(this, node);
}


void BglNode::moveChild(core::objectmodel::BaseNode* node)
{
    BglNode* childNode=dynamic_cast<BglNode*>(node);
    if (!childNode) return;

    typedef std::vector< BglNode*> ParentsContainer;
    ParentsContainer nodeParents; childNode->getParents(nodeParents);
    if (nodeParents.empty())
    {
        addChild(node);
    }
    else
    {
        for (ParentsContainer::iterator it = nodeParents.begin(); it != nodeParents.end(); ++it)
        {
            BglNode *prev = (*it);
            notifyMoveChild(childNode,prev);
            prev->doRemoveChild(childNode);
        }
        doAddChild(childNode);
    }
}

void BglNode::detachFromGraph()
{
    Sequence<BglNode>::iterator it=parents.begin(), it_end=parents.end();
    for (; it!=it_end; ++it) (*it)->removeChild(this);
}


/// Test if the given context is a parent of this context.
bool BglNode::hasParent(const BaseContext* context) const
{
    if (context == NULL) return parents.empty();
    for (Sequence<BglNode>::iterator it=parents.begin(), it_end=parents.end(); it!=it_end; ++it)
    {
        BglNode* p = *it;
        if (p==context) return true;
    }
    return false;
}

/// Test if the given context is an ancestor of this context.
/// An ancestor is a parent or (recursively) the parent of an ancestor.
bool BglNode::hasAncestor(const BaseContext* context) const
{
    for (Sequence<BglNode>::iterator it=parents.begin(), it_end=parents.end(); it!=it_end; ++it)
    {
        BglNode* p = *it;
        if (p==context) return true;
        if (p->hasAncestor(context)) return true;
    }
    return false;
}


std::string BglNode::getPathName() const
{
    std::string str;
    if (!parents.empty())
        str = (*parents.begin())->getPathName();
    str += '/';
    str += getName();
    return str;

}


void BglNode::doExecuteVisitor( Visitor* visit )
{
#ifdef BREADTH_FIRST_VISIT
    BglGraphManager::getInstance()->breadthFirstVisit(this, *visit, SearchDown);
#else
    BglGraphManager::getInstance()->depthFirstVisit(this, *visit, SearchDown);
#endif
}



/// Generic object access, possibly searching up or down from the current context
///
/// Note that the template wrapper method should generally be used to have the correct return type,
void* BglNode::getObject(const sofa::core::objectmodel::ClassInfo& class_info, const sofa::core::objectmodel::TagSet& tags, SearchDirection dir) const
{
    GetObjectVisitor getobj(class_info);
    getobj.setTags(tags);
    if (dir == Local)
    {
        for (ObjectIterator it = this->object.begin(); it != this->object.end(); ++it)
        {
            void* result = class_info.dynamicCast(*it);
            if (result != NULL && (tags.empty() || (*it)->getTags().includes(tags)))
            {
                return result;
            }
        }
    }
    else
    {
#ifdef BREADTH_FIRST_VISIT
        BglGraphManager::getInstance()->breadthFirstVisit(this, getobj, dir);
#else
        BglGraphManager::getInstance()->depthFirstVisit(this, getobj, dir);
#endif
    }
    return getobj.getObject();
}


/// Generic object access, possibly searching up or down from the current context
///
/// Note that the template wrapper method should generally be used to have the correct return type,
void BglNode::getObjects(const sofa::core::objectmodel::ClassInfo& class_info, GetObjectsCallBack& container, const sofa::core::objectmodel::TagSet& tags, SearchDirection dir) const
{
    GetObjectsVisitor getobjs(class_info, container);
    getobjs.setTags(tags);
    if (dir == Local)
    {
        for (ObjectIterator it = this->object.begin(); it != this->object.end(); ++it)
        {
            void* result = class_info.dynamicCast(*it);
            if (result != NULL && (tags.empty() || (*it)->getTags().includes(tags)))
                container(result);
        }
    }
    else
    {
#ifdef BREADTH_FIRST_VISIT
        BglGraphManager::getInstance()->breadthFirstVisit(this, getobjs, dir);
#else
        BglGraphManager::getInstance()->depthFirstVisit(this, getobjs, dir);
#endif
    }
}



/// Generic object access, given a path from the current context
///
/// Note that the template wrapper method should generally be used to have the correct return type,
void* BglNode::getObject(const sofa::core::objectmodel::ClassInfo& class_info, const std::string& path) const
{

    if (path.empty())
    {
        return Node::getObject(class_info, Local);
    }
    else if (path[0] == '/')
    {
        if (!parents.empty())
        {
            for (Parents::iterator it=parents.begin(); it!=parents.end(); ++it)
            {
                void *result=(*it)->getObject(class_info, path);
                if (result) return result;
            }
            return NULL;
        }
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

        if (!parents.empty())
        {
            for (Parents::iterator it=parents.begin(); it!=parents.end(); ++it)
            {
                void *result=(*it)->getObject(class_info, newpath);
                if (result) return result;
            }
            return NULL;
        }
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



void BglNode::updateContext()
{
    if (!parents.empty())
    {
        copyContext(*(*parents.begin()));
    }
    simulation::Node::updateContext();
}

void BglNode::updateSimulationContext()
{
    if (!parents.empty())
    {
        copySimulationContext(*(*parents.begin()));
    }
    simulation::Node::updateSimulationContext();
}


void BglNode::initVisualContext()
{
    if (!parents.empty())
    {
        this->worldGravity_.setDisplayed(false); //only display gravity for the root: it will be propagated at each time step
        if (showVisualModels_.getValue() == -1)
        {
            showVisualModels_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showVisualModels_.setValue(showVisualModels_.getValue()!=0 || (*it)->showVisualModels_.getValue()!=0);
        }
        if (showBehaviorModels_.getValue() == -1)
        {
            showBehaviorModels_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showBehaviorModels_.setValue(showBehaviorModels_.getValue()==1 || (*it)->showBehaviorModels_.getValue()==1);
        }
        if (showCollisionModels_.getValue()== -1)
        {
            showCollisionModels_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showCollisionModels_.setValue(showCollisionModels_.getValue()==1 || (*it)->showCollisionModels_.getValue()==1);
        }
        if (showBoundingCollisionModels_.getValue()== -1)
        {
            showBoundingCollisionModels_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showBoundingCollisionModels_.setValue(showBoundingCollisionModels_.getValue()==1 || (*it)->showBoundingCollisionModels_.getValue()==1);
        }
        if (showMappings_.getValue()== -1)
        {
            showMappings_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showMappings_.setValue(showMappings_.getValue()==1 || (*it)->showMappings_.getValue()==1);
        }
        if (showMechanicalMappings_.getValue()== -1)
        {
            showMechanicalMappings_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showMechanicalMappings_.setValue(showMechanicalMappings_.getValue()==1 || (*it)->showMechanicalMappings_.getValue()==1);
        }
        if (showForceFields_.getValue()== -1)
        {
            showForceFields_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showForceFields_.setValue(showForceFields_.getValue()==1 || (*it)->showForceFields_.getValue()==1);
        }
        if (showInteractionForceFields_.getValue()== -1)
        {
            showInteractionForceFields_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showInteractionForceFields_.setValue(showInteractionForceFields_.getValue()==1 || (*it)->showInteractionForceFields_.getValue()==1);
        }
        if (showWireFrame_.getValue()== -1)
        {
            showWireFrame_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showWireFrame_.setValue(showWireFrame_.getValue()==1 || (*it)->showWireFrame_.getValue()==1);
        }
        if (showNormals_.getValue()== -1)
        {
            showNormals_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showNormals_.setValue(showNormals_.getValue()==1 || (*it)->showNormals_.getValue()==1);
        }
    }
}

void BglNode::updateVisualContext(VISUAL_FLAG FILTER)
{
    if (!parents.empty())
    {
        switch (FILTER)
        {
        case VISUALMODELS:
            showVisualModels_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showVisualModels_.setValue( (*it)->showVisualModels_.getValue());
            break;
        case BEHAVIORMODELS:
            showBehaviorModels_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showBehaviorModels_.setValue( (*it)->showBehaviorModels_.getValue());
            break;
        case COLLISIONMODELS:
            showCollisionModels_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showCollisionModels_.setValue( (*it)->showCollisionModels_.getValue());
            break;
        case BOUNDINGCOLLISIONMODELS:
            showBoundingCollisionModels_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showBoundingCollisionModels_.setValue( (*it)->showBoundingCollisionModels_.getValue());
            break;
        case MAPPINGS:
            showMappings_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showMappings_.setValue( (*it)->showMappings_.getValue());
            break;
        case MECHANICALMAPPINGS:
            showMechanicalMappings_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showMechanicalMappings_.setValue( (*it)->showMechanicalMappings_.getValue());
            break;
        case FORCEFIELDS:
            showForceFields_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showForceFields_.setValue( (*it)->showForceFields_.getValue());
            break;
        case INTERACTIONFORCEFIELDS:
            showInteractionForceFields_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showInteractionForceFields_.setValue( (*it)->showInteractionForceFields_.getValue());
            break;
        case WIREFRAME:
            showWireFrame_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showWireFrame_.setValue( (*it)->showWireFrame_.getValue());
            break;
        case NORMALS:
            showNormals_.setValue(0);
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
                showNormals_.setValue( (*it)->showNormals_.getValue());
            break;
        case ALLFLAGS:
            for (ParentIterator it=parents.begin(); it!=parents.end(); ++it)
            {
                fusionVisualContext(*(*it));
            }
            break;
        }
    }
    simulation::Node::updateVisualContext(FILTER);
}

SOFA_DECL_CLASS(BglNode)

helper::Creator<simulation::xml::NodeElement::Factory, BglNode> BglNodeClass("BglNode");
}
}
}


