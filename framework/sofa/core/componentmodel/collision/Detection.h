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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_COMPONENTMODEL_COLLISION_DETECTION_H
#define SOFA_CORE_COMPONENTMODEL_COLLISION_DETECTION_H

#include <sofa/core/CollisionModel.h>
#include <sofa/core/componentmodel/collision/CollisionAlgorithm.h>
#include <sofa/core/componentmodel/collision/Intersection.h>
#include <vector>
#include <map>
#include <algorithm>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace collision
{

class Detection : public virtual CollisionAlgorithm
{
protected:
    /// Current intersection method
    Intersection* intersectionMethod;
    /// All intersection methods
    std::map<Instance,Intersection*> storedIntersectionMethod;
public:

    Detection()
        : intersectionMethod(NULL)
    {
    }

    /// virtual because subclasses might do precomputations based on intersection algorithms
    virtual void setIntersectionMethod(Intersection* v) { intersectionMethod = v;    }
    Intersection* getIntersectionMethod() const         { return intersectionMethod; }

protected:
    virtual void changeInstanceBP(Instance) {}
    virtual void changeInstanceNP(Instance) {}
    virtual void changeInstance(Instance inst)
    {
        storedIntersectionMethod[instance] = intersectionMethod;
        intersectionMethod = storedIntersectionMethod[inst];
        // callback overriden by BroadPhaseDetection
        changeInstanceBP(inst);
        // callback overriden by NarrowPhaseDetection
        changeInstanceNP(inst);
    }
};

} // namespace collision

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
