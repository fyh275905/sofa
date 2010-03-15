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
// C++ Implementation: LightManager
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <sofa/component/visualmodel/LightManager.h>
#include <sofa/simulation/common/VisualVisitor.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/helper/system/FileRepository.h>

#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>

namespace sofa
{

namespace component
{

namespace visualmodel
{

using namespace helper::gl;
using namespace simulation;

SOFA_DECL_CLASS(LightManager)
//Register LightManager in the Object Factory
int LightManagerClass = core::RegisterObject("LightManager")
        .add< LightManager >()
        ;

LightManager::LightManager()
    :shadowEnabled(initData(&shadowEnabled, (bool) false, "shadowEnabled", "Enable Shadow in the scene"))
    ,ambient(initData(&ambient, defaulttype::Vec4f(0.0f,0.0f,0.0f,0.0f), "ambient", "Ambient lights contribution"))
{

}

LightManager::~LightManager()
{
    restoreDefaultLight();
}

void LightManager::init()
{
    sofa::core::objectmodel::BaseContext* context = this->getContext();
#ifdef SOFA_HAVE_GLEW
    context->get<sofa::component::visualmodel::OglShadowShader, sofa::helper::vector<sofa::component::visualmodel::OglShadowShader*> >(&shadowShaders, core::objectmodel::BaseContext::SearchDown);

    if (shadowShaders.empty())
    {
        serr << "LightManager: No OglShadowShaders found ; shadow will be disabled."<< sendl;
        shadowEnabled.setValue(false);
        return;
    }

    for(unsigned int i=0 ; i<shadowShaders.size() ; i++)
        shadowShaders[i]->initShaders(lights.size());
#endif
    lightModelViewMatrix.resize(lights.size());

}


void LightManager::initVisual()
{
#ifdef SOFA_HAVE_GLEW
    for(unsigned int i=0 ; i<shadowShaders.size() ; i++)
        shadowShaders[i]->initVisual();
#endif

    for (std::vector<Light*>::iterator itl = lights.begin(); itl != lights.end() ; itl++)
    {
        (*itl)->initVisual();
    }
}

void LightManager::putLight(Light* light)
{
    if (lights.size() >= MAX_NUMBER_OF_LIGHTS)
    {
        serr << "The maximum of lights permitted ( "<< MAX_NUMBER_OF_LIGHTS << " ) has been reached." << sendl;
        return ;
    }

    light->setID(lights.size());
    lights.push_back(light) ;
}

void LightManager::putLights(std::vector<Light*> lights)
{
    for (std::vector<Light*>::iterator itl = lights.begin(); itl != lights.end() ; itl++)
        putLight(*itl);
}

void LightManager::makeShadowMatrix(unsigned int i)
{

    const GLfloat* lp = lights[i]->getProjectionMatrix();
    const GLfloat* lmv = lights[i]->getModelviewMatrix();

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(0.5f, 0.5f, 0.5f +( -0.006f) );
    glScalef(0.5f, 0.5f, 0.5f);

    glMultMatrixf(lp); // now multiply by the matrices we have retrieved before
    glMultMatrixf(lmv);
    sofa::defaulttype::Mat<4,4,float> model2;
    glGetFloatv(GL_MODELVIEW_MATRIX,model2.ptr());
    model2.invert(model2);

    glMultMatrixf(model2.ptr());

    if (lightModelViewMatrix.size() > 0)
    {
        lightModelViewMatrix[i] = lmv;
        lightProjectionMatrix[i] = lp;
    }
    else
    {
        lightModelViewMatrix.resize(lights.size());
        lightProjectionMatrix.resize(lights.size());
        lightModelViewMatrix[i] = lmv;
        lightProjectionMatrix[i] = lp;
    }
    //std::cout << "lightModelViewMatrix[i] "<<i << " -> " << lightModelViewMatrix[i] << std::endl;
    //std::cout << "lightProjectionMatrix[i] "<<i << " -> " << lightProjectionMatrix[i] << std::endl;

    glMatrixMode(GL_MODELVIEW);


    /*
       sofa::defaulttype::Mat4x4f m;
       m.identity();
       m[0][3] = 0.5f;
       m[1][3] = 0.5f;
       m[2][3] = 0.5f +( -0.006f);
       m[0][0] = 0.5f;
       m[1][1] = 0.5f;
       m[2][2] = 0.5f;

       sofa::defaulttype::Mat4x4f lightProj(lp); //lightProj.transpose();
       sofa::defaulttype::Mat4x4f lightModelView(lmv);// lightModelView.transpose();

       sofa::defaulttype::Mat4x4f model;
       glGetFloatv(GL_MODELVIEW_MATRIX,model.ptr());
       //model.transpose();
       sofa::defaulttype::Mat4x4f modelInv;
       modelInv.invert(model);

       m = m * lightProj * lightModelView * modelInv;
       //m.transpose();
       //std::cout << "Computed " << modelInv << std::endl;


       if (lightModelViewMatrix.size() > 0)
       {
    	   lightModelViewMatrix[i] = m;
       }
       else
       {
    	   lightModelViewMatrix.resize(lights.size());
    	   lightModelViewMatrix[i] = m;
       }
    */
}

void LightManager::fwdDraw(Pass)
{

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient.getValue().ptr());
    unsigned int id = 0;
    for (std::vector<Light*>::iterator itl = lights.begin(); itl != lights.end() ; itl++)
    {
        glEnable(GL_LIGHT0+id);
        (*itl)->drawLight();
        id++;
    }

#ifdef SOFA_HAVE_GLEW
    GLint lightFlag[MAX_NUMBER_OF_LIGHTS];
    GLint shadowTextureID[MAX_NUMBER_OF_LIGHTS];
    GLfloat lightModelViewProjectionMatrices[MAX_NUMBER_OF_LIGHTS*16];

    if (!shadowShaders.empty())
    {
        glEnable(GL_LIGHTING);
        for (unsigned int i=0 ; i < lights.size() ; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, lights[i]->getShadowTexture());

            lightFlag[i] = 1;
            shadowTextureID[i] = 0;

            if (shadowEnabled.getValue() && lights[i]->enableShadow.getValue())
            {
                lightFlag[i] = 2;
                shadowTextureID[i] = i;
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            makeShadowMatrix(i);
        }

        for (unsigned int i = lights.size() ; i< MAX_NUMBER_OF_LIGHTS ; i++)
        {
            lightFlag[i] = 0;
            shadowTextureID[i] = 0;

            for(unsigned int j=0 ; j<4; j++)
                for(unsigned int k=0 ; k<4; k++)
                    lightModelViewProjectionMatrices[16*i+j*4+k] = 0.0;
        }

        for(unsigned int i=0 ; i<shadowShaders.size() ; i++)
        {
            shadowShaders[i]->setIntVector(shadowShaders[i]->getCurrentIndex() , "lightFlag" , MAX_NUMBER_OF_LIGHTS, lightFlag);
            shadowShaders[i]->setIntVector(shadowShaders[i]->getCurrentIndex() , "shadowTexture" , MAX_NUMBER_OF_LIGHTS, shadowTextureID);
            shadowShaders[i]->setMatrix4(shadowShaders[i]->getCurrentIndex() , "lightModelViewMatrix" , MAX_NUMBER_OF_LIGHTS, false, (lightModelViewMatrix[0].ptr()));
            shadowShaders[i]->setMatrix4(shadowShaders[i]->getCurrentIndex() , "lightProjectionMatrix" , MAX_NUMBER_OF_LIGHTS, false, (lightModelViewMatrix[0].ptr()));
            //shadowShader->start();
        }

    }
#endif
}

void LightManager::bwdDraw(Pass)
{
#ifdef SOFA_HAVE_GLEW
    if (shadowEnabled.getValue())
        for(unsigned int i=0 ; i<shadowShaders.size() ; i++)
        {
            //shadowShaders[i]->stop();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
#endif

    for (unsigned int i=0 ; i<MAX_NUMBER_OF_LIGHTS ; i++)
        glDisable(GL_LIGHT0+i);

}

void LightManager::drawVisual()
{

}

void LightManager::clear()
{
    for (unsigned int i=0 ; i<MAX_NUMBER_OF_LIGHTS ; i++)
        glDisable(GL_LIGHT0+i);
    lights.clear();
}

void LightManager::reinit()
{
    for (std::vector<Light*>::iterator itl = lights.begin(); itl != lights.end() ; itl++)
    {
        (*itl)->reinit();
    }
}

void LightManager::preDrawScene(VisualParameters* vp)
{

#ifdef SOFA_HAVE_GLEW
    for (std::vector<Light*>::iterator itl = lights.begin(); itl != lights.end() ; itl++)
    {
        if(shadowEnabled.getValue())
        {
            (*itl)->preDrawShadow(vp);

            simulation::VisualDrawVisitor vdv( core::VisualModel::Std );

            vdv.execute ( getContext() );
        }
    }

    for (std::vector<Light*>::iterator itl = lights.begin(); itl != lights.end() ; itl++)
    {
        if(shadowEnabled.getValue())
        {
            (*itl)->postDrawShadow();
        }
    }
    //restore viewport
    glViewport(0, 0, vp->viewport[2] , vp->viewport[3]);
#endif
}

bool LightManager::drawScene(VisualParameters* /*vp*/)
{
    return false;
}

void LightManager::postDrawScene(VisualParameters* /*vp*/)
{
    restoreDefaultLight();
}

void LightManager::restoreDefaultLight()
{
    //restore default light
    GLfloat	ambientLight[4];
    GLfloat	diffuseLight[4];
    GLfloat	specular[4];
    GLfloat	lightPosition[4];

    lightPosition[0] = -0.7f;
    lightPosition[1] = 0.3f;
    lightPosition[2] = 0.0f;
    lightPosition[3] = 1.0f;

    ambientLight[0] = 0.5f;
    ambientLight[1] = 0.5f;
    ambientLight[2] = 0.5f;
    ambientLight[3] = 1.0f;

    diffuseLight[0] = 0.9f;
    diffuseLight[1] = 0.9f;
    diffuseLight[2] = 0.9f;
    diffuseLight[3] = 1.0f;

    specular[0] = 1.0f;
    specular[1] = 1.0f;
    specular[2] = 1.0f;
    specular[3] = 1.0f;

    // Setup 'light 0'
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);

    glEnable(GL_LIGHT0);
}

void LightManager::handleEvent(sofa::core::objectmodel::Event* event)
{
    if (sofa::core::objectmodel::KeypressedEvent* ev = dynamic_cast<sofa::core::objectmodel::KeypressedEvent*>(event))
    {
        switch(ev->getKey())
        {

        case 'l':
        case 'L':
#ifdef SOFA_HAVE_GLEW
            if (!shadowShaders.empty())
            {
                bool b = shadowEnabled.getValue();
                shadowEnabled.setValue(!b);
                std::cout << "Shadows : "<<(shadowEnabled.getValue()?"ENABLED":"DISABLED")<<std::endl;
            }
#endif
            break;
        }
    }

}

}//namespace visualmodel

}//namespace component

}//namespace sofa
