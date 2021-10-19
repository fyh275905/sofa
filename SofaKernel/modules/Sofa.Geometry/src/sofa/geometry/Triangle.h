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

#include <sofa/geometry/Edge.h>

namespace sofa::geometry
{

struct Triangle
{
    static const sofa::Size NumberOfNodes = 3;

    Triangle() = default;

    template<typename Node,
        typename T = std::decay_t<decltype(*std::begin(std::declval<Node>()))>,
        typename = std::enable_if_t<std::is_scalar_v<T>>>
        static constexpr auto area(const Node& n0, const Node& n1, const Node& n2)
    {
        const auto a = sofa::geometry::Edge::length(n0, n1);
        const auto b = sofa::geometry::Edge::length(n0, n2);
        const auto c = sofa::geometry::Edge::length(n1, n2);

        const auto area = 0.25 * std::sqrt((a + b + c) * (-a + b + c) * (a - b + c) * (a + b - c));
        return area;
    }
};

} // namespace sofa::geometry
