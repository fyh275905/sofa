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

#include <gtest/gtest.h>

#include <sofa/defaulttype/AbstractTypeInfo.h>
using sofa::defaulttype::AbstractTypeInfo;

#include <sofa/defaulttype/typeinfo/models/IncompleteTypeInfo.h>
using sofa::defaulttype::IncompleteTypeInfo;

#include <sofa/defaulttype/typeinfo/DataTypeInfoDynamicWrapper.h>
using sofa::defaulttype::DataTypeInfo;
using sofa::defaulttype::DataTypeInfoDynamicWrapper;

#include <sofa/defaulttype/TypeInfoID.h>
using sofa::defaulttype::TypeInfoId;

#include <sofa/defaulttype/TypeInfoRegistry.h>
using sofa::defaulttype::TypeInfoRegistry;
using sofa::defaulttype::TypeInfoType;

#include <sofa/defaulttype/TypeInfoRegistryTools.h>
using sofa::defaulttype::TypeInfoRegistryTools;


class MyTypeNotRegistered {};
TEST(TypeInfoRegistry, get)
{
    const AbstractTypeInfo* nfo = TypeInfoRegistry::Get(TypeInfoId::getTypeId<MyTypeNotRegistered>());
    EXPECT_NE(nfo, nullptr);
    EXPECT_FALSE(nfo->ValidInfo());
    EXPECT_EQ(nfo->name(), std::string("NoTypeInfo"));
}

class MyType {};
template<> class sofa::defaulttype::DataTypeInfo<MyType> : public IncompleteTypeInfo<MyType>
{
public:
    static std::string name(){ return "MyType"; }
    static std::string GetTypeName(){ return "MyType"; }
};

TEST(TypeInfoRegistry, set_and_get)
{
    TypeInfoRegistry::Set(TypeInfoId::getTypeId<MyType>(),
                          DataTypeInfoDynamicWrapper<DataTypeInfo<MyType>>::get(), "TestTarget");

    const AbstractTypeInfo* nfo = TypeInfoRegistry::Get(TypeInfoId::getTypeId<MyType>());
    EXPECT_NE(nfo, nullptr);
    EXPECT_FALSE(nfo->ValidInfo());
    EXPECT_EQ(nfo->name(), std::string("MyType"));
}

TEST(TypeInfoRegistry, dump)
{
    TypeInfoRegistryTools::dumpRegistryContentToStream(std::cout, TypeInfoType::NONE);
    TypeInfoRegistryTools::dumpRegistryContentToStream(std::cout, TypeInfoType::PARTIAL);
    TypeInfoRegistryTools::dumpRegistryContentToStream(std::cout, TypeInfoType::ALL);
}
