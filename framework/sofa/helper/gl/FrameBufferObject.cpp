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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
/*
 * FrameBufferObject.cpp
 *
 *  Created on: 6 janv. 2009
 *      Author: froy
 */

#include <sofa/helper/gl/FrameBufferObject.h>

namespace sofa
{
namespace helper
{
namespace gl
{

FrameBufferObject::FrameBufferObject()
    :width(0)
    ,height(0)
    ,depthTexture(0)
    ,initialized(false)
{

}

FrameBufferObject::~FrameBufferObject()
{

}

void FrameBufferObject::init(unsigned int width, unsigned height)
{
    if (!initialized)
    {
        this->width = width;
        this->height = height;

        glGenFramebuffersEXT(1, &id);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id);
        createDepthBuffer();
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);
        createColorBuffer();
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorTexture, 0);

        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);

        //debug
        //if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT)
        //	std::cout << "FBO OK" << std::endl;




        /*
        switch(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT))
        {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
        	std::cout << "Status: Framebuffer Initialisation OK"
        	<< std::endl;
        	break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        	std::cout << "Error: Framebuffer Configuration"
        	<< " Unsupported" << std::endl;
        	break;
        default:
        	std::cout << "Error: Unknown Framebuffer"
        	<< " Configuration Error" << std::endl;
        	break;
        }
        */

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


        initialized=true;
    }

}

void FrameBufferObject::start()
{
    if (initialized)
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id);

    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

}

void FrameBufferObject::stop()
{

    if (initialized)
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
}

GLuint FrameBufferObject::getDepthTexture()
{
    return depthTexture;
}

GLuint FrameBufferObject::getColorTexture()
{
    return colorTexture;
}

void FrameBufferObject::setSize(unsigned int width, unsigned height)
{
    if (initialized)
    {
        this->width = width;
        this->height = height;

        //glGenFramebuffersEXT(1, &id);
        initDepthBuffer();
        //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);
        initColorBuffer();
        //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorTexture, 0);
        //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    }
}

void FrameBufferObject::createDepthBuffer()
{
    //Depth Texture
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &depthTexture);
    initDepthBuffer();
}

void FrameBufferObject::createColorBuffer()
{
    //Color Texture
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &colorTexture);
    initColorBuffer();
}

void FrameBufferObject::initDepthBuffer()
{
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBufferObject::initColorBuffer()
{
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

}



} //gl

} //helper

} //sofa
