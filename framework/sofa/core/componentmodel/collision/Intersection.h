/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_CORE_COMPONENTMODEL_COLLISION_INTERSECTION_H
#define SOFA_CORE_COMPONENTMODEL_COLLISION_INTERSECTION_H

#include <sofa/core/CollisionModel.h>
#include <sofa/core/componentmodel/collision/DetectionOutput.h>
#include <sofa/helper/FnDispatcher.h>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace collision
{

class ElementIntersector
{
protected:
    virtual ~ElementIntersector() {}
public:

    /// Test if 2 elements can collide. Note that this can be conservative (i.e. return true even when no collision is present)
    virtual bool canIntersect(core::CollisionElementIterator elem1, core::CollisionElementIterator elem2) = 0;

    /// Begin intersection tests between two collision models. Return the number of contacts written in the contacts vector.
    /// If the given contacts vector is NULL, then this method should allocate it.
    virtual int beginIntersect(core::CollisionModel* model1, core::CollisionModel* model2, DetectionOutputVector*& contacts) = 0;

    /// Compute the intersection between 2 elements. Return the number of contacts written in the contacts vector.
    virtual int intersect(core::CollisionElementIterator elem1, core::CollisionElementIterator elem2, DetectionOutputVector* contacts) = 0;

    /// End intersection tests between two collision models. Return the number of contacts written in the contacts vector.
    virtual int endIntersect(core::CollisionModel* model1, core::CollisionModel* model2, DetectionOutputVector* contacts) = 0;

    virtual std::string name() const = 0;
};

/// Table storing associations between types of collision models and intersectors implementing intersection tests
///
/// This class uses the new ClassInfo metaclass to be able to recognize derived classes. So it is no longer necessary
/// to register all derived collision models (i.e. an intersector registered for RayModel will also be used for RayPickIntersector).
class IntersectorMap : public std::map< std::pair< helper::TypeInfo, helper::TypeInfo >, ElementIntersector* >
{
public:

    template<class Model1, class Model2, class T>
    void add(T* ptr);

    template<class Model1, class Model2>
    void ignore();

    helper::TypeInfo getType(core::CollisionModel* model);

    ElementIntersector* get(core::CollisionModel* model1, core::CollisionModel* model2, bool& swapModels);

protected:
    std::map< helper::TypeInfo, helper::TypeInfo > castMap;
    std::set< const objectmodel::ClassInfo* > classes;
};

class Intersection : public virtual objectmodel::BaseObject
{
public:

    virtual ~Intersection();

    /// Return the intersector class handling the given pair of collision models, or NULL if not supported.
    /// @param swapModel output value set to true if the collision models must be swapped before calling the intersector.
    virtual ElementIntersector* findIntersector(core::CollisionModel* object1, core::CollisionModel* object2, bool& swapModels) = 0;

    /// Test if intersection between 2 types of elements is supported, i.e. an intersection test is implemented for this combinaison of types.
    /// Note that this method is deprecated in favor of findIntersector
    virtual bool isSupported(core::CollisionElementIterator elem1, core::CollisionElementIterator elem2);

    /// returns true if algorithm uses proximity detection
    virtual bool useProximity() const { return false; }

    /// returns true if algorithm uses continous detection
    virtual bool useContinuous() const { return false; }

    /// Return the alarm distance (must return 0 if useProximity() is false)
    virtual double getAlarmDistance() const { return 0.0; }

    /// Return the contact distance (must return 0 if useProximity() is false)
    virtual double getContactDistance() const { return 0.0; }

};

} // namespace collision

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
