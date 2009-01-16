/*
 * PostProcessManager.cpp
 *
 *  Created on: 12 janv. 2009
 *      Author: froy
 */

#include "PostProcessManager.h"
#include <sofa/simulation/common/VisualVisitor.h>
#include <sofa/core/ObjectFactory.h>


namespace sofa
{

namespace component
{

namespace visualmodel
{

SOFA_DECL_CLASS(PostProcessManager)
//Register PostProcessManager in the Object Factory
int PostProcessManagerClass = core::RegisterObject("PostProcessManager")
        .add< PostProcessManager >()
        ;

const std::string PostProcessManager::DEPTH_OF_FIELD_VERTEX_SHADER = "shaders/depthOfField.vert";
const std::string PostProcessManager::DEPTH_OF_FIELD_FRAGMENT_SHADER = "shaders/depthOfField.frag";

PostProcessManager::PostProcessManager()
    :postProcessEnabled (true)
{
    // TODO Auto-generated constructor stub

}

PostProcessManager::~PostProcessManager()
{

}



void PostProcessManager::init()
{
    sofa::core::objectmodel::BaseContext* context = this->getContext();
    dofShader = context->core::objectmodel::BaseContext::get<sofa::component::visualmodel::OglShader>();

    if (!dofShader)
    {
        std::cerr << "PostProcessingManager: OglShader not found ; no post process applied."<< std::endl;
        postProcessEnabled = false;
        return;
    }
}

void PostProcessManager::initVisual()
{
    if (postProcessEnabled)
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        GLint windowWidth = viewport[2];
        GLint windowHeight = viewport[3];

        fbo.init(windowWidth, windowHeight);

        /*dofShader = new OglShader();
        dofShader->vertFilename.setValue(vertFilename.getValue());
        dofShader->fragFilename.setValue(fragFilename.getValue());

        dofShader->init();
        dofShader->initVisual();

        */
        dofShader->setInt(0, "colorTexture", 0);
        dofShader->setInt(0, "depthTexture", 1);
    }
}

void PostProcessManager::preDrawScene(helper::gl::VisualParameters* vp)
{
    if (postProcessEnabled)
    {
        fbo.start();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluPerspective(60.0,1.0, 1, 5000);

        glMatrixMode(GL_MODELVIEW);
        simulation::VisualDrawVisitor vdv( core::VisualModel::Std );
        vdv.execute ( getContext() );
        simulation::VisualDrawVisitor vdvt( core::VisualModel::Transparent );
        vdvt.execute ( getContext() );

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        fbo.stop();
        glViewport(0,0,vp->viewport[2],vp->viewport[3]);
    }
}

bool PostProcessManager::drawScene(helper::gl::VisualParameters* /*vp*/)
{
    if (postProcessEnabled)
    {
        float vxmax, vymax, vzmax ;
        float vxmin, vymin, vzmin ;
        float txmax,tymax,tzmax;
        float txmin,tymin,tzmin;

        txmin = tymin = tzmin = 0.0;
        vxmin = vymin = vzmin = -1.0;
        vxmax = vymax = vzmax = txmax = tymax = tzmax = 1.0;

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, fbo.getColorTexture());

        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, fbo.getDepthTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE);


        dofShader->setInt(0, "colorTexture", 0);
        dofShader->setInt(0, "depthTexture", 1);


        dofShader->start();

        glBegin(GL_QUADS);
        {
            glTexCoord3f(txmin,tymax,0.0); glVertex3f(vxmin,vymax,0.0);
            glTexCoord3f(txmax,tymax,0.0); glVertex3f(vxmax,vymax,0.0);
            glTexCoord3f(txmax,tymin,0.0); glVertex3f(vxmax,vymin,0.0);
            glTexCoord3f(txmin,tymin,0.0); glVertex3f(vxmin,vymin,0.0);
        }
        glEnd();

        dofShader->stop();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        return true;
    }

    return false;
}

void PostProcessManager::postDrawScene(helper::gl::VisualParameters* /*vp*/)
{

}

void PostProcessManager::handleEvent(sofa::core::objectmodel::Event* /*event*/)
{
    /* if (sofa::core::objectmodel::KeypressedEvent* ev = dynamic_cast<sofa::core::objectmodel::KeypressedEvent*>(event))
        {
            switch(ev->getKey())
            {

            }
        }
     */
}

} //visualmodel

} //component

} //sofa
