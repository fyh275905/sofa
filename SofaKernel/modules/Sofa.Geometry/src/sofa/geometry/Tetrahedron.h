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

#include <sofa/type/fixed_array_algorithms.h>
#include <sofa/type/vector_algebra.h>
#include <sofa/type/Vec.h>
#include <iterator>

namespace sofa::geometry
{

struct Tetrahedron
{
    static const sofa::Size NumberOfNodes = 4;

    Tetrahedron() = default;

    /**
    * @brief	Compute the volume of a tetrahedron
    * @remark	This function is not generic
    * @tparam   Node a container of the type sofa::type::Vec3 (needed for cross(), dot(), operator-)
    * @tparam   T scalar
    * @param	n0,n1,n2,n3,n4 nodes of the tetrahedron
    * @return	Volume of the hexahedron (a T scalar)
    */
    template<typename Node,
             typename T = std::decay_t<decltype(*std::begin(std::declval<Node>()))>,
             typename = std::enable_if_t<std::is_scalar_v<T>>
    >
    static constexpr auto volume(const Node& n0, const Node& n1, const Node& n2, const Node& n3)
    {
        constexpr Node n{};
        //static_assert(std::distance(std::begin(n), std::end(n)) == 3, "volume can only be computed in 3 dimensions.");

        if constexpr (std::distance(std::begin(n), std::end(n)) == 3)
        {
            const auto a = n1 - n0;
            const auto b = n2 - n0;
            const auto c = n3 - n0;

            return std::abs(sofa::type::dot(sofa::type::cross(a, b), c) / static_cast<T>(6));
        }
        else
        {
            //does not make sense to compute volume other than 3D
            //but some code effectively wants 2d volumes(??)
            return static_cast<T>(0);
        }
    }
};

} // namespace sofa::geometry
