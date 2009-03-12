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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
// Author: François Faure, INRIA-UJF, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
#include <sofa/component/linearsolver/BTDLinearSolver.h>
#include <sofa/component/linearsolver/NewMatMatrix.h>
#include <sofa/core/ObjectFactory.h>
#include <iostream>
#include "sofa/helper/system/thread/CTime.h"

namespace sofa
{

namespace component
{

namespace linearsolver
{

using namespace sofa::defaulttype;
using namespace sofa::core::componentmodel::behavior;
using namespace sofa::simulation;

SOFA_DECL_CLASS(BTDLinearSolver)

int BTDLinearSolverClass = core::RegisterObject("Linear system solver using Thomas Algorithm for Block Tridiagonal matrices")
        .add< BTDLinearSolver<NewMatMatrix,NewMatVector> >()
        .add< BTDLinearSolver<NewMatSymmetricMatrix,NewMatVector> >()
        .add< BTDLinearSolver<NewMatBandMatrix,NewMatVector> >(true)
        .add< BTDLinearSolver<NewMatSymmetricBandMatrix,NewMatVector> >()
        ;

} // namespace linearsolver

} // namespace component

} // namespace sofa

