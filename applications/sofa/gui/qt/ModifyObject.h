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
#ifndef SOFA_GUI_QT_MODIFYOBJECT_H
#define SOFA_GUI_QT_MODIFYOBJECT_H

#include "SofaGUIQt.h"
#include <sofa/core/objectmodel/BaseObject.h>

#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/helper/fixed_array.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/component/misc/Monitor.h>
#include <sofa/gui/qt/QTransformationWidget.h>
#include <sofa/gui/qt/QEnergyStatWidget.h>
#include <sofa/gui/qt/DisplayFlagWidget.h>
#include "WFloatLineEdit.h"

#ifdef SOFA_QT4
#include <QDialog>
#include <QWidget>
#include <Q3ListViewItem>
#include <Q3ListView>
#include <Q3Table>
#include <Q3GroupBox>
#include <Q3Grid>
#include <Q3TextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <Q3CheckListItem>
#include <QVBoxLayout>
#else
#include <qdialog.h>
#include <qwidget.h>
#include <qlistview.h>
#include <qtable.h>
#include <qgroupbox.h>
#include <qgrid.h>
#include <qtextedit.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlayout.h>
#endif

namespace sofa
{
namespace core
{
namespace objectmodel
{
class Base;
class BaseData;
}
}
namespace gui
{

namespace qt
{


#ifndef SOFA_QT4
typedef QListView   Q3ListView;
typedef QListViewItem Q3ListViewItem;
typedef QCheckListItem   Q3CheckListItem;
typedef QTable    Q3Table;
typedef QGroupBox Q3GroupBox;
typedef QTextEdit   Q3TextEdit;
typedef QGrid       Q3Grid;
#endif

typedef struct ModifyObjectFlags
{

    bool HIDE_FLAG; //if we allow to hide Datas
    bool READONLY_FLAG; //if we allow  ReadOnly Datas
    bool EMPTY_FLAG;//if we allow empty datas
    bool RESIZABLE_FLAG;
    bool REINIT_FLAG;
    bool LINKPATH_MODIFIABLE_FLAG; //if we allow to modify the links of the Data

    ModifyObjectFlags():
        HIDE_FLAG(true),
        READONLY_FLAG(true),
        EMPTY_FLAG(false),
        RESIZABLE_FLAG(false),
        REINIT_FLAG(true),
        LINKPATH_MODIFIABLE_FLAG(false) {};

    void setFlagsForSofa()
    {
        HIDE_FLAG = true;
        READONLY_FLAG = true;
        EMPTY_FLAG = false;
        RESIZABLE_FLAG = true;
        REINIT_FLAG = true;
        LINKPATH_MODIFIABLE_FLAG = false;
    };

    void setFlagsForModeler()
    {
        HIDE_FLAG = false;
        READONLY_FLAG=false; //everything will be editable
        EMPTY_FLAG = true;
        RESIZABLE_FLAG = true;
        REINIT_FLAG = false;
        LINKPATH_MODIFIABLE_FLAG = true;
    };
} ModifyObjectFlags;

class DataWidget;

class SOFA_SOFAGUIQT_API ModifyObject : public QDialog
{
    Q_OBJECT
public:

    explicit ModifyObject( void *Id,
            Q3ListViewItem* item_clicked,
            QWidget* parent,
            const ModifyObjectFlags& dialogFlags,
            const char* name= 0,
            bool  modal= FALSE,
            Qt::WFlags f= 0 );

    ~ModifyObject()
    {
        delete buttonUpdate;
    }

    const ModifyObjectFlags& getFlags() { return dialogFlags_;}

    void createDialog(core::objectmodel::Base* node);
    void createDialog(core::objectmodel::BaseData* data);
    bool hideData(core::objectmodel::BaseData* data) { return (!data->isDisplayed()) && dialogFlags_.HIDE_FLAG;};
    void readOnlyData(Q3Table *widget, core::objectmodel::BaseData* data);
    void readOnlyData(QWidget *widget, core::objectmodel::BaseData* data);

public slots:
    void reject   () {                 emit(dialogClosed(Id_)); deleteLater(); QDialog::reject();} //When closing a window, inform the parent.
    void accept   () { updateValues(); emit(dialogClosed(Id_)); deleteLater(); QDialog::accept();} //if closing by using Ok button, update the values
    void closeNow () {emit(reject());} //called from outside to close the current widget
    virtual void closeEvent ( QCloseEvent * ) {emit(reject());}
    void updateTables();
    virtual void updateValues();              //update the node with the values of the field
    void updateListViewItem();
signals:
    void updateDataWidgets();             // emitted eachtime updateValues is called to propagate the changes to the widgets.
    void objectUpdated();                 //update done
    void dialogClosed(void *);            //the current window has been closed: we give the Id of the current window
    void nodeNameModification(Q3ListViewItem *);
protected slots:
    //update the tables of value at each step of the simulation
    void clearWarnings() {node->clearWarnings(); logWarningEdit->clear();}
    void clearOutputs() {node->clearOutputs(); logOutputEdit->clear();}

protected:
    void updateConsole();             //update the console log of warnings and outputs

    void* Id_;
    Q3ListViewItem* item_;
    core::objectmodel::Base* node;
    core::objectmodel::BaseData* data_;
    const ModifyObjectFlags dialogFlags_;

    QWidget* outputTab;
    Q3TextEdit *logOutputEdit;
    QWidget *warningTab;
    Q3TextEdit *logWarningEdit;

    QTabWidget *dialogTab;
    QPushButton *buttonUpdate;

    //Widget specific to Node:
    //Transformation widget: translation, rotation, scale ( only experimental and deactivated)
    QTransformationWidget* transformation;
    //Energy widget: plot the kinetic & potential energy
    QEnergyStatWidget* energy;
    //Visual Flags
    QDisplayFlagWidget *displayFlag;
};


} // namespace qt

} // namespace gui

} // namespace sofa

#endif

