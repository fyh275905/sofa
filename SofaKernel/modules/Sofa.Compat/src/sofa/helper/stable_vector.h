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

#include <sofa/type/stable_vector.h>

// The following SOFA_DEPRECATED_HEADER is commented to avoid a massive number of warnings.
// This flag will be enabled once all the code base in Sofa is ported to Sofa.Type.
//SOFA_DEPRECATED_HEADER("v21.06", "v21.12", "sofa/type/stable_vector.h")

namespace sofa::helper
{
    template<class T, class A = std::allocator<T>>
    using stable_vector = sofa::type::stable_vector<T, A>;

} // namespace sofa::helper
