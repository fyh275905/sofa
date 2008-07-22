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
#ifndef SOFA_SIMULATION_MECHANICALVPRINTACTION_H
#define SOFA_SIMULATION_MECHANICALVPRINTACTION_H

#include <sofa/simulation/common/Visitor.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalState.h>
#include <iostream>

namespace sofa
{

namespace simulation
{


/** Print a vector */
class MechanicalVPrintVisitor : public Visitor
{
public:
    typedef sofa::core::componentmodel::behavior::BaseMechanicalState::VecId VecId;

    MechanicalVPrintVisitor( VecId v, std::ostream& =std::cerr );
    virtual Result processNodeTopDown(simulation::Node* node);

protected:
    VecId v_;
    std::ostream& out_;
};


/** Print a vector with an elapsed time, useful to compare convergence in odeSolver in function of time */
class MechanicalVPrintWithElapsedTimeVisitor : public Visitor
{
    typedef sofa::core::componentmodel::behavior::BaseMechanicalState::VecId VecId;
protected:
    VecId v_;
    unsigned count_;
    unsigned time_;
    std::ostream& out_;
public:
    MechanicalVPrintWithElapsedTimeVisitor( VecId v, unsigned time, std::ostream& =std::cerr );
    virtual Result processNodeTopDown(simulation::Node* node);
};

} // namespace simulation

} // namespace sofa

#endif
