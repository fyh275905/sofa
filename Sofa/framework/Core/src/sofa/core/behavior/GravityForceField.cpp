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
#define SOFA_COMPONENT_FORCEFIELD_GRAVITYFORCEFIELD_CPP

// SOFA_ATTRIBUTE_DISABLED("v22.06 (PR#XXXX)", "v22.12", "Transition removing gravity and introducing GravityForceField")
//#include <sofa/component/mechanicalload/GravityForceField.inl> // TO UNCOMMENT
#include "GravityForceField.inl" // TO REMOVE

#include <sofa/core/ObjectFactory.h>

#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>

namespace sofa::component::mechanicalload
{

using namespace sofa::defaulttype;


int GravityForceFieldClass = core::RegisterObject("Gravity as an external force in the world coordinate system")
        .add< GravityForceField<Vec3Types> >()
        .add< GravityForceField<Vec2Types> >()
        .add< GravityForceField<Vec1Types> >()
        .add< GravityForceField<Vec6Types> >()
        .add< GravityForceField<Rigid3Types> >()
        .add< GravityForceField<Rigid2Types> >()

        ;

template class SOFA_CORE_API GravityForceField<Vec3Types>;
template class SOFA_CORE_API GravityForceField<Vec2Types>;
template class SOFA_CORE_API GravityForceField<Vec1Types>;
template class SOFA_CORE_API GravityForceField<Vec6Types>;
template class SOFA_CORE_API GravityForceField<Rigid3Types>;
template class SOFA_CORE_API GravityForceField<Rigid2Types>;

} // namespace sofa::component::mechanicalload
