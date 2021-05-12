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
#define SOFA_COMPONENT_COLLISION_DISCRETEINTERSECTION_CPP

#include <SofaBaseCollision/DiscreteIntersection.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/collision/Intersection.inl>

#include <SofaBaseCollision/SphereModel.h>
#include <SofaBaseCollision/BaseIntTool.h>

namespace sofa::core::collision
{
    template class SOFA_SOFABASECOLLISION_API IntersectorFactory<component::collision::DiscreteIntersection>;
} // namespace sofa::core::collision

namespace sofa::component::collision
{

using namespace sofa::defaulttype;
using namespace sofa::core::collision;

int DiscreteIntersectionClass = core::RegisterObject("TODO-DiscreteIntersectionClass")
        .add< DiscreteIntersection >()
        ;

DiscreteIntersection::DiscreteIntersection()
{
    intersectors.add<CubeCollisionModel,       CubeCollisionModel,         DiscreteIntersection> (this);

    intersectors.add<SphereCollisionModel<sofa::defaulttype::Vec3Types>,     SphereCollisionModel<sofa::defaulttype::Vec3Types>,       DiscreteIntersection> (this);

    intersectors.add<RigidSphereModel,RigidSphereModel,DiscreteIntersection>(this);
    intersectors.add<SphereCollisionModel<sofa::defaulttype::Vec3Types>,RigidSphereModel, DiscreteIntersection> (this);

	IntersectorFactory::getInstance()->addIntersectors(this);
}

/// Return the intersector class handling the given pair of collision models, or nullptr if not supported.
ElementIntersector* DiscreteIntersection::findIntersector(core::CollisionModel* object1, core::CollisionModel* object2, bool& swapModels)
{
    return intersectors.get(object1, object2, swapModels);
}

template<>
bool DiscreteIntersection::testIntersection(Cube& cube1, Cube& cube2)
{
    const SReal alarmDist = this->getAlarmDistance();

    if (cube1 == cube2)
    {
        if (cube1.getConeAngle() < M_PI / 2)
            return false;
        else
            return true;
    }

    const defaulttype::Vector3& minVect1 = cube1.minVect();
    const defaulttype::Vector3& minVect2 = cube2.minVect();
    const defaulttype::Vector3& maxVect1 = cube1.maxVect();
    const defaulttype::Vector3& maxVect2 = cube2.maxVect();

    for (int i = 0; i < 3; i++)
    {
        if (minVect1[i] > maxVect2[i] + alarmDist || minVect2[i] > maxVect1[i] + alarmDist)
            return false;
    }

    return true;
}

template<>
int DiscreteIntersection::computeIntersection(Cube& cube1, Cube& cube2, OutputVector* contacts)
{
    SOFA_UNUSED(cube1);
    SOFA_UNUSED(cube2);
    SOFA_UNUSED(contacts);

    return 0;
}

template <> 
bool DiscreteIntersection::testIntersection<Sphere, Sphere>(Sphere& sph1, Sphere& sph2)
{
    return testIntersectionSphere(sph1, sph2, this->getAlarmDistance());
}

template <> 
int DiscreteIntersection::computeIntersection<Sphere, Sphere>(Sphere& sph1, Sphere& sph2, OutputVector* contacts)
{
    return computeIntersectionSphere(sph1, sph2, contacts, this->getAlarmDistance(), this->getContactDistance());
}

template <> 
bool DiscreteIntersection::testIntersection<RigidSphere, RigidSphere>(RigidSphere& sph1, RigidSphere& sph2)
{
    return testIntersectionSphere(sph1, sph2, this->getAlarmDistance());
}

template <> 
int DiscreteIntersection::computeIntersection<RigidSphere, RigidSphere>(RigidSphere& sph1, RigidSphere& sph2, OutputVector* contacts)
{
    return computeIntersectionSphere(sph1, sph2, contacts, this->getAlarmDistance(), this->getContactDistance());
}

template <> 
bool DiscreteIntersection::testIntersection<Sphere, RigidSphere>(Sphere& sph1, RigidSphere& sph2)
{
    return testIntersectionSphere(sph1, sph2, this->getAlarmDistance());
}

template <> 
int DiscreteIntersection::computeIntersection<Sphere, RigidSphere>(Sphere& sph1, RigidSphere& sph2, OutputVector* contacts)
{
    return computeIntersectionSphere(sph1, sph2, contacts, this->getAlarmDistance(), this->getContactDistance());
}

} // namespace sofa::component::collision
