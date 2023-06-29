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
#include <string>
#include "sofa/helper/StringUtils.h"
#include <sofa/core/config.h>
#include <sofa/core/PathResolver.h>
#include <sofa/core/objectmodel/BaseObjectDescription.h>

namespace sofa::core
{
SOFA_CORE_API std::string getTemplateFromMechanicalState(sofa::core::objectmodel::BaseContext* context, sofa::core::objectmodel::BaseObjectDescription*);
SOFA_CORE_API std::string getTemplateFromMeshTopology(sofa::core::objectmodel::BaseContext* context, sofa::core::objectmodel::BaseObjectDescription*);

/// Deduce the object template from the object pointed by the linkName, if not set, use the provided defaultValue
template<class TargetObject>
std::string getTemplateFromLink(const std::string& attributeName, const std::string defaultLinkPath, sofa::core::objectmodel::BaseContext* context, sofa::core::objectmodel::BaseObjectDescription* arg)
{
    // get the template type from the object pointed by the linkPath
    std::string linkedPath = arg->getAttribute(attributeName, defaultLinkPath.c_str());
    if(linkedPath.empty())
        return "";

    linkedPath = sofa::helper::split(linkedPath, ' ')[0];
    auto base = sofa::core::PathResolver::FindBaseFromClassAndPath(context, TargetObject::GetClass(), linkedPath);
    if(base!=nullptr)
        return base->getTemplateName();

    return "";
}

/// Deduce the object template from the object pointed by the linkName, then search in the current context for a MechanicalState
SOFA_CORE_API std::string getTemplateFromLinkedMechanicalState(const std::string& attributeName, const std::string& defaultValue,  sofa::core::objectmodel::BaseContext* context, sofa::core::objectmodel::BaseObjectDescription* arg);
SOFA_CORE_API std::string getTemplateFromLinkedMechanicalState(const std::string& attributeName, sofa::core::objectmodel::BaseContext* context, sofa::core::objectmodel::BaseObjectDescription*);

/// Deduce the object template from the object pointed by the linkName, then search in the current context for a BaseMeshTopology
SOFA_CORE_API std::string getTemplateFromLinkedBaseMeshTopology(const std::string& attributeName, sofa::core::objectmodel::BaseContext* context, sofa::core::objectmodel::BaseObjectDescription*);
}
