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

#include <sofa/component/collision/response/mapper/BaseContactMapper.h>

// SOFA_DEPRECATED_HEADER("v22.06", "v23.06", "sofa/component/collision/response/mapper/BaseContactMapper.h")

namespace sofa::component::collision
{
    template <class TDataTypes>
    using BaseContactMapper = sofa::component::collision::response::mapper::BaseContactMapper<TDataTypes>;    

    template<class TCollisionModel, class DataTypes>
    using ContactMapper = sofa::component::collision::response::mapper::ContactMapper<TCollisionModel, DataTypes>;

    template<class Mapper>
    using ContactMapperCreator = sofa::component::collision::response::mapper::ContactMapperCreator<Mapper>;

} // namespace sofa::component::collision
