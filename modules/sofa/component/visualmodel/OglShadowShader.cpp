/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
//
// C++ Implementation: Shader
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <sofa/component/visualmodel/OglShadowShader.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/helper/system/FileRepository.h>

#include <sofa/component/visualmodel/LightManager.h>

namespace sofa
{

namespace component
{

namespace visualmodel
{


SOFA_DECL_CLASS(OglShadowShader)

//Register OglShader in the Object Factory
int OglShadowShaderClass = core::RegisterObject("OglShadowShader")
        .add< OglShadowShader >()
        ;

#ifndef PCSS_SHADOWS
const std::string OglShadowShader::PATH_TO_SHADOW_VERTEX_SHADERS = "shaders/shadowMapping.vert";
const std::string OglShadowShader::PATH_TO_SHADOW_FRAGMENT_SHADERS = "shaders/shadowMapping.frag";
#else
const std::string OglShadowShader::PATH_TO_SHADOW_VERTEX_SHADERS = "shaders/shadowMappingPCSS.vert";
const std::string OglShadowShader::PATH_TO_SHADOW_FRAGMENT_SHADERS = "shaders/shadowMappingPCSS.frag";
#endif

OglShadowShader::OglShadowShader()
    :test(initData(&test, (int) 0, "test", "test"))
{


}

OglShadowShader::~OglShadowShader()
{

}

void OglShadowShader::init()
{
    passive.setValue(false);
    turnOn.setValue(true);
}

void OglShadowShader::initShaders(unsigned int /* numberOfLights */)
{
    std::string tempFragment="";
    std::string tempVertex="";

    vertexFilenames.push_back( PATH_TO_SHADOW_VERTEX_SHADERS );
    fragmentFilenames.push_back( PATH_TO_SHADOW_FRAGMENT_SHADERS );
    shaderVector.push_back(new sofa::helper::gl::GLSLShader());

    std::ostringstream oss;
    oss << LightManager::MAX_NUMBER_OF_LIGHTS;

    this->addDefineMacro(0,std::string("MAX_NUMBER_OF_LIGHTS"), oss.str());

}

}//namespace visualmodel

} //namespace component

} //namespace sofa
