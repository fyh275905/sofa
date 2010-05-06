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
#ifndef SOFA_GUI_QT_QMOUSEOPERATIONS_H
#define SOFA_GUI_QT_QMOUSEOPERATIONS_H

#include <sofa/gui/MouseOperations.h>
#include <sofa/gui/qt/SofaMouseManager.h>
#include <sofa/gui/qt/DataWidget.h>
#ifdef SOFA_QT4
#include <QWidget>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QCheckBox>
#else
#include <qwidget.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#endif
#include <iostream>

namespace sofa
{

namespace gui
{

namespace qt
{


class QAttachOperation : public QWidget, public AttachOperation
{
    Q_OBJECT
public:
    QAttachOperation();

//        double getStiffness() const;
//        double getArrowSize() const;
    void configure(PickHandler *picker, MOUSE_BUTTON b)
    {
        AttachOperation::configure(picker, b);
    }
public slots:
    void WidgetDirty(bool);

protected:
    DataWidget *stiffnessWidget;
    DataWidget *arrowSizeWidget;
};



class QInciseOperation : public QWidget, public InciseOperation
{
    Q_OBJECT
public:
    QInciseOperation();
    int getIncisionMethod() const;
    int getSnapingBorderValue() const;
    int getSnapingValue() const;

    bool getCompleteIncision () {return finishIncision;}
    bool getKeepPoint () {return keepPoint;}

    void configure(PickHandler *picker, MOUSE_BUTTON b)
    {
        InciseOperation::configure(picker, b);
    }

    bool finishIncision;
    bool keepPoint;

public slots:
    void setEnableBox (bool i);
    void setFinishIncision (bool i);
    void setkeepPoint (bool i);


protected:
    QGroupBox* incisionMethodChoiceGroup;
    QRadioButton* method1;
    QRadioButton* method2;

    QGroupBox *advancedOperations;
    QCheckBox *finishCut;
    QCheckBox *storeLastPoint;

    QGroupBox* advancedOptions;
    QSlider  *snapingBorderSlider;
    QSpinBox *snapingBorderValue;
    QSlider  *snapingSlider;
    QSpinBox *snapingValue;
};



class QFixOperation : public QWidget, public FixOperation
{
    Q_OBJECT
public:
    QFixOperation();
    double getStiffness() const;
    void configure(PickHandler *picker, MOUSE_BUTTON b)
    {
        FixOperation::configure(picker, b);
    }

protected:
    QLineEdit *value;
};



class QTopologyOperation : public QWidget, public TopologyOperation
{
    Q_OBJECT
public:
    QTopologyOperation();
    double getScale() const;
    int getTopologicalOperation() const;
    bool getVolumicMesh() const;



    void configure(PickHandler *picker, MOUSE_BUTTON b)
    {
        TopologyOperation::configure(picker, b);
    }

public slots:
    void setEnableBox (int i);

protected:

    QComboBox *operationChoice;
    QRadioButton *meshType1;
    QRadioButton *meshType2;

    QGroupBox *advancedOptions;
    QSlider *scaleSlider;
    QSpinBox *scaleValue;
};


class QAddSutureOperation : public QWidget, public AddSutureOperation
{
    Q_OBJECT
public:
    QAddSutureOperation();
    double getStiffness() const;
    double getDamping() const;

    void configure(PickHandler *picker, MOUSE_BUTTON b)
    {
        AddSutureOperation::configure(picker, b);
    }

protected:
    QLineEdit *stiffness;
    QLineEdit *damping;
};

}
}
}

#endif
