#include <sofa/gui/qt/QTabulationModifyObject.h>

#include <sofa/gui/qt/QDisplayDataWidget.h>

#include <sofa/gui/qt/ModifyObject.h>

namespace sofa
{
namespace gui
{
namespace qt
{

QTabulationModifyObject::QTabulationModifyObject(QWidget* parent,
        core::objectmodel::Base *o, Q3ListViewItem* i,
        unsigned int idx):
    QWidget(parent), object(o), item(i), index(idx), size(0), maxSize(8), dirty(false)
{
    new QVBoxLayout( this, 0, 1, "tabVisualizationLayout");
}

void QTabulationModifyObject::addData(sofa::core::objectmodel::BaseData *data, const ModifyObjectFlags& flags)
{
    if (  (!data->isDisplayed()) && flags.HIDE_FLAG ) return;



    const std::string name=data->getName();
    QDisplayDataWidget* displaydatawidget = new QDisplayDataWidget(this,data,flags);
    this->layout()->add(displaydatawidget);

    size += displaydatawidget->getNumWidgets();

    connect(displaydatawidget, SIGNAL( WidgetDirty(bool) ), this, SLOT( setTabDirty(bool) ) );
    connect(displaydatawidget, SIGNAL( DataOwnerDirty(bool)),  this, SLOT( updateListViewItem() ) );
    connect(this, SIGNAL(UpdateDatas()), displaydatawidget, SLOT( UpdateData()));
    connect(this, SIGNAL(UpdateDataWidgets()), displaydatawidget, SLOT( UpdateWidgets()));
}


void QTabulationModifyObject::updateListViewItem()
{
    if (dynamic_cast< simulation::Node *>(object))
        item->setText(0,object->getName().c_str());
    else
    {
        std::string name=item->text(0).ascii();
        std::string::size_type pos = name.find(' ');
        if (pos != std::string::npos)
            name = name.substr(0,pos);
        name += "  ";
        name += object->getName();
        item->setText(0,name.c_str());
    }
}

void QTabulationModifyObject::setTabDirty(bool b)
{
    dirty=b;
    emit TabDirty(b);
}

bool QTabulationModifyObject::isDirty() const
{
    return dirty;
}

bool QTabulationModifyObject::isFull() const
{
    return size >= maxSize;
}

bool QTabulationModifyObject::isEmpty() const
{
    return size==0;
}

void QTabulationModifyObject::updateDataValue()
{
    emit UpdateDatas();
}

void QTabulationModifyObject::updateWidgetValue()
{
    emit UpdateDataWidgets();
}

void QTabulationModifyObject::addStretch()
{
    dynamic_cast<QVBoxLayout*>(this->layout())->addStretch();
}

} // qt
} // gui
} //sofa


