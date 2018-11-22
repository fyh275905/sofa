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
#define FRAME_COROTATIONALFORCEFIELD_CPP

#include "CorotationalForceField.inl"
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;


// Register in the Factory
int CorotationalForceFieldClass = core::RegisterObject("Compute corotational forces on deformation gradients")
#ifndef SOFA_FLOAT
        .add< CorotationalForceField<DeformationGradient331dTypes> >()
        .add< CorotationalForceField<DeformationGradient332dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< CorotationalForceField<DeformationGradient331fTypes> >()
        .add< CorotationalForceField<DeformationGradient332fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class SOFA_FRAME_API CorotationalForceField<DeformationGradient331dTypes>;
template class SOFA_FRAME_API CorotationalForceField<DeformationGradient332dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_FRAME_API CorotationalForceField<DeformationGradient331fTypes>;
template class SOFA_FRAME_API CorotationalForceField<DeformationGradient332fTypes>;
#endif



}
}

} // namespace sofa


