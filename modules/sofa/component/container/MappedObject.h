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
#ifndef SOFA_COMPONENT_MAPPEDOBJECT_H
#define SOFA_COMPONENT_MAPPEDOBJECT_H

#include <sofa/core/componentmodel/behavior/MappedModel.h>
#include <sofa/core/objectmodel/XDataPtr.h>
#include <sofa/core/objectmodel/VDataPtr.h>
#include <sofa/component/component.h>
#include <vector>
#include <assert.h>
#include <fstream>

namespace sofa
{

namespace component
{

using namespace core::componentmodel::behavior;
using namespace core::objectmodel;

/// This class can be overridden if needed for additionnal storage within template specializations.
template<class DataTypes>
class MappedObjectInternalData
{
public:
};

template <class DataTypes>
class MappedObject : public MappedModel<DataTypes>
{
public:
    typedef MappedModel<DataTypes> Inherited;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;

protected:

    MappedObjectInternalData<DataTypes> data;

public:

    MappedObject();

    virtual ~MappedObject();

    virtual void init();

    Data<VecCoord> f_X;
    Data<VecDeriv> f_V;

    VecCoord* getX()  { return f_X.beginEdit(); }
    VecDeriv* getV()  { return f_V.beginEdit(); }

    const VecCoord* getX()  const { return &f_X.getValue();  }
    const VecDeriv* getV()  const { return &f_V.getValue();  }

};

} // namespace component

} // namespace sofa

#endif
