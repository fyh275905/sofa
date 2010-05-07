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
#include <sofa/gui/qt/SofaMouseManager.h>
#include <sofa/gui/qt/QMouseOperations.h>

#include <sofa/gui/MouseOperations.h>
#include <sofa/gui/OperationFactory.h>

#include <iostream>
#ifndef SOFA_QT4
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>
#endif

namespace sofa
{
namespace gui
{
namespace qt
{
SofaMouseManager::SofaMouseManager()
{
    connect( LeftOperationCombo,   SIGNAL(activated(int)), this, SLOT( selectOperation(int) ));
    connect( MiddleOperationCombo, SIGNAL(activated(int)), this, SLOT( selectOperation(int) ));
    connect( RightOperationCombo,  SIGNAL(activated(int)), this, SLOT( selectOperation(int) ));

    RegisterOperation("Attach").add< QAttachOperation >();
    RegisterOperation("AddFrame").add< AddFrameOperation >();
    RegisterOperation("Fix")   .add< QFixOperation  >();
    RegisterOperation("Incise").add< QInciseOperation  >();
    RegisterOperation("Remove").add< QTopologyOperation  >();
    RegisterOperation("Suture").add< QAddSutureOperation >();
}

void SofaMouseManager::updateContent()
{
    LeftOperationCombo->clear();
    MiddleOperationCombo->clear();
    RightOperationCombo->clear();
    mapIndexOperation.clear();

    if (mapIndexOperation.empty())
    {
        const OperationFactory::RegisterStorage &registry = OperationFactory::getInstance()->registry;

        int idx=0;
        for (OperationFactory::RegisterStorage::const_iterator it=registry.begin(); it!=registry.end(); ++it)
        {
            LeftOperationCombo  ->insertItem(QString(OperationFactory::GetDescription(it->first).c_str()));
            MiddleOperationCombo->insertItem(QString(OperationFactory::GetDescription(it->first).c_str()));
            RightOperationCombo ->insertItem(QString(OperationFactory::GetDescription(it->first).c_str()));

            if (OperationFactory::GetDescription(it->first) == OperationFactory::GetDescription(usedOperations[LEFT]))
                LeftOperationCombo->setCurrentItem(idx);
            if (OperationFactory::GetDescription(it->first) == OperationFactory::GetDescription(usedOperations[MIDDLE]))
                MiddleOperationCombo->setCurrentItem(idx);
            if (OperationFactory::GetDescription(it->first) == OperationFactory::GetDescription(usedOperations[RIGHT]))
                RightOperationCombo->setCurrentItem(idx);

            mapIndexOperation.insert(std::make_pair(idx++, it->first));
        }
    }
}

void SofaMouseManager::setPickHandler(PickHandler *picker)
{
    pickHandler=picker;
    updateContent();
    updateOperation(LEFT,   "Attach");
    updateOperation(MIDDLE, "Incise");
    updateOperation(RIGHT,  "Remove");
}


void SofaMouseManager::selectOperation(int operation)
{
    QComboBox *combo = (QComboBox*)(sender());
    const std::string operationName=mapIndexOperation[operation];

    if      (combo == LeftOperationCombo)   updateOperation(LEFT,   operationName);
    else if (combo == MiddleOperationCombo) updateOperation(MIDDLE, operationName);
    else if (combo == RightOperationCombo)  updateOperation(RIGHT,  operationName);
}

void SofaMouseManager::updateOperation(  sofa::component::configurationsetting::MouseButtonSetting* setting)
{
    //By changing the operation, we delete the previous operation
    Operation *operation=pickHandler->changeOperation( setting);
    updateOperation(operation);
}

void SofaMouseManager::updateOperation( MOUSE_BUTTON button, const std::string &id)
{
    //By changing the operation, we delete the previous operation
    Operation *operation=pickHandler->changeOperation( button, id);
    updateOperation(operation);
}


void SofaMouseManager::updateOperation( Operation* operation)
{
    if (!operation || operation->getMouseButton()==NONE ) return;
    usedOperations[operation->getMouseButton()] = operation->getId();

    QWidget* qoperation=dynamic_cast<QWidget*>(operation);
    if (!qoperation) return;

    switch(operation->getMouseButton())
    {
    case LEFT:
    {
#ifdef SOFA_QT4
        LeftButton->layout()->addWidget(qoperation);
#else
        LeftButton->layout()->add(qoperation);
#endif
        break;
    }
    case MIDDLE:
    {
#ifdef SOFA_QT4
        MiddleButton->layout()->addWidget(qoperation);
#else
        MiddleButton->layout()->add(qoperation);
#endif
        break;
    }
    case RIGHT:
    {
#ifdef SOFA_QT4
        RightButton->layout()->addWidget(qoperation);
#else
        RightButton->layout()->add(qoperation);
#endif
        break;
    }
    default:
    {
    }
    }


}

}
}
}

