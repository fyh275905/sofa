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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_SIMULATION_VelocityThresholdVisitor_H
#define SOFA_SIMULATION_VelocityThresholdVisitor_H


#include <sofa/simulation/common/Visitor.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalState.h>

namespace sofa
{

namespace simulation
{


using namespace sofa::defaulttype;
class VelocityThresholdVisitor : public Visitor
{
public:
    typedef sofa::core::componentmodel::behavior::BaseMechanicalState::VecId VecId;

    virtual Visitor::Result processNodeTopDown(simulation::Node* node);

    VelocityThresholdVisitor( VecId v, double threshold );



    /// Return a category name for this action.
    /// Only used for debugging / profiling purposes
    virtual const char* getCategoryName() const
    {
        return "threshold";
    }

protected:
    VecId vid; ///< Id of the vector to process
    double threshold; ///< All the entries below this threshold will be set to 0.
};

} // namespace simulation

} // namespace sofa

#endif
