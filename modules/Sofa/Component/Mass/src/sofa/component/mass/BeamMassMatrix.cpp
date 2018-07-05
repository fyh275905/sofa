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
#define SOFA_COMPONENT_MASS_BEAMMASSMATRIX_CPP

#include <sofa/component/mass/BeamMassMatrix.inl>
#include <sofa/defaulttype/RigidTypes.h>

#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace mass
{

using namespace sofa::defaulttype;

SOFA_DECL_CLASS(BeamMassMatrix)

// Register in the Factory
int BeamMassMatrixClass = core::RegisterObject("Define a specific mass for beam elements")
#ifndef SOFA_FLOAT
		.add< BeamMassMatrix<defaulttype::Rigid3dTypes,double> >()
#endif
#ifndef SOFA_DOUBLE
		.add< BeamMassMatrix<defaulttype::Rigid3fTypes,float> >()
#endif
		;

#ifndef SOFA_FLOAT
template class SOFA_MASS_API BeamMassMatrix<defaulttype::Rigid3dTypes,double>;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_MASS_API BeamMassMatrix<defaulttype::Rigid3fTypes,float>;
#endif


} // namespace mass

} // namespace component

} // namespace sofa
