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
#include <SofaMeshCollision/config.h>
#include <sofa/defaulttype/VecTypes.h>

namespace sofa::component::collision
{
template <class DataTypes>
class TTriangle;

/// this class computes if a Triangle P intersects a line segment
class SOFA_SOFAMESHCOLLISION_API RayTriangleIntersection
{
public:
    RayTriangleIntersection(); // start a Proximity solver
    ~RayTriangleIntersection();

    bool NewComputation( const sofa::defaulttype::Vector3 &p1, const sofa::defaulttype::Vector3 &p2, const sofa::defaulttype::Vector3 &p3, const sofa::defaulttype::Vector3 &origin, const sofa::defaulttype::Vector3 &direction,  SReal &t,  SReal &u, SReal &v);
    bool NewComputation(TTriangle<sofa::defaulttype::Vec3Types>* triP, const sofa::defaulttype::Vector3& origin, const sofa::defaulttype::Vector3& direction, SReal& t, SReal& u, SReal& v);
};

} //namespace sofa::component::collision
