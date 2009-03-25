/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc., 51  *
* Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
*******************************************************************************
*                            SOFA :: Applications                             *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/

#include <MyFakeComponent.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace behaviormodel
{



MyFakeComponent::MyFakeComponent()
    : myData( initData(&myData,(unsigned)1,"myData","Example of data") )
{
}


MyFakeComponent::~MyFakeComponent()
{
}

void MyFakeComponent::init()
{
}

void MyFakeComponent::reinit()
{
}

void MyFakeComponent::updatePosition(double /*dt*/)
{
}




SOFA_DECL_CLASS(MyFakeComponent)

int MyFakeComponentClass = core::RegisterObject("just an example of component")
        .add< MyFakeComponent >()
        ;




}	//behaviormodel

}	//component

}	//sofa

