/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_INTERACTIONCONSTRAINT_H
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_INTERACTIONCONSTRAINT_H

#include <sofa/core/componentmodel/behavior/Constraint.h>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

/**
 *  \brief InteractionConstraint is a constraint linking several bodies (MechanicalState) together.
 *
 *  An InteractionConstraint computes constraints applied to several simulated
 *  bodies given their current positions and velocities.
 *
 */
class SOFA_CORE_API InteractionConstraint : public BaseConstraint
{
public:

    /// Get the first MechanicalState
    /// \todo Rename to getMechState1()
    /// \todo Replace with an accessor to a list of states, as an InteractionConstraint can be applied to more than two.
    virtual BaseMechanicalState* getMechModel1() = 0;

    /// Get the first MechanicalState
    /// \todo Rename to getMechState2()
    /// \todo Replace with an accessor to a list of states, as an InteractionConstraint can be applied to more than two.
    virtual BaseMechanicalState* getMechModel2() = 0;
};

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
