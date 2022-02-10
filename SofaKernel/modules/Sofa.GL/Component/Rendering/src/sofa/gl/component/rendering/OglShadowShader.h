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
#ifndef SOFA_COMPONENT_OGLSHADOWSHADER
#define SOFA_COMPONENT_OGLSHADOWSHADER
#include <sofa/gl/component/rendering/config.h>

#include <sofa/gl/component/rendering/OglShader.h>

namespace sofa
{

namespace component
{

namespace visualmodel
{

class SOFA_GL_COMPONENT_RENDERING_API OglShadowShader : public sofa::component::visualmodel::OglShader
{
public:
    SOFA_CLASS(OglShadowShader, sofa::component::visualmodel::OglShader);
protected:
    OglShadowShader();
    ~OglShadowShader() override;
public:
    void init() override;

    virtual void initShaders(unsigned int numberOfLights, bool softShadow);

protected:
    static const std::string PATH_TO_SHADOW_VERTEX_SHADERS;
    static const std::string PATH_TO_SHADOW_FRAGMENT_SHADERS;
    static const std::string PATH_TO_SOFT_SHADOW_VERTEX_SHADERS;
    static const std::string PATH_TO_SOFT_SHADOW_FRAGMENT_SHADERS;

};

}//namespace visualmodel

} //namespace component

} //namespace sofa

#endif //SOFA_COMPONENT_OGLSHADOWSHADER
