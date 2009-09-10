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
// C++ Interface: BglNode
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SOFA_SIMULATION_BGL_BGLNODE_H
#define SOFA_SIMULATION_BGL_BGLNODE_H

#include "BglGraphManager.h"
#include "BglSimulation.h"
#include <sofa/simulation/common/Node.h>
#include <sofa/simulation/tree/xml/NodeElement.h>
#include <deque>

namespace sofa
{
namespace simulation
{
namespace bgl
{

using sofa::core::objectmodel::BaseObject;

using sofa::simulation::Node;
/**
   sofa::simulation::Node as a node of a BGL scene graph.


   @author Francois Faure in The SOFA team </www.sofa-framework.org>
*/

class BglNode : public sofa::simulation::Node
{
public:
    typedef Sequence< BglNode > Parents;
    typedef Parents::iterator ParentIterator;
    Parents parents;


    BglNode(const std::string& name="");
    /**
       \param sg the SOFA scene containing a bgl graph
       \param n the node of the bgl graph corresponding to this
    */
    ~BglNode();


    /// Add a child node
    void addChild(core::objectmodel::BaseNode* node);

    /// Remove a child node
    void removeChild(core::objectmodel::BaseNode* node);

    /// Move a node from another node
    void moveChild(core::objectmodel::BaseNode* obj);


    /// Add an object and return this. Detect the implemented interfaces and add the object to the corresponding lists.
    bool addObject(BaseObject* obj);

    /// Remove an object
    bool removeObject(BaseObject* obj);


    /// Remove the current node from the graph: consists in removing the link to all the parents
    void detachFromGraph() ;





    /// Find all the parents of the node, and fill them inside the container passed as argument
    template <typename Container>
    void getParents(Container &data) const
    {
        std::copy(parents.begin(),parents.end(), std::back_inserter<Container>(data));
    }

    std::string getPathName() const;

    void addParent(BglNode *node);

    void removeParent(BglNode *node);




    /** Perform a scene graph traversal with the given Visitor, starting from this node.
        Visitor::processNodetopdown is applied on discover, and Visitor::processNodeBottomUp is applied on finish.
    */
    void doExecuteVisitor( Visitor* action);

    /// Generic object access, given a set of required tags, possibly searching up or down from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void* getObject(const sofa::core::objectmodel::ClassInfo& class_info, const sofa::core::objectmodel::TagSet& tags, SearchDirection dir = SearchUp) const;

    /// Generic object access, given a path from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void* getObject(const sofa::core::objectmodel::ClassInfo& class_info, const std::string& path) const;

    /// Generic list of objects access, given a set of required tags, possibly searching up or down from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void getObjects(const sofa::core::objectmodel::ClassInfo& class_info, GetObjectsCallBack& container, const sofa::core::objectmodel::TagSet& tags, SearchDirection dir = SearchUp) const;



    /// Called during initialization to corectly propagate the visual context to the children
    virtual void initVisualContext();

    /// Update the whole context values, based on parent and local ContextObjects
    virtual void updateContext();

    /// Update the visual context values, based on parent and local ContextObjects
    virtual void updateVisualContext(VISUAL_FLAG FILTER=ALLFLAGS);

    /// Update the simulation context values(gravity, time...), based on parent and local ContextObjects
    virtual void updateSimulationContext();




    static void create(BglNode*& obj, simulation::tree::xml::Element<core::objectmodel::BaseNode>* arg)
    {
        obj = new BglNode();
        obj->parse(arg);
    }




    //A Node is a property of a vertex of the boost graph. Each vertex must have a unique identifier we are calling id.
    unsigned int getId() const {return id;};
    static unsigned int uniqueId;
protected:
    //Id of the Node in the Boost graph
    unsigned int id;
    //Provide a unique Id
    static unsigned int getUniqueId();
    //Stack of Ids that has been freed by previously deleted BglNodes
    static std::deque<unsigned int> freeId;




    virtual void doAddChild(BglNode* node);
    void doRemoveChild(BglNode* node);
    // VisitorScheduler can use doExecuteVisitor() method
    friend class simulation::VisitorScheduler;
};
}
}
}

#endif
