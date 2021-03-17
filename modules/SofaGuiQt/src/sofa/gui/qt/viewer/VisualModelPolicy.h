/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program. If not, see <http://www.gnu.org/licenses/>.              *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <sofa/gui/qt/config.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa::gui::qt::viewer
{

class SOFA_SOFAGUIQT_API VisualModelPolicy
{
public:
	VisualModelPolicy(core::visual::VisualParams* vparams = core::visual::visualparams::defaultInstance());
	virtual ~VisualModelPolicy();
    virtual void load() = 0;
    virtual void unload() = 0;
protected:
    sofa::core::visual::VisualParams* vparams;

};

} // namespace sofa::gui::qt::viewer
