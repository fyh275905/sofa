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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_COMPONENTMODEL_COLLISION_CONTACTMANAGER_H
#define SOFA_CORE_COMPONENTMODEL_COLLISION_CONTACTMANAGER_H

#include <sofa/core/componentmodel/collision/CollisionAlgorithm.h>
#include <sofa/core/componentmodel/collision/Contact.h>
#include <sofa/core/componentmodel/collision/NarrowPhaseDetection.h>

#include <vector>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace collision
{
/**
 * @brief Given a set of detected contact points, create contact response components
 */

class ContactManager : public virtual CollisionAlgorithm
{
public:
    typedef NarrowPhaseDetection::DetectionOutputMap DetectionOutputMap;
    typedef sofa::helper::vector<Contact*> ContactVector;

    /// Constructor
    ContactManager()
        : intersectionMethod(NULL)
    {
    }

    /// Destructor
    virtual ~ContactManager() { }

    virtual void createContacts(DetectionOutputMap& outputs) = 0;

    virtual const ContactVector& getContacts() { return contacts; }

    /// virtual because subclasses might do precomputations based on intersection algorithms
    virtual void setIntersectionMethod(Intersection* v) { intersectionMethod = v;    }
    Intersection* getIntersectionMethod() const         { return intersectionMethod; }

protected:
    /// Current intersection method
    Intersection* intersectionMethod;

    ContactVector contacts;


    /// All intersection methods
    std::map<Instance,Intersection*> storedIntersectionMethod;

    std::map<Instance,ContactVector> storedContacts;

    virtual void changeInstance(Instance inst)
    {
        storedIntersectionMethod[instance] = intersectionMethod;
        intersectionMethod = storedIntersectionMethod[inst];
        storedContacts[instance].swap(contacts);
        contacts.swap(storedContacts[inst]);
    }
};

} // namespace collision

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
