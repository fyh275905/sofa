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
//
// C++ Implementation: ArticulatedHierarchyBVHController
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <sofa/component/controller/ArticulatedHierarchyBVHController.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace controller
{

void ArticulatedHierarchyBVHController::init()
{
    sofa::simulation::tree::GNode* curNode = dynamic_cast<sofa::simulation::tree::GNode*>(this->getContext());
    if (curNode)
    {
        curNode->getTreeObjects<ArticulatedHierarchyContainer::ArticulationCenter, ArtCenterVec >(&m_artCenterVec);
        curNode->getTreeObject(ahc);
        frame = 0;
        n=0;
    }
}

void ArticulatedHierarchyBVHController::reset()
{
    frame = 0;
    n=0;
}

void ArticulatedHierarchyBVHController::applyController(void)
{
    double ndiv = 1.0/ahc->dtbvh;
    int frameInc = 1;
    if (ndiv < 1.0)
        frameInc = int(ahc->numOfFrames/(ahc->numOfFrames/ahc->dtbvh));

    for (unsigned int i=0; i<m_artCenterVec.size(); i++)
    {
        ArtCenterVecIt artCenterIt = m_artCenterVec.begin();
        ArtCenterVecIt artCenterItEnd = m_artCenterVec.end();

        while ((artCenterIt != artCenterItEnd))
        {
            ArtVecIt it = (*artCenterIt)->articulations.begin();
            ArtVecIt itEnd = (*artCenterIt)->articulations.end();
            while (it != itEnd)
            {
                std::vector< MechanicalState<sofa::defaulttype::Vec1dTypes>* > articulatedObjects;

                sofa::simulation::tree::GNode* curNode = dynamic_cast<sofa::simulation::tree::GNode*>(this->getContext());
                if (curNode)
                    curNode->getTreeObjects<MechanicalState<sofa::defaulttype::Vec1dTypes>, std::vector< MechanicalState<sofa::defaulttype::Vec1dTypes>* > >(&articulatedObjects);

                if (!articulatedObjects.empty())
                {
                    // Reference potential initial articulations value for interaction springs
                    // and Current articulation value at the coresponding artculation

                    std::vector< MechanicalState<sofa::defaulttype::Vec1dTypes>* >::iterator articulatedObjIt = articulatedObjects.begin();
                    std::vector< MechanicalState<sofa::defaulttype::Vec1dTypes>* >::iterator articulatedObjItEnd = articulatedObjects.end();

                    //	while (articulatedObjIt != articulatedObjItEnd)
                    if ((*it)->translation.getValue())
                    {
                        double diffMotions = (*it)->motion[frame+1] - (*it)->motion[frame];
                        (*(*articulatedObjIt)->getX())[(*it)->articulationIndex.getValue()] = (*it)->motion[frame] + (n/ndiv)*diffMotions;
                        (*(*articulatedObjIt)->getXfree())[(*it)->articulationIndex.getValue()] = (*it)->motion[frame] + (n/ndiv)*diffMotions;
                    }
                    else
                    {
                        double diffMotions = (((*it)->motion[frame+1]/180.0)*3.14) - (((*it)->motion[frame]/180.0)*3.14);
                        (*(*articulatedObjIt)->getX())[(*it)->articulationIndex.getValue()] = (((*it)->motion[frame]/180.0)*3.14) + (n/ndiv)*diffMotions;
                        (*(*articulatedObjIt)->getXfree())[(*it)->articulationIndex.getValue()] = (((*it)->motion[frame]/180.0)*3.14) + (n/ndiv)*diffMotions;
                    }
                }
                ++it;
            }
            ++artCenterIt;
        }
    }
    if (frame<(ahc->numOfFrames-2))
    {
        if (n<ndiv-1)
        {
            n++;
        }
        else
        {
            if((frame+frameInc) <= (ahc->numOfFrames-2))
                frame+=frameInc;
            n=0;
        }
    }
}

SOFA_DECL_CLASS(ArticulatedHierarchyBVHController)

// Register in the Factory
int ArticulatedHierarchyBVHControllerClass = core::RegisterObject("")
        .add< ArticulatedHierarchyBVHController >()
        ;
} // namespace controller

} // namespace component

} // namespace sofa
