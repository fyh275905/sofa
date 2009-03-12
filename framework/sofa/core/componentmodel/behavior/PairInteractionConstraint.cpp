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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include "PairInteractionConstraint.inl"

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

using namespace sofa::defaulttype;

template class SOFA_CORE_API PairInteractionConstraint<Vec3dTypes>;
template class SOFA_CORE_API PairInteractionConstraint<Vec2dTypes>;
template class SOFA_CORE_API PairInteractionConstraint<Vec1dTypes>;
template class SOFA_CORE_API PairInteractionConstraint<Rigid3dTypes>;
template class SOFA_CORE_API PairInteractionConstraint<Rigid2dTypes>;

template class SOFA_CORE_API PairInteractionConstraint<Vec3fTypes>;
template class SOFA_CORE_API PairInteractionConstraint<Vec2fTypes>;
template class SOFA_CORE_API PairInteractionConstraint<Vec1fTypes>;
template class SOFA_CORE_API PairInteractionConstraint<Rigid3fTypes>;
template class SOFA_CORE_API PairInteractionConstraint<Rigid2fTypes>;



} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa
