/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#include <sofa/simulation/common/VisualVisitor.h>

namespace sofa
{

namespace simulation
{


Visitor::Result VisualDrawVisitor::processNodeTopDown(simulation::Node* node)
{
    glPushMatrix();
    double glMatrix[16];
    node->getPositionInWorld().writeOpenGlMatrix(glMatrix);
    glMultMatrixd( glMatrix );

    hasShader = (node->getShader()!=NULL);

    for_each(this, node, node->visualModel,     &VisualDrawVisitor::fwdVisualModel);
    this->VisualVisitor::processNodeTopDown(node);

    glPopMatrix();
    return RESULT_CONTINUE;
}

void VisualDrawVisitor::processNodeBottomUp(simulation::Node* node)
{
    for_each(this, node, node->visualModel,     &VisualDrawVisitor::bwdVisualModel);
}

void VisualDrawVisitor::processObject(simulation::Node* /*node*/, core::objectmodel::BaseObject* o)
{
    if (pass == core::VisualModel::Std || pass == core::VisualModel::Shadow)
        o->draw();
}

void VisualDrawVisitor::fwdVisualModel(simulation::Node* /*node*/, core::VisualModel* vm)
{
    vm->fwdDraw(pass);
}

void VisualDrawVisitor::bwdVisualModel(simulation::Node* /*node*/, core::VisualModel* vm)
{
    vm->bwdDraw(pass);
}

void VisualDrawVisitor::processVisualModel(simulation::Node* node, core::VisualModel* vm)
{
    //cerr<<"VisualDrawVisitor::processVisualModel "<<vm->getName()<<endl;
    sofa::core::Shader* shader = NULL;
    if (hasShader)
        shader = dynamic_cast<sofa::core::Shader*>(node->getShader());

    switch(pass)
    {
    case core::VisualModel::Std:
    {
        if (shader && shader->isActive())
            shader->start();
        vm->drawVisual();
        if (shader && shader->isActive())
            shader->stop();
        break;
    }
    case core::VisualModel::Transparent:
    {
        if (shader && shader->isActive())
            shader->start();
        vm->drawTransparent();
        if (shader && shader->isActive())
            shader->stop();
        break;
    }
    case core::VisualModel::Shadow:
        vm->drawShadow();
        break;
    }
}

void VisualUpdateVisitor::processVisualModel(simulation::Node*, core::VisualModel* vm)
{
    vm->updateVisual();
}

void VisualInitVisitor::processVisualModel(simulation::Node*, core::VisualModel* vm)
{
    vm->initVisual();
}

VisualComputeBBoxVisitor::VisualComputeBBoxVisitor()
{
    minBBox[0] = minBBox[1] = minBBox[2] = 1e10;
    maxBBox[0] = maxBBox[1] = maxBBox[2] = -1e10;
}

void VisualComputeBBoxVisitor::processMechanicalState(simulation::Node*, core::componentmodel::behavior::BaseMechanicalState* vm)
{
    vm->addBBox(minBBox, maxBBox);
}
void VisualComputeBBoxVisitor::processVisualModel(simulation::Node*, core::VisualModel* vm)
{
    vm->addBBox(minBBox, maxBBox);
}

} // namespace simulation

} // namespace sofa

