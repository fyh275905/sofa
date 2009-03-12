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
// C++ Models: MechanicalStateController
//
// Description:
//
//
// Author: Pierre-Jean Bensoussan, Digital Trainers (2008)
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef SOFA_COMPONENT_CONTROLLER_MECHANICALSTATECONTROLLER_INL
#define SOFA_COMPONENT_CONTROLLER_MECHANICALSTATECONTROLLER_INL

#include <sofa/component/controller/MechanicalStateController.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/objectmodel/MouseEvent.h>
#include <sofa/core/objectmodel/OmniEvent.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/Quat.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/UpdateMappingVisitor.h>

#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/VecTypes.h>
namespace sofa
{

namespace component
{

namespace controller
{

template <class DataTypes>
MechanicalStateController<DataTypes>::MechanicalStateController()
    : index( initData(&index, (unsigned int)0, "index", "Index of the controlled DOF") )
    , onlyTranslation( initData(&onlyTranslation, false, "onlyTranslation", "Controlling the DOF only in translation") )
    , mainDirection(sofa::defaulttype::Vec<3,Real>((Real)0.0, (Real)0.0, (Real)-1.0))
    , mainDirectionPtr( initDataPtr(&mainDirectionPtr, &mainDirection, "mainDirection", "Main direction and orientation of the controlled DOF") )
{
    mainDirection.normalize();
}

template <class DataTypes>
void MechanicalStateController<DataTypes>::init()
{
    using core::componentmodel::behavior::MechanicalState;
    mainDirectionPtr.beginEdit();
    mState = dynamic_cast<MechanicalState<DataTypes> *> (this->getContext()->getMechanicalState());
    if (!mState)
        serr << "MechanicalStateController has no binding MechanicalState" << sendl;
    omni = false;
}


template <class DataTypes>
void MechanicalStateController<DataTypes>::applyController()
{

    using sofa::defaulttype::Quat;
    using sofa::defaulttype::Vec;


    if(omni)
    {
        if(mState)
        {
//			if(mState->getXfree())
            {
//				(*mState->getXfree())[0].getCenter()[0] = (Real)omniX;
//			//	(*mState->getX())[0].getCenter()[0] = omniX;
//				(*mState->getXfree())[0].getCenter()[1] = (Real)omniY;
//			//	(*mSt ate->getX())[0].getCenter()[1] = omniY;
//				(*mState->getXfree())[0].getCenter()[2] = (Real)omniZ;
//			//	(*mState->getX())[0].getCenter()[2] = omniZ;

                (*mState->getXfree())[0].getCenter() = position;
                (*mState->getX())[0].getCenter() = position;

//				(*mState->getXfree())[0].getOrientation()[0] = 0.0;
//				(*mState->getXfree())[0].getOrientation()[1] = 0.0;
//				(*mState->getXfree())[0].getOrientation()[2] = 0.0;
//				(*mState->getXfree())[0].getOrientation()[3] = 1.0;

                (*mState->getXfree())[0].getOrientation() = orientation;

//				(*mState->getX())[0].getOrientation()[0] = 0.0;
//				(*mState->getX())[0].getOrientation()[1] = 0.0;
//				(*mState->getX())[0].getOrientation()[2] = 0.0;
//				(*mState->getX())[0].getOrientation()[3] = 1.0;

                (*mState->getX())[0].getOrientation() = orientation;


            }
        }
        omni = false;
    }

    if ( !onlyTranslation.getValue()  && ((mouseMode==BtLeft) || (mouseMode==BtRight)))
    {
        int dx = eventX - mouseSavedPosX;
        int dy = eventY - mouseSavedPosY;
        mouseSavedPosX = eventX;
        mouseSavedPosY = eventY;

        if (mState)
        {
            unsigned int i = index.getValue();

            Vec<3,Real> x(1,0,0);
            Vec<3,Real> y(0,1,0);
            Vec<3,Real> z(0,0,1);

            if (mouseMode==BtLeft)
            {
                (*mState->getXfree())[i].getOrientation() = (*mState->getX())[i].getOrientation() * Quat(y, dx * (Real)0.001) * Quat(z, dy * (Real)0.001);
                (*mState->getX())[i].getOrientation() = (*mState->getX())[i].getOrientation() * Quat(y, dx * (Real)0.001) * Quat(z, dy * (Real)0.001);
            }
            else
            {
                sofa::helper::Quater<Real>& quatrot = (*mState->getX())[i].getOrientation();
                sofa::defaulttype::Vec<3,Real> vectrans(dy * mainDirection[0] * (Real)0.05, dy * mainDirection[1] * (Real)0.05, dy * mainDirection[2] * (Real)0.05);
                vectrans = quatrot.rotate(vectrans);

                (*mState->getX())[i].getCenter() += vectrans;
                (*mState->getX())[i].getOrientation() = (*mState->getX())[i].getOrientation() * Quat(x, dx * (Real)0.001);

                //	(*mState->getX0())[i].getCenter() += vectrans;
                //	(*mState->getX0())[i].getOrientation() = (*mState->getX0())[i].getOrientation() * Quat(x, dx * (Real)0.001);

                if(mState->getXfree())
                {
                    (*mState->getXfree())[i].getCenter() += vectrans;
                    (*mState->getXfree())[i].getOrientation() = (*mState->getX())[i].getOrientation() * Quat(x, dx * (Real)0.001);
                }
            }
        }
    }
    else if( onlyTranslation.getValue() )
    {
        if( mouseMode )
        {
            int dx = eventX - mouseSavedPosX;
            int dy = eventY - mouseSavedPosY;
            mouseSavedPosX = eventX;
            mouseSavedPosY = eventY;

// 			Real d = sqrt(dx*dx+dy*dy);
// 			if( dx<0 || dy<0 ) d = -d;

            if (mState)
            {
                unsigned int i = index.getValue();

                switch( mouseMode )
                {
                case BtLeft:
                    (*mState->getX())[i].getCenter() += Vec<3,Real>((Real)dx,(Real)0,(Real)0);
                    break;
                case BtRight :
                    (*mState->getX())[i].getCenter() += Vec<3,Real>((Real)0,(Real)dy,(Real)0);
                    break;
                case BtMiddle :
                    (*mState->getX())[i].getCenter() += Vec<3,Real>((Real)0,(Real)0,(Real)dy);
                    break;
                default :
                    break;
                }
            }
        }
    }


    sofa::simulation::Node *node = static_cast<sofa::simulation::Node*> (this->getContext());
    sofa::simulation::MechanicalPropagatePositionAndVelocityVisitor mechaVisitor; mechaVisitor.execute(node);
    sofa::simulation::UpdateMappingVisitor updateVisitor; updateVisitor.execute(node);
};



template <class DataTypes>
void MechanicalStateController<DataTypes>::onOmniEvent(core::objectmodel::OmniEvent *oev)
{
    //sout << "void MechanicalStateController<DataTypes>::onOmniEvent()"<<sendl;
    //if (oev->getButton())
    //{
    //		sout<<" Button1 pressed"<<sendl;

    //}

    omni = true;
    buttonOmni = oev->getButton();
    omniX = oev->getPosX();
    omniY = oev->getPosY();
    omniZ = oev->getPosZ();
    position = oev->getPosition();
    orientation = oev->getOrientation();
    applyController();
    omni = false;



}



template <class DataTypes>
void MechanicalStateController<DataTypes>::onBeginAnimationStep()
{
    applyController();
}



template <class DataTypes>
core::componentmodel::behavior::MechanicalState<DataTypes> *MechanicalStateController<DataTypes>::getMechanicalState() const
{
    return mState;
}



template <class DataTypes>
void MechanicalStateController<DataTypes>::setMechanicalState(core::componentmodel::behavior::MechanicalState<DataTypes> *_mState)
{
    mState = _mState;
}



template <class DataTypes>
unsigned int MechanicalStateController<DataTypes>::getIndex() const
{
    return index.getValue();
}



template <class DataTypes>
void MechanicalStateController<DataTypes>::setIndex(const unsigned int _index)
{
    index.setValue(_index);
}



template <class DataTypes>
const sofa::defaulttype::Vec<3, typename MechanicalStateController<DataTypes>::Real > &MechanicalStateController<DataTypes>::getMainDirection() const
{
    return mainDirection;
}



template <class DataTypes>
void MechanicalStateController<DataTypes>::setMainDirection(const sofa::defaulttype::Vec<3,Real> _mainDirection)
{
    mainDirection = _mainDirection;
}



template <class DataTypes>
void MechanicalStateController<DataTypes>::onMouseEvent(core::objectmodel::MouseEvent *mev)
{

}



#ifndef SOFA_FLOAT
template <>
SOFA_COMPONENT_CONTROLLER_API void MechanicalStateController<defaulttype::Vec1dTypes>::applyController();

template <>
SOFA_COMPONENT_CONTROLLER_API void MechanicalStateController<defaulttype::Vec1dTypes>::onMouseEvent(core::objectmodel::MouseEvent *mev);

template <>
SOFA_COMPONENT_CONTROLLER_API void MechanicalStateController<defaulttype::Rigid3dTypes>::onMouseEvent(core::objectmodel::MouseEvent *mev);
#endif

#ifndef SOFA_DOUBLE
template <>
SOFA_COMPONENT_CONTROLLER_API void MechanicalStateController<defaulttype::Vec1fTypes>::applyController();

template <>
SOFA_COMPONENT_CONTROLLER_API void MechanicalStateController<defaulttype::Vec1fTypes>::onMouseEvent(core::objectmodel::MouseEvent *mev);

template <>
SOFA_COMPONENT_CONTROLLER_API void MechanicalStateController<defaulttype::Rigid3fTypes>::onMouseEvent(core::objectmodel::MouseEvent *mev);
#endif


} // namespace controller

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENT_CONTROLLER_MECHANICALSTATECONTROLLER_H
