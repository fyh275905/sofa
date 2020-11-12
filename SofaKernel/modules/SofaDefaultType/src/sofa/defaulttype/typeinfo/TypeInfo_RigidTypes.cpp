/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/defaulttype/typeinfo/TypeInfo_RigidTypes.h>
#include <sofa/defaulttype/DataTypeInfoRegistry.h>

namespace sofa::defaulttype
{

#define REGISTER_RIGIDCOORD(theInnerType, size) static int REGISTER_UNIQUE_NAME_GENERATOR(theInnerName , __LINE__) = DataTypeInfoRegistry::Set(DataTypeId<RigidCoord<theInnerType, size>>::getTypeId(), \
                                                VirtualTypeInfoA< DataTypeInfo<RigidCoord<theInnerType,size>>>::get(),\
                                                sofa_tostring(SOFA_TARGET));
#define REGISTER_RIGIDDERIV(theInnerType, size) static int REGISTER_UNIQUE_NAME_GENERATOR(theInnerName , __LINE__) = DataTypeInfoRegistry::Set(DataTypeId<RigidDeriv<theInnerType, size>>::getTypeId(), \
                                                VirtualTypeInfoA< DataTypeInfo<RigidDeriv<theInnerType, size>>>::get(),\
                                                sofa_tostring(SOFA_TARGET));
//REGISTER_RIGIDCOORD(double, 2)
//REGISTER_RIGIDCOORD(float, 2)
//REGISTER_RIGIDCOORD(double, 3)
//REGISTER_RIGIDCOORD(float, 3)

//REGISTER_RIGIDDERIV(double, 2)
//REGISTER_RIGIDDERIV(float, 2)
//REGISTER_RIGIDDERIV(double, 3)
//REGISTER_RIGIDDERIV(float, 3)

REGISTER_TYPE_INFO_CREATOR(Rigid2fTypes)
REGISTER_TYPE_INFO_CREATOR(Rigid2dTypes)
REGISTER_TYPE_INFO_CREATOR(Rigid3fTypes)
REGISTER_TYPE_INFO_CREATOR(Rigid3dTypes)



REGISTER_TYPE_INFO_CREATOR(Rigid2fMass)
REGISTER_TYPE_INFO_CREATOR(Rigid2dMass)
REGISTER_TYPE_INFO_CREATOR(Rigid3fMass)
REGISTER_TYPE_INFO_CREATOR(Rigid3dMass)

REGISTER_TYPE_INFO_CREATOR(Rigid2fTypes)
REGISTER_TYPE_INFO_CREATOR(Rigid2dTypes)
REGISTER_TYPE_INFO_CREATOR(Rigid3fTypes)
REGISTER_TYPE_INFO_CREATOR(Rigid3dTypes)


} /// namespace sofa::defaulttype

