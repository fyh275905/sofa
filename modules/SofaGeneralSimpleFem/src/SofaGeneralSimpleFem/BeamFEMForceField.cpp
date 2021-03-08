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
#define SOFA_COMPONENT_FORCEFIELD_BEAMFEMFORCEFIELD_CPP
#include <SofaGeneralSimpleFem/BeamFEMForceField.inl>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/reflection/ClassInfoBuilder.h>

namespace sofa::component::forcefield::_beamfemforcefield_
{

using namespace sofa::defaulttype;

/// Register the container interfaces into the class info registry
auto a = sofa::core::reflection::ClassInfoBuilder::GetOrBuildClassInfo<sofa::component::container::StiffnessContainer>(sofa_tostring(SOFA_TARGET));

/// Register the container interfaces into the class info registry
auto b = sofa::core::reflection::ClassInfoBuilder::GetOrBuildClassInfo<sofa::component::container::PoissonContainer>(sofa_tostring(SOFA_TARGET));

/// Register in the Factory
int BeamFEMForceFieldClass = core::RegisterObject("Beam finite elements")
        .add< BeamFEMForceField<Rigid3Types> >()
        ;

template class SOFA_SOFAGENERALSIMPLEFEM_API BeamFEMForceField<Rigid3Types>;


} // namespace sofa::component::forcefield::_beamfemforcefield_
