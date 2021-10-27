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

#include <sofa/geometry/config.h>
#include <cmath>
#include <numeric>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace sofa::geometry
{

struct Edge
{
    static constexpr sofa::Size NumberOfNodes = 2;

    Edge() = default;

    template<typename Node,
        typename T = std::decay_t<decltype(*std::begin(std::declval<Node>()))>,
        typename = std::enable_if_t<std::is_scalar_v<T>>>
        static constexpr auto squaredLength(const Node& n0, const Node& n1)
    {
        constexpr Node v{};
        constexpr auto size = std::distance(std::cbegin(v), std::cend(v));
        // specialized function is faster than the generic (using STL) one
        if constexpr (std::is_same_v< Node, sofa::type::Vec<size, T>>)
        {
            return (static_cast<sofa::type::Vec<size, T>>(n1) - static_cast<sofa::type::Vec<size, T>>(n0)).norm2();
        }
        else
        {
            Node diff{};
            std::transform(n0.cbegin(), n0.cend(), n1.cbegin(), diff.begin(), std::minus<T>());
            return std::inner_product(std::cbegin(diff), std::cend(diff), std::cbegin(diff), static_cast<T>(0));
        }
    }

    template<typename Node,
        typename T = std::decay_t<decltype(*std::begin(std::declval<Node>()))>,
        typename = std::enable_if_t<std::is_scalar_v<T>>>
        static constexpr auto length(const Node& n0, const Node& n1)
    {
        return std::sqrt(squaredLength(n0, n1));
    }
};

} // namespace sofa::geometry
