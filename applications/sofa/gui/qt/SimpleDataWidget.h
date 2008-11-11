/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_GUI_QT_SIMPLEDATAWIDGET_H
#define SOFA_GUI_QT_SIMPLEDATAWIDGET_H

#include <sofa/gui/qt/ModifyObject.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/VecTypes.h>
//#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/helper/fixed_array.h>
#include "WFloatLineEdit.h"

namespace sofa
{

namespace gui
{

namespace qt
{

using sofa::helper::Quater;


/// This class is used to specify how to graphically represent a data type,
/// by default using a simple QLineEdit
template<class T>
class data_widget_trait
{
public:
    typedef T data_type;
    typedef QLineEdit Widget;
    static Widget* create(QWidget* parent, const data_type& /*d*/)
    {
        Widget* w = new Widget(parent);
        return w;
    }
    static void readFromData(Widget* w, const data_type& d)
    {
        std::ostringstream o;
        o << d;
        w->setText(QString(o.str().c_str()));
    }
    static void writeToData(Widget* w, data_type& d)
    {
        std::string s = w->text().ascii();
        std::istringstream i(s);
        i >> d;
    }
    template<class P, class S>
    static void connectChanged(Widget* w, P* p, S s)
    {
        p->connect(w, SIGNAL( textChanged(const QString&) ), p, s);
    }
};


/// This class is used to create and manage the GUI of a data type,
/// using data_widget_trait to know which widgets to use
template<class T>
class data_widget_container
{
public:
    typedef T data_type;
    typedef data_widget_trait<data_type> helper;
    typedef typename helper::Widget Widget;
    Widget* w;
    data_widget_container() : w(NULL) {}
    template<class Dialog, class Slot>
    bool createWidgets(Dialog* dialog, Slot s, QWidget* parent, const data_type& d, bool readOnly)
    {
        w = helper::create(parent, d);
        if (w == NULL) return false;
        helper::readFromData(w, d);
        if (readOnly)
            w->setEnabled(false);
        else
            helper::connectChanged(w, dialog, s);
        return true;
    }
    void setReadOnly(bool readOnly)
    {
        w->setEnabled(!readOnly);
    }
    void readFromData(const data_type& d)
    {
        helper::readFromData(w, d);
    }
    void writeToData(data_type& d)
    {
        helper::writeToData(w, d);
    }
    bool processChange(const QObject* sender)
    {
        if (sender == w)
            return true;
        return false;
    }
};

/// This class manages the GUI of a BaseData, using the corresponding instance of data_widget_container
template<class T, class Container = data_widget_container<T> >
class SimpleDataWidget : public DataWidget
{
public:
    typedef sofa::core::objectmodel::TData<T> MyData;
protected:
    typedef T data_type;
    Container container;
    typedef data_widget_trait<data_type> helper;
    MyData* data;
    bool modified;
public:
    SimpleDataWidget(MyData* d) : DataWidget(d), data(d), modified(false) {}
    virtual bool createWidgets(QWidget* parent)
    {
        const data_type& d = data->virtualGetValue();
        if (!container.createWidgets(this->dialog, SLOT( changeValue() ), parent, d, this->readOnly))
            return false;
        return true;
    }
    virtual void readFromData()
    {
        container.readFromData(data->virtualGetValue());
        modified = false;
    }
    virtual bool isModified() { return modified; }
    virtual void writeToData()
    {
        if (!modified) return;
        data_type d = data->virtualGetValue();
        container.writeToData(d);
        data->virtualSetValue(d);
    }
    virtual bool processChange(const QObject* sender)
    {
        if (container.processChange(sender))
        {
            modified = true;
            return true;
        }
        else return false;
    }
};

////////////////////////////////////////////////////////////////
/// std::string support
////////////////////////////////////////////////////////////////

template<>
class data_widget_trait < std::string >
{
public:
    typedef std::string data_type;
    typedef QLineEdit Widget;
    static Widget* create(QWidget* parent, const data_type& /*d*/)
    {
        Widget* w = new Widget(parent);
        return w;
    }
    static void readFromData(Widget* w, const data_type& d)
    {
        w->setText(QString(d.c_str()));
    }
    static void writeToData(Widget* w, data_type& d)
    {
        d = w->text().ascii();
    }
    template<class P, class S>
    static void connectChanged(Widget* w, P* p, S s)
    {
        p->connect(w, SIGNAL( textChanged(const QString&) ), p, s);
    }
};

////////////////////////////////////////////////////////////////
/// bool support
////////////////////////////////////////////////////////////////

template<>
class data_widget_trait < bool >
{
public:
    typedef bool data_type;
    typedef QCheckBox Widget;
    static Widget* create(QWidget* parent, const data_type& /*d*/)
    {
        Widget* w = new Widget(parent);
        return w;
    }
    static void readFromData(Widget* w, const data_type& d)
    {
        w->setChecked(d);
    }
    static void writeToData(Widget* w, data_type& d)
    {
        d = (data_type) w->isOn();
    }
    template<class P, class S>
    static void connectChanged(Widget* w, P* p, S s)
    {
        p->connect(w, SIGNAL( toggled(bool) ), p, s);
    }
};

////////////////////////////////////////////////////////////////
/// float and double support
////////////////////////////////////////////////////////////////

template<class T>
class real_data_widget_trait
{
public:
    typedef T data_type;
    typedef WFloatLineEdit Widget;
    static Widget* create(QWidget* parent, const data_type& /*d*/)
    {
        Widget* w = new Widget(parent, "real");
        w->setMinFloatValue( (float)-INFINITY );
        w->setMaxFloatValue( (float)INFINITY );
        return w;
    }
    static void readFromData(Widget* w, const data_type& d)
    {
        w->setFloatValue(d);
    }
    static void writeToData(Widget* w, data_type& d)
    {
        d = (data_type) w->getFloatValue();
    }
    template<class P, class S>
    static void connectChanged(Widget* w, P* p, S s)
    {
        p->connect(w, SIGNAL( textChanged(const QString&) ), p, s);
    }
};

template<>
class data_widget_trait < float > : public real_data_widget_trait < float >
{};

template<>
class data_widget_trait < double > : public real_data_widget_trait < double >
{};


////////////////////////////////////////////////////////////////
/// int, unsigned int, char and unsigned char support
////////////////////////////////////////////////////////////////

template<class T, int vmin, int vmax>
class int_data_widget_trait
{
public:
    typedef T data_type;
    typedef QSpinBox Widget;
    static Widget* create(QWidget* parent, const data_type& /*d*/)
    {
        Widget* w = new Widget(vmin, vmax, 1, parent);
        return w;
    }
    static void readFromData(Widget* w, const data_type& d)
    {
        w->setValue((int)d);
    }
    static void writeToData(Widget* w, data_type& d)
    {
        d = (data_type) w->value();
    }
    template<class P, class S>
    static void connectChanged(Widget* w, P* p, S s)
    {
        p->connect(w, SIGNAL( valueChanged(int) ), p, s);
    }
};

template<>
class data_widget_trait < int > : public int_data_widget_trait < int, INT_MIN, INT_MAX >
{};

template<>
class data_widget_trait < unsigned int > : public int_data_widget_trait < unsigned int, 0, INT_MAX >
{};

template<>
class data_widget_trait < char > : public int_data_widget_trait < char, -128, 127 >
{};

template<>
class data_widget_trait < unsigned char > : public int_data_widget_trait < unsigned char, 0, 255 >
{};

////////////////////////////////////////////////////////////////
/// arrays and vectors support
////////////////////////////////////////////////////////////////

/// This class is used to get properties of a data type in order to display it as a table or a list
template<class T>
class vector_data_trait
{
public:
    typedef T data_type;
    /// Type of a row if this data type is viewed in a table or list
    typedef T value_type;
    /// Number of dimensions of this data type
    enum { NDIM = 0 };
    enum { SIZE = 1 };
    /// Get the number of rows
    static int size(const data_type&) { return SIZE; }
    /// Get the name of a row, or NULL if the index should be used instead
    static const char* header(const data_type& /*d*/, int /*i*/ = 0)
    {
        return NULL;
    }
    /// Get a row
    static const value_type* get(const data_type& d, int i = 0)
    {
        return (i == 0) ? &d : NULL;
    }
    /// Set a row
    static void set( const value_type& v, data_type& d, int i = 0)
    {
        if (i == 0)
            d = v;
    }
    /// Resize
    static void resize( int /*s*/, data_type& /*d*/)
    {
    }
};

template<class T, class Container = data_widget_container< typename vector_data_trait<T>::value_type> >
class fixed_vector_data_widget_container
{
public:
    typedef T data_type;
    typedef vector_data_trait<data_type> vhelper;
    typedef typename vhelper::value_type value_type;
    enum { N = vhelper::SIZE };
    Container w[N];
    fixed_vector_data_widget_container() {}
    template<class Dialog, class Slot>
    bool createWidgets(Dialog* dialog, Slot s, QWidget* parent, const data_type& d, bool readOnly)
    {
        Q3Grid* grid= new Q3Grid(N,parent);
        for (int i=0; i<N; ++i)
            if (!w[i].createWidgets(dialog, s, grid, *vhelper::get(d,i), readOnly))
                return false;
        return true;
    }
    void setReadOnly(bool readOnly)
    {
        for (int i=0; i<N; ++i)
            w[i].setReadOnly(readOnly);
    }
    void readFromData(const data_type& d)
    {
        for (int i=0; i<N; ++i)
            w[i].readFromData(*vhelper::get(d,i));
    }
    void writeToData(data_type& d)
    {
        for (int i=0; i<N; ++i)
        {
            value_type v = *vhelper::get(d,i);
            w[i].writeToData(v);
            vhelper::set(v,d,i);
        }
    }
    bool processChange(const QObject* sender)
    {
        for (int i=0; i<N; ++i)
            if (w[i].processChange(sender))
                return true;
        return false;
    }
};

template<class T, class Container = data_widget_container< typename vector_data_trait< typename vector_data_trait<T>::value_type >::value_type> >
class fixed_grid_data_widget_container
{
public:
    typedef T data_type;
    typedef vector_data_trait<data_type> rhelper;
    typedef typename rhelper::value_type row_type;
    typedef vector_data_trait<row_type> vhelper;
    typedef typename vhelper::value_type value_type;
    enum { L = rhelper::SIZE };
    enum { C = vhelper::SIZE };
    Container w[L][C];
    fixed_grid_data_widget_container() {}
    template<class Dialog, class Slot>
    bool createWidgets(Dialog* dialog, Slot s, QWidget* parent, const data_type& d, bool readOnly)
    {
        Q3Grid* grid= new Q3Grid(C,parent);
        for (int y=0; y<L; ++y)
            for (int x=0; x<C; ++x)
                if (!w[y][x].createWidgets(dialog, s, grid, *vhelper::get(*rhelper::get(d,y),x), readOnly))
                    return false;
        return true;
    }
    void setReadOnly(bool readOnly)
    {
        for (int y=0; y<L; ++y)
            for (int x=0; x<C; ++x)
                w[y][x].setReadOnly(readOnly);
    }
    void readFromData(const data_type& d)
    {
        for (int y=0; y<L; ++y)
            for (int x=0; x<C; ++x)
                w[y][x].readFromData(*vhelper::get(*rhelper::get(d,y),x));
    }
    void writeToData(data_type& d)
    {
        for (int y=0; y<L; ++y)
        {
            row_type r = *rhelper::get(d,y);
            for (int x=0; x<C; ++x)
            {
                value_type v = *vhelper::get(r,x);
                w[y][x].writeToData(v);
                vhelper::set(v,r,x);
            }
            rhelper::set(r,d,y);
        }
    }
    bool processChange(const QObject* sender)
    {
        for (int y=0; y<L; ++y)
            for (int x=0; x<C; ++x)
                if (w[y][x].processChange(sender))
                    return true;
        return false;
    }
};

////////////////////////////////////////////////////////////////
/// sofa::helper::fixed_array support
////////////////////////////////////////////////////////////////

template<class T, std::size_t N>
class vector_data_trait < sofa::helper::fixed_array<T, N> >
{
public:
    typedef sofa::helper::fixed_array<T, N> data_type;
    typedef T value_type;
    enum { NDIM = 1 };
    enum { SIZE = N };
    static int size(const data_type&) { return SIZE; }
    static const char* header(const data_type& /*d*/, int /*i*/ = 0)
    {
        return NULL;
    }
    static const value_type* get(const data_type& d, int i = 0)
    {
        return ((unsigned)i < (unsigned)size(d)) ? &(d[i]) : NULL;
    }
    static void set( const value_type& v, data_type& d, int i = 0)
    {
        if ((unsigned)i < (unsigned)size(d))
            d[i] = v;
    }
    static void resize( int s, data_type& d)
    {
    }
};

template<class T, std::size_t N>
class data_widget_container < sofa::helper::fixed_array<T, N> > : public fixed_vector_data_widget_container < sofa::helper::fixed_array<T, N> >
{};

////////////////////////////////////////////////////////////////
/// sofa::defaulttype::Vec support
////////////////////////////////////////////////////////////////

template<int N, class T>
class vector_data_trait < sofa::defaulttype::Vec<N, T> >
{
public:
    typedef sofa::defaulttype::Vec<N, T> data_type;
    typedef T value_type;
    enum { NDIM = 1 };
    enum { SIZE = N };
    static int size(const data_type&) { return SIZE; }
    static const char* header(const data_type& /*d*/, int /*i*/ = 0)
    {
        return NULL;
    }
    static const value_type* get(const data_type& d, int i = 0)
    {
        return ((unsigned)i < (unsigned)size(d)) ? &(d[i]) : NULL;
    }
    static void set( const value_type& v, data_type& d, int i = 0)
    {
        if ((unsigned)i < (unsigned)size(d))
            d[i] = v;
    }
    static void resize( int /*s*/, data_type& /*d*/)
    {
    }
};

template<>
inline const char* vector_data_trait < sofa::defaulttype::Vec<2, float> >::header(const data_type& /*d*/, int i)
{
    switch(i)
    {
    case 0: return "X";
    case 1: return "Y";
    }
    return NULL;
}

template<>
inline const char* vector_data_trait < sofa::defaulttype::Vec<2, double> >::header(const data_type& /*d*/, int i)
{
    switch(i)
    {
    case 0: return "X";
    case 1: return "Y";
    }
    return NULL;
}

template<>
inline const char* vector_data_trait < sofa::defaulttype::Vec<3, float> >::header(const data_type& /*d*/, int i)
{
    switch(i)
    {
    case 0: return "X";
    case 1: return "Y";
    case 2: return "Z";
    }
    return NULL;
}

template<>
inline const char* vector_data_trait < sofa::defaulttype::Vec<3, double> >::header(const data_type& /*d*/, int i)
{
    switch(i)
    {
    case 0: return "X";
    case 1: return "Y";
    case 2: return "Z";
    }
    return NULL;
}

template<int N, class T>
class data_widget_container < sofa::defaulttype::Vec<N, T> > : public fixed_vector_data_widget_container < sofa::defaulttype::Vec<N, T> >
{};

////////////////////////////////////////////////////////////////
/// std::helper::Quater support
////////////////////////////////////////////////////////////////

template<class T>
class vector_data_trait < Quater<T> >
{
public:
    typedef Quater<T> data_type;
    typedef T value_type;
    enum { NDIM = 1 };
    enum { SIZE = 4 };
    static int size(const data_type&) { return SIZE; }
    static const char* header(const data_type& /*d*/, int i = 0)
    {
        switch(i)
        {
        case 0: return "qX";
        case 1: return "qY";
        case 2: return "qZ";
        case 3: return "qW";
        }
        return NULL;
    }
    static const value_type* get(const data_type& d, int i = 0)
    {
        return ((unsigned)i < (unsigned)size(d)) ? &(d[i]) : NULL;
    }
    static void set( const value_type& v, data_type& d, int i = 0)
    {
        if ((unsigned)i < (unsigned)size(d))
            d[i] = v;
    }
    static void resize( int /*s*/, data_type& /*d*/)
    {
    }
};

template<class T>
class data_widget_container < Quater<T> > : public fixed_vector_data_widget_container < Quater<T> >
{};


////////////////////////////////////////////////////////////////
/// sofa::defaulttype::Mat support
////////////////////////////////////////////////////////////////

template<int L, int C, class T>
class vector_data_trait < sofa::defaulttype::Mat<L, C, T> >
{
public:
    typedef sofa::defaulttype::Mat<L, C, T> data_type;
    typedef typename data_type::Line value_type;
    enum { NDIM = 1 };
    enum { SIZE = L };
    static int size(const data_type&) { return SIZE; }
    static const char* header(const data_type& d, int i = 0)
    {
        return NULL;
    }
    static const value_type* get(const data_type& d, int i = 0)
    {
        return ((unsigned)i < (unsigned)size(d)) ? &(d[i]) : NULL;
    }
    static void set( const value_type& v, data_type& d, int i = 0)
    {
        if ((unsigned)i < (unsigned)size(d))
            d[i] = v;
    }
    static void resize( int /*s*/, data_type& /*d*/)
    {
    }
};

template<int L, int C, class T>
class data_widget_container < sofa::defaulttype::Mat<L, C, T> > : public fixed_grid_data_widget_container < sofa::defaulttype::Mat<L, C, T> >
{};

} // namespace qt

} // namespace gui

} // namespace sofa


#endif
