/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
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
#pragma once
#include <SofaBaseCollision/config.h>

#include <SofaBaseCollision/BaseProximityIntersection.h>

namespace sofa::component::collision
{

/**
 * It uses proximities on cubes and spheres, but supported pairs of collision models can
 * be extended. For example, see MeshMinProximityIntersection which adds support for
 * Point/Point, Line/Point, etc.
 *
 * Supported by default:
 * - Cube/Cube
 * - Sphere/Sphere (rigid or vec3)
 * MeshMinProximityIntersection adds support for:
 * - Point/Point
 * - Line/Point
 * - Line/Line
 * - Triangle/Point
 * - Triangle/Line
 * - Triangle/Triangle
 * - Sphere/Point
 * - RigidSphere/Point
 * - Line/RigidSphere
 * - Line/Sphere
 * - Triangle/RigidSphere
 * - Triangle/Sphere
 */
class SOFA_SOFABASECOLLISION_API NewProximityIntersection : public BaseProximityIntersection
{
public:
    SOFA_CLASS(NewProximityIntersection,BaseProximityIntersection);

    Data<bool> useLineLine; ///< Line-line collision detection enabled

    typedef core::collision::IntersectorFactory<NewProximityIntersection> IntersectorFactory;

    void init() override;

    static inline int
    doIntersectionPointPoint(SReal dist2, const type::Vector3& p, const type::Vector3& q,
                             OutputVector* contacts, int id);

    bool testIntersection(Cube& cube1, Cube& cube2) override;
    int computeIntersection(Cube& cube1, Cube& cube2, OutputVector* contacts) override;

    template<typename SphereType1, typename SphereType2>
    bool testIntersection(SphereType1& sph1, SphereType2& sph2);
    template<typename SphereType1, typename SphereType2>
    int computeIntersection(SphereType1& sph1, SphereType2& sph2, OutputVector* contacts);

protected:
    NewProximityIntersection();

};

} // namespace sofa::component::collision

namespace sofa::core::collision
{
#if  !defined(SOFA_COMPONENT_COLLISION_NEWPROXIMITYINTERSECTION_CPP)
extern template class SOFA_SOFABASECOLLISION_API IntersectorFactory<component::collision::NewProximityIntersection>;
#endif

} // namespace sofa::core::collision
