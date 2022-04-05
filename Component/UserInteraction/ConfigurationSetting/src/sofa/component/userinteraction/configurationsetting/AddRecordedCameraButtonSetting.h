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

#include <sofa/component/userinteraction/configurationsetting/config.h>

#include <sofa/core/objectmodel/ConfigurationSetting.h>
#include <sofa/component/userinteraction/configurationsetting/MouseButtonSetting.h>

namespace sofa::component::userinteraction::configurationsetting
{

class SOFA_COMPONENT_USERINTERACTION_CONFIGURATIONSETTING_API AddRecordedCameraButtonSetting: public MouseButtonSetting
{
public:
    SOFA_CLASS(AddRecordedCameraButtonSetting,MouseButtonSetting);
protected:
    AddRecordedCameraButtonSetting(){};
public:
    std::string getOperationType() override {return "Add recorded camera's position and orientation";}

};

class SOFA_COMPONENT_USERINTERACTION_CONFIGURATIONSETTING_API StartNavigationButtonSetting: public MouseButtonSetting
{
public:
    SOFA_CLASS(StartNavigationButtonSetting,MouseButtonSetting);
protected:
    StartNavigationButtonSetting(){};
public:
    std::string getOperationType() override {return "Start navigation if some view poins have been saved";}
};

} // namespace sofa::component::userinteraction::configurationsetting
