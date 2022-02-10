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

#include <sofa/gl/component/rendering/config.h>

#include <sofa/core/visual/VisualManager.h>
#include <sofa/core/visual/VisualParams.h>

namespace sofa::gl::component::rendering
{

/**
 *  \brief The utility to enable/disable sRGB rendering
 */

class SOFA_GL_COMPONENT_RENDERING_API OglRenderingSRGB : public core::visual::VisualManager
{
public:
    SOFA_CLASS(OglRenderingSRGB, core::visual::VisualManager);

    void fwdDraw(core::visual::VisualParams* ) override;
    void bwdDraw(core::visual::VisualParams* ) override;
};

} // namespace sofa::gl::component::rendering
