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

#include <SceneChecking/config.h>
#include <SceneChecking/SceneCheck.h>

#include <map>
#include <sstream>

namespace sofa::_scenechecking_
{
    
class SOFA_SCENECHECKING_API SceneCheckDuplicatedName : public SceneCheck
{
public:
    virtual ~SceneCheckDuplicatedName() {}
    typedef std::shared_ptr<SceneCheckDuplicatedName> SPtr;
    static SPtr newSPtr() { return SPtr(new SceneCheckDuplicatedName()); }
    virtual const std::string getName() override;
    virtual const std::string getDesc() override;
    void doInit(sofa::simulation::Node* node) override;
    void doCheckOn(sofa::simulation::Node* node) override;
    void doPrintSummary(std::string sceneExtension) override;

private:
    bool m_hasDuplicates;
    std::stringstream m_duplicatedMsg;
};

} // namespace sofa::_scenechecking_

namespace sofa::scenechecking
{
    using _scenechecking_::SceneCheckDuplicatedName;
}
