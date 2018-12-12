/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2018 INRIA, USTL, UJF, CNRS, MGH                    *
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
#define SOFA_COMPONENT_FORCEFIELD_HEXAHEDRONFEMFORCEFIELD_CPP
#include "HexahedronFEMForceField.inl"
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/ObjectFactory.h>


namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;


// Register in the Factory
int HexahedronFEMForceFieldClass = core::RegisterObject("Hexahedral finite elements")
#ifdef SOFA_WITH_FLOAT
        .add< HexahedronFEMForceField<Vec3fTypes> >()
#endif
#ifdef SOFA_WITH_DOUBLE
        .add< HexahedronFEMForceField<Vec3dTypes> >()
#endif
        ;

#ifdef SOFA_WITH_FLOAT
template class SOFA_SIMPLE_FEM_API HexahedronFEMForceField<Vec3fTypes>;
#endif
#ifdef SOFA_WITH_DOUBLE
template class SOFA_SIMPLE_FEM_API HexahedronFEMForceField<Vec3dTypes>;
#endif

} // namespace forcefield

} // namespace component

} // namespace sofa

