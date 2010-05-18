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

#ifndef SOFA_GUI_QT_DATAWIDGET_H
#define SOFA_GUI_QT_DATAWIDGET_H


#include "SofaGUIQt.h"
#include <sofa/core/objectmodel/BaseData.h>
#include <sofa/core/objectmodel/Base.h>
#include <sofa/helper/Factory.h>



#ifdef SOFA_QT4
#include <QDialog>
#include <QLineEdit>
#include <Q3Table>
#include <QPushButton>
#include <QSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
#else
#include <qspinbox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qtable.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#endif // SOFA_QT4

#ifndef SOFA_QT4
typedef QTable    Q3Table;
#endif


namespace sofa
{

namespace core
{
namespace objectmodel
{
class Base;
}
}
namespace gui
{
namespace qt
{

/**
*\brief Abstract Interface of a qwidget which allows to edit a data.
*/
class SOFA_SOFAGUIQT_API DataWidget : public QWidget
{
    Q_OBJECT
public:
    //
    // Factory related code
    //

    struct CreatorArgument
    {
        std::string name;
        core::objectmodel::BaseData* data;
        QWidget* parent;
        bool readOnly;
    };

    template<class T>
    static void create(T*& instance, const CreatorArgument& arg)
    {
        typename T::MyData* data = dynamic_cast<typename T::MyData*>(arg.data);
        if(!data) return;
        instance = new T(arg.parent, arg.name.c_str(), data);
        instance->setEnabled(arg.readOnly);
        if ( !instance->createWidgets() )
        {
            delete instance;
            instance = NULL;
        }
    }

    typedef sofa::helper::Factory<std::string, DataWidget, DataWidget::CreatorArgument> DataWidgetFactory;


    static DataWidget *CreateDataWidget(const DataWidget::CreatorArgument &dwarg)
    {

        DataWidget *datawidget_=0;
        const std::string &widgetName=dwarg.data->getWidget();
        if (widgetName.empty())
            datawidget_ = DataWidgetFactory::CreateAnyObject(dwarg);
        else
            datawidget_ = DataWidgetFactory::CreateObject(widgetName, dwarg);
        return datawidget_;
    };


public slots:
    /// Checks that widget has been edited
    /// emit DataOwnerDirty in case the name field has been modified
    void updateDataValue()
    {
        if(dirty)
        {
            const bool hasOwner = baseData->getOwner();
            std::string previousName;
            if ( hasOwner ) previousName = baseData->getOwner()->getName();
            writeToData();
            updateVisibility();
            if(hasOwner && baseData->getOwner()->getName() != previousName)
            {
                emit DataOwnerDirty(true);
            }
        }

        dirty = false;
        counter = baseData->getCounter();

    }
    /// First checks that the widget is not currently being edited
    /// checks that the data has changed since the last time the widget
    /// has read the data value.
    /// ultimately read the data value.
    void updateWidgetValue()
    {
        if(!dirty)
        {
            if(counter != baseData->getCounter())
                readFromData();
        }
    }
    /// You call this slot anytime you want to specify that the widget
    /// value is out of sync with the underlying data value.
    void setWidgetDirty(bool b=true)
    {
        dirty = b;
        emit WidgetDirty(b);
    }
signals:
    /// Emitted each time setWidgetDirty is called. You can also emit
    /// it if you want to tell the widget value is out of sync with
    /// the underlying data value.
    void WidgetDirty(bool );
    /// Currently this signal is used to reflect the changes of the
    /// component name in the sofaListview.
    void DataOwnerDirty(bool );
public:
    typedef core::objectmodel::BaseData MyData;

    DataWidget(QWidget* parent,const char* name, MyData* d) :
        QWidget(parent,name), baseData(d), dirty(false), counter(-1)
    {
    }
    virtual ~DataWidget() {}

    inline virtual void setData( MyData* d)
    {
        baseData = d;
        readFromData();
    }


    /// BaseData pointer accessor function.
    core::objectmodel::BaseData* getBaseData() const { return baseData; }
    void updateVisibility()
    {
        parentWidget()->setShown(baseData->isDisplayed());
    };
    bool isDirty() { return dirty; }

    /// The implementation of this method holds the widget creation and the signal / slot
    /// connections.
    virtual bool createWidgets() = 0;
    /// Helper method to give a size.
    virtual unsigned int sizeWidget() {return 1;}
    /// Helper method for colum.
    virtual unsigned int numColumnWidget() {return 3;}

protected:
    /// The implementation of this method tells how the widget reads the value of the data.
    virtual void readFromData() = 0;
    /// The implementation of this methods needs to tell how the widget can write its value
    /// in the data
    virtual void writeToData() = 0;

    core::objectmodel::BaseData* baseData;
    bool dirty;
    int counter;



};



/**
*\brief This class is basically the same as DataWidget, except that it
* takes a template parameter so the actual type of Data can be retrieved
* through the getData() accessor. In most cases you will need to derive
* from this class to implement the edition of your data in the GUI.
**/
template<class T>
class SOFA_SOFAGUIQT_API TDataWidget : public DataWidget
{

public:
    typedef sofa::core::objectmodel::TData<T> MyTData;

    template <class RealObject>
    static void create( RealObject*& obj, CreatorArgument& arg)
    {
        typename RealObject::MyTData* realData = dynamic_cast< typename RealObject::MyTData* >(arg.data);
        if (!realData) obj = NULL;
        else
        {
            obj = new RealObject(arg.parent,arg.name.c_str(), realData);
            obj->setEnabled(!arg.readOnly);
            if( !obj->createWidgets() )
            {
                delete obj;
                obj = NULL;
            }
        }

    }

    TDataWidget(QWidget* parent,const char* name, MyTData* d):
        DataWidget(parent,name,d),Tdata(d) {};
    /// Accessor function. Gives you the actual data instead
    /// of a BaseData pointer of it like in getBaseData().
    sofa::core::objectmodel::TData<T>* getData() const {return Tdata;}
    inline virtual void setData(MyTData* d)
    {
        Tdata = d;
    }
protected:
    MyTData* Tdata;
};



class QTableUpdater : virtual public Q3Table
{
    Q_OBJECT
public:
    QTableUpdater ( int numRows, int numCols, QWidget * parent = 0, const char * name = 0 ):
#ifdef SOFA_QT4
        Q3Table(numRows, numCols, parent, name)
#else
        QTable(numRows, numCols, parent, name)
#endif
    {};
public slots:
    void setDisplayed(bool b) {this->setShown(b);}
    void resizeTableV( int number )
    {
        QSpinBox *spinBox = (QSpinBox *) sender();
        QString header;
        if( spinBox == NULL)
        {
            return;
        }
        if (number != numRows())
        {
            setNumRows(number);

        }
    }

    void resizeTableH( int number )
    {
        QSpinBox *spinBox = (QSpinBox *) sender();
        QString header;
        if( spinBox == NULL)
        {
            return;
        }
        if (number != numCols())
        {
            setNumCols(number);

        }
    }

};


class QPushButtonUpdater: public QPushButton
{
    Q_OBJECT
public:

    QPushButtonUpdater( const QString & text, QWidget * parent = 0 ): QPushButton(text,parent) {};

public slots:
    void setDisplayed(bool b);
};

//Widget used to display the name of a Data and if needed the link to another Data
class QDisplayDataInfoWidget: public QWidget
{
    Q_OBJECT
public:
    QDisplayDataInfoWidget(QWidget* parent, const std::string& helper, core::objectmodel::BaseData* d, bool modifiable);
public slots:
    void linkModification();
    void linkEdited();
    unsigned int getNumLines() const { return numLines_;}
protected:
    void formatHelperString(const std::string& helper, std::string& final_text);
    static unsigned int numLines(const std::string& str);
    core::objectmodel::BaseData* data;
    unsigned int numLines_;
    QLineEdit *linkpath_edit;
};

typedef sofa::helper::Factory<std::string, DataWidget, DataWidget::CreatorArgument> DataWidgetFactory;


//MOC_SKIP_BEGIN
#ifdef SOFA_QT4
#if defined(WIN32) && !defined(SOFA_BUILD_SOFAGUIQT)
//delay load of the specialized Factory class. unique definition reside in the cpp file.
extern template class SOFA_SOFAGUIQT_API helper::Factory<std::string, DataWidget, DataWidget::CreatorArgument>;
#endif
#endif
//MOC_SKIP_END


} // qt
} // gui
} // sofa

#endif // SOFA_GUI_QT_DATAWIDGET_H

