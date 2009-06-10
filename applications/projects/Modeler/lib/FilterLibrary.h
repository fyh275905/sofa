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
#ifndef SOFA_FILTERLIBRARY_H
#define SOFA_FILTERLIBRARY_H

#include <iostream>
#include <sofa/core/ComponentLibrary.h>


#ifdef SOFA_QT4
#include <Q3Header>
#include <QLineEdit>
#else
#include <qheader.h>
#include <qlineedit.h>
#endif


using sofa::core::ComponentLibrary;

namespace sofa
{

namespace gui
{

namespace qt
{


class FilterQuery
{
public:
    FilterQuery( const std::string &query);

    bool isValid( const ComponentLibrary* component) const ;

protected:
    void decodeQuery();

    std::string query;

    std::vector< QString > components;
    std::vector< QString > templates;
    std::vector< QString > licenses;
    std::vector< QString > authors;
};

//***************************************************************
class FilterLibrary : public QLineEdit
{
    Q_OBJECT
public:
    FilterLibrary( QWidget* parent);

public slots:
    void searchText(const QString&);
protected:
    std::string help;
signals:
    void filterList(const FilterQuery&);
};

}
}
}

#endif
