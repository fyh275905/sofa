#ifndef MATERIAL_DATAWIDGET_H
#define MATERIAL_DATAWIDGET_H
#include <sofa/gui/qt/DataWidget.h>
#include <sofa/core/componentmodel/loader/Material.h>
#include <sofa/defaulttype/Vec.h>


#ifdef SOFA_QT4
#include <QColorDialog>
#include <QPainter>
#include <QStyle>
#include <QCheckBox>
#include <QColor>
#include <QPixmap>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QColorDialog>
#else
#include <qcolor.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qgridlayout.h>
#include <qcolordialog.h>
#include <qpixmap.h>
#include <qvalidator.h>
#include <qlineedit.h>
#include <qlabel.h>
#endif

namespace sofa
{
namespace gui
{
namespace qt
{

class RGBAColorPicker : public QWidget
{
    Q_OBJECT
signals:
    void hasChanged();
public:
    RGBAColorPicker(QWidget* parent);
    void setColor( const sofa::defaulttype::Vec4f& color );
    sofa::defaulttype::Vec4f getColor() const;
protected:
    QRgb _rgba;
    QLineEdit* _r;
    QLineEdit* _g;
    QLineEdit* _b;
    QLineEdit* _a;
    QPushButton* _colorButton;

protected slots:
    void updateRGBAColor();
    void redrawColorButton();
    void raiseQColorDialog();
};

class MaterialDataWidget : public TDataWidget<sofa::core::componentmodel::loader::Material>
{
    Q_OBJECT
public:
    MaterialDataWidget(QWidget* parent,
            const char* name,
            core::objectmodel::TData<sofa::core::componentmodel::loader::Material>* data):
        TDataWidget<sofa::core::componentmodel::loader::Material>(parent,name,data)
    {};

    virtual bool createWidgets();
    virtual unsigned int numColumnWidget() {return 1;}

protected:
    virtual void readFromData();
    virtual void writeToData();
    QLineEdit* _nameEdit;
    RGBAColorPicker* _ambientPicker;
    RGBAColorPicker* _emissivePicker;
    RGBAColorPicker* _specularPicker;
    RGBAColorPicker* _diffusePicker;
    QLineEdit*  _shininessEdit;
    QCheckBox* _ambientCheckBox;
    QCheckBox* _emissiveCheckBox;
    QCheckBox* _specularCheckBox;
    QCheckBox* _diffuseCheckBox;
    QCheckBox* _shininessCheckBox;
};
}
}
}

#endif MATERIAL_DATAWIDGET_H
