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
#include <sofa/component/mapping/LaparoscopicRigidMapping.inl>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/LaparoscopicRigidTypes.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/componentmodel/behavior/MappedModel.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/componentmodel/behavior/MechanicalMapping.inl>
#include <sofa/core/Mapping.inl>

namespace sofa
{

namespace component
{

namespace mapping
{

SOFA_DECL_CLASS(LaparoscopicRigidMapping)

using namespace sofa::defaulttype;
using namespace core;
using namespace core::componentmodel::behavior;

// Register in the Factory
int LaparoscopicRigidMappingClass = core::RegisterObject("TODO-LaparoscopicRigidMappingClass")
        .add< LaparoscopicRigidMapping< MechanicalMapping< MechanicalState<LaparoscopicRigidTypes>, MechanicalState<RigidTypes> > > >()
        .add< LaparoscopicRigidMapping< Mapping< State<LaparoscopicRigidTypes>, MappedModel<RigidTypes> > > >()
        ;

template class LaparoscopicRigidMapping< MechanicalMapping<MechanicalState<LaparoscopicRigidTypes>, MechanicalState<RigidTypes> > >;
template class LaparoscopicRigidMapping< Mapping< State<LaparoscopicRigidTypes>, MappedModel<RigidTypes> > >;

} // namespace mapping

} // namespace component

} // namespace sofa

