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

#include <SofaBaseCollision/BruteForceBroadPhase.h>
#include <SofaBaseCollision/BVHNarrowPhase.h>
#include <sofa/core/NameHelper.h>

namespace sofa::component::collision
{

class SOFA_SOFABASECOLLISION_API BruteForceDetection final : public sofa::core::objectmodel::BaseObject
{
public:
    SOFA_CLASS(BruteForceDetection, sofa::core::objectmodel::BaseObject);

    void init() override;

    /// Construction method called by ObjectFactory.
    template<class T>
    static typename T::SPtr create(T*, sofa::core::objectmodel::BaseContext* context, sofa::core::objectmodel::BaseObjectDescription* arg)
    {
        BruteForceBroadPhase::SPtr broadPhase = sofa::core::objectmodel::New<BruteForceBroadPhase>();
        broadPhase->setName(sofa::core::NameHelper::getInstance().resolveName(broadPhase->getClassName(), {}));
        if (context) context->addObject(broadPhase);

        BVHNarrowPhase::SPtr narrowPhase = sofa::core::objectmodel::New<BVHNarrowPhase>();
        narrowPhase->setName(sofa::core::NameHelper::getInstance().resolveName(narrowPhase->getClassName(), {}));
        if (context) context->addObject(narrowPhase);

        typename T::SPtr obj = sofa::core::objectmodel::New<T>();
        if (context) context->addObject(obj);
        if (arg) obj->parse(arg);

        return obj;
    }

protected:
    BruteForceDetection() = default;
    ~BruteForceDetection() override = default;

};

} // namespace sofa::component::collision
