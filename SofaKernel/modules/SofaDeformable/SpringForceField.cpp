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
#define SOFA_COMPONENT_FORCEFIELD_SPRINGFORCEFIELD_CPP
#include <SofaDeformable/SpringForceField.inl>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/objectmodel/DataTypeInfoHelper.h>

REGISTER_TYPE_WITHOUT_DATATYPEINFO(sofa::helper::vector<sofa::component::interactionforcefield::LinearSpring<SReal>>);

namespace sofa
{

namespace component
{

namespace interactionforcefield
{

using namespace sofa::defaulttype;


//Register in the Factory
int SpringForceFieldClass = core::RegisterObject("Springs")
        .add< SpringForceField<Vec3Types> >()
        .add< SpringForceField<Vec2Types> >()
        .add< SpringForceField<Vec1Types> >()
        .add< SpringForceField<Vec6Types> >()
        .add< SpringForceField<Rigid3Types> >()
        ;

template class SOFA_DEFORMABLE_API LinearSpring<SReal>;
template class SOFA_DEFORMABLE_API SpringForceField<Vec3Types>;
template class SOFA_DEFORMABLE_API SpringForceField<Vec2Types>;
template class SOFA_DEFORMABLE_API SpringForceField<Vec1Types>;
template class SOFA_DEFORMABLE_API SpringForceField<Vec6Types>;
template class SOFA_DEFORMABLE_API SpringForceField<Rigid3Types>;

} // namespace interactionforcefield

} // namespace component

} // namespace sofa

