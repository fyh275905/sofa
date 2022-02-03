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

#include <sofa/component/sceneutility/config.h>

#include <sofa/component/sceneutility/PauseAnimation.h>

namespace sofa::component::sceneutility
{

/**
*/
class SOFA_COMPONENT_SCENEUTILITY_API PauseAnimationOnEvent : public PauseAnimation
{
public:
    SOFA_CLASS(PauseAnimationOnEvent,PauseAnimation);
protected:
    PauseAnimationOnEvent();

    ~PauseAnimationOnEvent() override;
public:
    void init() override;

    bool paused;
    bool isPaused() override;

    void handleEvent(sofa::core::objectmodel::Event* event) override;

};


} // namespace sofa::component::sceneutility
