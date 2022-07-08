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
#include <sofa/gl/component/rendering3d/OglSceneFrame.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/gl/gl.h>

namespace sofa::gl::component::rendering3d
{

int OglSceneFrameClass = core::RegisterObject("Display a frame at the corner of the scene view")
        .add< OglSceneFrame >()
        ;

using namespace sofa::defaulttype;

OglSceneFrame::OglSceneFrame()
    : d_drawFrame(initData(&d_drawFrame, true,  "draw", "Display the frame or not"))
    , d_style(initData(&d_style, "style", "Style of the frame"))
    , d_alignment(initData(&d_alignment, "alignment", "Alignment of the frame in the view"))
    , d_viewportSize(initData(&d_viewportSize, 150, "viewportSize", "Size of the viewport where the frame is rendered"))
{
    sofa::helper::OptionsGroup styleOptions(3,"Arrows", "Cylinders", "CubeCones");
    styleOptions.setSelectedItem(1);
    d_style.setValue(styleOptions);

    sofa::helper::OptionsGroup alignmentOptions(4,"BottomLeft", "BottomRight", "TopRight", "TopLeft");
    alignmentOptions.setSelectedItem(1);
    d_alignment.setValue(alignmentOptions);
}

void OglSceneFrame::init()
{
    Inherit1::init();
    updateVisual();
}

void OglSceneFrame::reinit()
{
    updateVisual();
}

void OglSceneFrame::drawArrows(const core::visual::VisualParams* vparams)
{
    //X axis
    vparams->drawTool()->drawArrow(
        sofa::core::visual::DrawTool::Vector3(),
        sofa::core::visual::DrawTool::Vector3(1, 0, 0),
        0.05f,
        sofa::core::visual::DrawTool::RGBAColor(1.0f, 0.0f, 0.0f, 1.0f));

    //Y axis
    vparams->drawTool()->drawArrow(
        sofa::core::visual::DrawTool::Vector3(),
        sofa::core::visual::DrawTool::Vector3(0, 1, 0),
        0.05f,
        sofa::core::visual::DrawTool::RGBAColor(0.0f, 1.0f, 0.0f, 1.0f)
    );

    //Z axis
    vparams->drawTool()->drawArrow(
        sofa::core::visual::DrawTool::Vector3(),
        sofa::core::visual::DrawTool::Vector3(0, 0, 1),
        0.05f,
        sofa::core::visual::DrawTool::RGBAColor(0.0f, 0.0f, 1.0f, 1.0f)
    );
}

void OglSceneFrame::drawCylinders(const core::visual::VisualParams* vparams)
{
    //X axis
    vparams->drawTool()->drawCylinder(
        sofa::core::visual::DrawTool::Vector3(),
        sofa::core::visual::DrawTool::Vector3(1, 0, 0),
        0.05f,
        sofa::core::visual::DrawTool::RGBAColor::red()
    );

    //Y axis
    vparams->drawTool()->drawCylinder(
        sofa::core::visual::DrawTool::Vector3(),
        sofa::core::visual::DrawTool::Vector3(0, 1, 0),
        0.05f,
        sofa::core::visual::DrawTool::RGBAColor::green()
    );

    //Z axis
    vparams->drawTool()->drawCylinder(
        sofa::core::visual::DrawTool::Vector3(),
        sofa::core::visual::DrawTool::Vector3(0, 0, 1),
        0.05f,
        sofa::core::visual::DrawTool::RGBAColor::blue()
    );
}

void OglSceneFrame::drawCubeCones(const core::visual::VisualParams* vparams)
{
    constexpr float s = 0.25f;

    vparams->drawTool()->drawHexahedron(
        sofa::core::visual::DrawTool::Vector3(-s, -s, -s),
        sofa::core::visual::DrawTool::Vector3(s, -s, -s),
        sofa::core::visual::DrawTool::Vector3(s, s, -s),
        sofa::core::visual::DrawTool::Vector3(-s, s, -s),
        sofa::core::visual::DrawTool::Vector3(-s, -s, s),
        sofa::core::visual::DrawTool::Vector3(s, -s, s),
        sofa::core::visual::DrawTool::Vector3(s, s, s),
        sofa::core::visual::DrawTool::Vector3(-s, s, s),
        sofa::core::visual::DrawTool::RGBAColor::darkgray());

    //X axis
    vparams->drawTool()->drawCone(
        sofa::core::visual::DrawTool::Vector3(s, 0, 0),
        sofa::core::visual::DrawTool::Vector3(s * 3.f, 0, 0),
        0, s,
        sofa::core::visual::DrawTool::RGBAColor::red());
    vparams->drawTool()->drawCone(
        sofa::core::visual::DrawTool::Vector3(-s, 0, 0),
        sofa::core::visual::DrawTool::Vector3(-s * 3.f, 0, 0),
        0, s,
        sofa::core::visual::DrawTool::RGBAColor::gray());

    //Y axis
    vparams->drawTool()->drawCone(
        sofa::core::visual::DrawTool::Vector3(0, s, 0),
        sofa::core::visual::DrawTool::Vector3(0, s * 3.f, 0),
        0, s,
        sofa::core::visual::DrawTool::RGBAColor::green());
    vparams->drawTool()->drawCone(
        sofa::core::visual::DrawTool::Vector3(0, -s, 0),
        sofa::core::visual::DrawTool::Vector3(0, -s * 3.f, 0),
        0, s,
        sofa::core::visual::DrawTool::RGBAColor::gray());

    //Z axis
    vparams->drawTool()->drawCone(
        sofa::core::visual::DrawTool::Vector3(0, 0, s),
        sofa::core::visual::DrawTool::Vector3(0, 0, s * 3.f),
        0, s,
        sofa::core::visual::DrawTool::RGBAColor::blue());
    vparams->drawTool()->drawCone(
        sofa::core::visual::DrawTool::Vector3(0, 0, -s),
        sofa::core::visual::DrawTool::Vector3(0, 0, -s * 3.f),
        0, s,
        sofa::core::visual::DrawTool::RGBAColor::gray());
}

void OglSceneFrame::draw(const core::visual::VisualParams* vparams)
{
    if (!d_drawFrame.getValue()) return;

    vparams->drawTool()->saveLastState();

    const Viewport& viewport = vparams->viewport();

    const auto viewportSize = d_viewportSize.getValue();

    switch(d_alignment.getValue().getSelectedId())
    {
        case 0: //BottomLeft
        default:
            glViewport(0,0,viewportSize,viewportSize);
            glScissor(0,0,viewportSize,viewportSize);
            break;
        case 1: //BottomRight
            glViewport(viewport[2]-viewportSize,0,viewportSize,viewportSize);
            glScissor(viewport[2]-viewportSize,0,viewportSize,viewportSize);
            break;
        case 2: //TopRight
            glViewport(viewport[2]-viewportSize,viewport[3]-viewportSize,viewportSize,viewportSize);
            glScissor(viewport[2]-viewportSize,viewport[3]-viewportSize,viewportSize,viewportSize);
            break;
        case 3: //TopLeft
            glViewport(0,viewport[3]-viewportSize,viewportSize,viewportSize);
            glScissor(0,viewport[3]-viewportSize,viewportSize,viewportSize);
            break;
    }


    glEnable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor (1.0f, 1.0f, 1.0f, 0.0f);

    glMatrixMode(GL_PROJECTION);
    vparams->drawTool()->pushMatrix();
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 0.5, 10.0);

    GLdouble matrix[16];
    vparams->getModelViewMatrix(matrix);

    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = -3;
    matrix[15] = 1;

    glMatrixMode(GL_MODELVIEW);
    vparams->drawTool()->pushMatrix();
    glLoadMatrixd(matrix);

    vparams->drawTool()->disableLighting();

    switch (d_style.getValue().getSelectedId())
    {
    case 0:
    default:
        drawArrows(vparams);
        break;

    case 1:
        drawCylinders(vparams);
        break;

    case 2:
        drawCubeCones(vparams);
        break;
    }

    glMatrixMode(GL_PROJECTION);
    vparams->drawTool()->popMatrix();
    glMatrixMode(GL_MODELVIEW);
    vparams->drawTool()->popMatrix();

    vparams->drawTool()->restoreLastState();
    glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);

}

} // namespace sofa::gl::component::rendering3d
