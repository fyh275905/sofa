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
#ifndef SOFA_CORE_COMPONENTMODEL_COLLISION_PIPELINE_H
#define SOFA_CORE_COMPONENTMODEL_COLLISION_PIPELINE_H

#include <sofa/core/CollisionModel.h>
#include <sofa/core/CollisionElement.h>
#include <sofa/core/componentmodel/collision/Intersection.h>
#include <sofa/core/componentmodel/collision/BroadPhaseDetection.h>
#include <sofa/core/componentmodel/collision/NarrowPhaseDetection.h>
#include <sofa/core/componentmodel/collision/DetectionOutput.h>
#include <sofa/core/componentmodel/collision/ContactManager.h>
#include <sofa/core/componentmodel/collision/CollisionGroupManager.h>

#include <vector>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace collision
{

class Pipeline : public virtual sofa::core::objectmodel::BaseObject
{
protected:
    //sofa::helper::vector<DetectionOutput*> detectionOutputs;

    sofa::helper::vector<Intersection*> intersectionMethods;
    sofa::helper::vector<BroadPhaseDetection*> broadPhaseDetections;
    sofa::helper::vector<NarrowPhaseDetection*> narrowPhaseDetections;
    sofa::helper::vector<ContactManager*> contactManagers;
    sofa::helper::vector<CollisionGroupManager*> groupManagers;

    Intersection* intersectionMethod;
    BroadPhaseDetection* broadPhaseDetection;
    NarrowPhaseDetection* narrowPhaseDetection;
    ContactManager* contactManager;
    CollisionGroupManager* groupManager;

public:
    typedef NarrowPhaseDetection::DetectionOutputMap DetectionOutputMap;

    Pipeline();

    virtual ~Pipeline();

    virtual void reset()=0;

    /// Remove collision response from last step
    virtual void computeCollisionReset()=0;
    /// Detect new collisions. Note that this step must not modify the simulation graph
    virtual void computeCollisionDetection()=0;
    /// Add collision response in the simulation graph
    virtual void computeCollisionResponse()=0;

    void computeCollisions()
    {
        computeCollisionReset();
        computeCollisionDetection();
        computeCollisionResponse();
    }

    //sofa::helper::vector<DetectionOutput*>& getDetectionOutputs() { return detectionOutputs; }

protected:
    /// Remove collision response from last step
    virtual void doCollisionReset() = 0;
    /// Detect new collisions. Note that this step must not modify the simulation graph
    virtual void doCollisionDetection(const sofa::helper::vector<core::CollisionModel*>& collisionModels) = 0;
    /// Add collision response in the simulation graph
    virtual void doCollisionResponse() = 0;
};

} // namespace collision

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
