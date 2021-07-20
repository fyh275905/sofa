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

#include <SofaGeneralMeshCollision/config.h>

#include <sofa/core/collision/Intersection.h>

#include <SofaMeshCollision/LineModel.h>
#include <SofaMeshCollision/TriangleModel.h>
#include <SofaBaseCollision/SphereModel.h>

namespace sofa::component::collision
{

class DiscreteIntersection;

class SOFA_SOFAGENERALMESHCOLLISION_API MeshDiscreteIntersection : public core::collision::BaseIntersector
{
    typedef core::collision::BaseIntersector::OutputVector OutputVector;

public:
    MeshDiscreteIntersection(DiscreteIntersection* object, bool addSelf=true);

    bool testIntersection(Triangle&, Line&);
    template<class T> bool testIntersection(TSphere<T>&, Triangle&);

    int computeIntersection(Triangle& e1, Line& e2, OutputVector* contacts);
    template<class T> int computeIntersection(TSphere<T>&, Triangle&, OutputVector*);

protected:
    DiscreteIntersection* intersection;

};

} // namespace sofa::component::collision
