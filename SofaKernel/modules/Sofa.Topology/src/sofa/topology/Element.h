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
#pragma once

#include <sofa/topology/config.h>

#include <sofa/type/stdtype/fixed_array.h>
#include <sofa/topology/Point.h>

#include <type_traits>

namespace sofa::topology
{

template <typename GeometryElement>
struct Element : public sofa::type::stdtype::fixed_array<sofa::Index, GeometryElement::NumberOfNodes>
{
    constexpr Element() noexcept
    {
        std::fill(this->begin(), this->end(), sofa::InvalidID);
    }

    template< typename... ArgsT
        , typename = std::enable_if_t<std::conjunction<std::is_convertible<ArgsT, sofa::Index>...>::value>
        , typename = std::enable_if_t< (GeometryElement::NumberOfNodes == sizeof...(ArgsT)) >
    >
        constexpr Element(ArgsT&&... args) noexcept
        : sofa::type::stdtype::fixed_array< sofa::Index, GeometryElement::NumberOfNodes >
    { static_cast<sofa::Index>(std::forward< ArgsT >(args))... }
    {}
};

} // namespace sofa::geometry
