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
#ifndef SOFA_COMPONENT_FEM_MATERIAL_BASEMATERIAL_H
#define SOFA_COMPONENT_FEM_MATERIAL_BASEMATERIAL_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/Mat.h>


namespace sofa
{
namespace core
{
namespace fem
{

using namespace sofa::defaulttype;
/**
 * Generic material class
 */
class BaseMaterial : public virtual core::objectmodel::BaseObject
{
public:
    SOFA_CLASS(BaseMaterial,core::objectmodel::BaseObject);

    BaseMaterial() {}
    virtual ~BaseMaterial() {}


    //virtual VecN computeStress (VecN & strain,int idElement,int id_QP){return stress in the i-th quadrature point}
    //So here needed the shapefunctionvalue *  ,  quadratureformular*  (verifie if shapfunctionvalue compute with the local method)
    // The same principe for computing the strain given the displacement
    //BaseMaterial must be moved to framwork::core::sofa::fem
    virtual void computeStress (Vector3 & ,Vector3 &,unsigned int &) {}
    virtual void computeDStress (Vector3 & ,Vector3 &) {}
};



} // namespace fem

} // namespace core

} // namespace sofa
#endif
