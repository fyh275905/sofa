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
#ifndef SOFA_COMPONENT_COLLISION_OCTREEDETECTION_H
#define SOFA_COMPONENT_COLLISION_OCTREEDETECTION_H

#include <sofa/core/componentmodel/collision/BroadPhaseDetection.h>
#include <sofa/core/componentmodel/collision/NarrowPhaseDetection.h>
#include <sofa/core/CollisionElement.h>
#include <sofa/defaulttype/Vec.h>
#include <set>


namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;

/**
 *  \brief It is a Ray Trace based collision detection algorithm
 *
 *   For each point in one object, we trace a ray following de oposite of the point's normal
 *    up to find a triangle in the other object. Both triangles are tested to evaluate if they are in
 * colliding state. It must be used with a TriangleOctreeModel,as an octree is used to traverse the object.
 */
class SOFA_COMPONENT_COLLISION_API RayTraceDetection :public core::componentmodel::collision::
    BroadPhaseDetection,
public core::componentmodel::collision::NarrowPhaseDetection
{
private:
    sofa::helper::vector < core::CollisionModel * >collisionModels;
    Data < bool > bDraw;

public:
    typedef sofa::helper::vector<sofa::core::componentmodel::collision::DetectionOutput>    OutputVector;
    RayTraceDetection ();

    void setDraw (bool val)
    {
        bDraw.setValue (val);
    }
    void selfCollision (TriangleOctreeModel * cm1);
    void addCollisionModel (core::CollisionModel * cm);
    void addCollisionPair (const std::pair < core::CollisionModel *,
            core::CollisionModel * >&cmPair);

    void findPairsVolume (CubeModel * cm1,
            CubeModel * cm2);

    virtual void beginBroadPhase()
    {
        core::componentmodel::collision::BroadPhaseDetection::beginBroadPhase();
        collisionModels.clear();
    }

    void draw ();
};

}				// namespace collision

}				// namespace component

}				// namespace sofa

#endif
