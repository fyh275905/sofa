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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
//
// C++ Implementation : Controller
//
// Description:
//
//
// Author: Pierre-Jean Bensoussan, Digital Trainers (2008)
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <sofa/core/componentmodel/behavior/BaseController.h>

#include <sofa/core/objectmodel/Event.h>
#include <sofa/core/objectmodel/JoystickEvent.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>
#include <sofa/core/objectmodel/MouseEvent.h>
#include <sofa/core/objectmodel/OmniEvent.h>


#include <iostream>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

BaseController::BaseController()
    : handleEventTriggersUpdate( initData(&handleEventTriggersUpdate, "handleEventTriggersUpdate", "Event handling frequency controls the controller update frequency" ) )
{

}



void BaseController::handleEvent(core::objectmodel::Event *event)
{
    if (dynamic_cast<sofa::simulation::AnimateBeginEvent *>(event))
    {
        onBeginAnimationStep();
    }
    else if (dynamic_cast<sofa::simulation::AnimateEndEvent *>(event))
    {
        onEndAnimationStep();
    }
    else if (dynamic_cast<sofa::core::objectmodel::KeypressedEvent *>(event))
    {
        sofa::core::objectmodel::KeypressedEvent *kpev = dynamic_cast<sofa::core::objectmodel::KeypressedEvent *>(event);
        onKeyPressedEvent(kpev);
    }
    else if (dynamic_cast<sofa::core::objectmodel::KeyreleasedEvent *>(event))
    {
        sofa::core::objectmodel::KeyreleasedEvent *krev = dynamic_cast<sofa::core::objectmodel::KeyreleasedEvent *>(event);
        onKeyReleasedEvent(krev);
    }
    else if (dynamic_cast<sofa::core::objectmodel::MouseEvent *>(event))
    {
        sofa::core::objectmodel::MouseEvent *mev = dynamic_cast<sofa::core::objectmodel::MouseEvent *>(event);
        onMouseEvent(mev);
    }
    else if (dynamic_cast<sofa::core::objectmodel::JoystickEvent *>(event))
    {
        sofa::core::objectmodel::JoystickEvent *jev = dynamic_cast<sofa::core::objectmodel::JoystickEvent *>(event);
        onJoystickEvent(jev);
    }
    else if (dynamic_cast<sofa::core::objectmodel::OmniEvent *>(event))
    {
        sofa::core::objectmodel::OmniEvent *oev = dynamic_cast<sofa::core::objectmodel::OmniEvent *>(event);
        onOmniEvent(oev);
    }
}

}

}

}

}

