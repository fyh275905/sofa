/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
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
#define SOFA_COMPONENT_LINEARSOLVER_BTDLINEARSOLVER_CPP
#include <SofaBaseLinearSolver/BTDLinearSolver.inl>

namespace sofa
{

namespace component
{

namespace linearsolver
{

SOFA_DECL_CLASS(BTDLinearSolver)

//.add< BTDLinearSolver<BTDMatrix<3,double>,BlockVector<3,double> > >()
//.add< BTDLinearSolver<BTDMatrix<3,float>,BlockVector<3,float> > >()
//.add< BTDLinearSolver<BTDMatrix<2,double>,BlockVector<2,double> > >()
//.add< BTDLinearSolver<BTDMatrix<2,float>,BlockVector<2,float> > >()
//.add< BTDLinearSolver<BTDMatrix<1,double>,BlockVector<1,double> > >()
//.add< BTDLinearSolver<BTDMatrix<1,float>,BlockVector<1,float> > >()
//.add< BTDLinearSolver<NewMatMatrix,NewMatVector> >()
//.add< BTDLinearSolver<NewMatSymmetricMatrix,NewMatVector> >()
//.add< BTDLinearSolver<NewMatBandMatrix,NewMatVector> >(true)
//.add< BTDLinearSolver<NewMatSymmetricBandMatrix,NewMatVector> >()
        ;

#ifndef SOFA_FLOAT
template<> const char* BTDMatrix<1,double>::Name() { return "BTDMatrix1d"; }
template<> const char* BTDMatrix<2,double>::Name() { return "BTDMatrix2d"; }
template<> const char* BTDMatrix<3,double>::Name() { return "BTDMatrix3d"; }
template<> const char* BTDMatrix<4,double>::Name() { return "BTDMatrix4d"; }
template<> const char* BTDMatrix<5,double>::Name() { return "BTDMatrix5d"; }
template<> const char* BTDMatrix<6,double>::Name() { return "BTDMatrix6d"; }
#endif

#ifndef SOFA_DOUBLE
template<> const char* BTDMatrix<1,float>::Name() { return "BTDMatrix1f"; }
template<> const char* BTDMatrix<2,float>::Name() { return "BTDMatrix2f"; }
template<> const char* BTDMatrix<3,float>::Name() { return "BTDMatrix3f"; }
template<> const char* BTDMatrix<4,float>::Name() { return "BTDMatrix4f"; }
template<> const char* BTDMatrix<5,float>::Name() { return "BTDMatrix5f"; }
template<> const char* BTDMatrix<6,float>::Name() { return "BTDMatrix6f"; }
#endif

#ifndef SOFA_FLOAT
template class SOFA_BASE_LINEAR_SOLVER_API BTDLinearSolver<BTDMatrix<6,float>,BlockVector<6,float> >;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_BASE_LINEAR_SOLVER_API BTDLinearSolver<BTDMatrix<6,double>,BlockVector<6,double> >;
#endif

} // namespace linearsolver

} // namespace component

} // namespace sofa

