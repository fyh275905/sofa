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
// C++ Interface: LightManager
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef SOFA_COMPONENT_LIGHTMANAGER_H
#define SOFA_COMPONENT_LIGHTMANAGER_H

#include <sofa/defaulttype/SolidTypes.h>
#include <sofa/component/component.h>
#include <sofa/component/visualmodel/Light.h>
#include <sofa/core/VisualManager.h>
#include <sofa/core/objectmodel/Event.h>
#include <sofa/component/visualmodel/OglShadowShader.h>

namespace sofa
{

namespace component
{

namespace visualmodel
{

/**
 *  \brief Utility to manage lights into an Opengl scene
 *
 *  This class must be used with the Light class.
 *  It centralizes all the Lights and managed them.
 *
 */

class SOFA_COMPONENT_VISUALMODEL_API LightManager : public core::VisualManager
{
private:
    static const unsigned int MAX_NUMBER_OF_LIGHTS = GL_MAX_LIGHTS;
    std::vector<Light*> lights;
    bool shadowEnabled;

    OglShadowShader* shadowShader;
    void makeShadowMatrix(unsigned int i);

public:
    Data<bool> debugViewDepthBuffer;
    LightManager();
    virtual ~LightManager();

    void init();
    void reinit();
    void initVisual();
    void update() { };

    void preDrawScene(helper::gl::VisualParameters* vp);
    bool drawScene(helper::gl::VisualParameters* vp);
    void postDrawScene(helper::gl::VisualParameters* vp);


    void draw();
    void fwdDraw(Pass);
    void bwdDraw(Pass);

    ///Register a light into the LightManager
    void putLight(Light* light);

    ///Register a vector of lights into the LightManager
    void putLights(std::vector<Light*> lights);

    ///Remove all lights of the LightManager
    void clear();

    void handleEvent(sofa::core::objectmodel::Event* event);

};

}//namespace visualmodel

}//namespace component

}//namespace sofa

#endif //SOFA_COMPONENT_LIGHT_MANAGER_H
