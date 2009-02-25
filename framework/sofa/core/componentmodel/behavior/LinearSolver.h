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
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_LINEARSOLVER_H
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_LINEARSOLVER_H

#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalState.h>
#include <sofa/defaulttype/BaseMatrix.h>
#include <sofa/defaulttype/BaseVector.h>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

/**
 *  \brief Abstract interface for linear system solvers
 *
 */
class SOFA_CORE_API LinearSolver : public virtual objectmodel::BaseObject
{
public:
    typedef BaseMechanicalState::VecId VecId;

    LinearSolver();

    virtual ~LinearSolver();

    /// Reset the current linear system.
    virtual void resetSystem() = 0;

    /// Set the linear system matrix, combining the mechanical M,B,K matrices using the given coefficients
    ///
    /// @todo Should we put this method in a specialized class for mechanical systems, or express it using more general terms (i.e. coefficients of the second order ODE to solve)
    virtual void setSystemMBKMatrix(double mFact=0.0, double bFact=0.0, double kFact=0.0) = 0;

    /// Set the linear system right-hand term vector, from the values contained in the (Mechanical/Physical)State objects
    virtual void setSystemRHVector(VecId v) = 0;

    /// Set the initial estimate of the linear system left-hand term vector, from the values contained in the (Mechanical/Physical)State objects
    /// This vector will be replaced by the solution of the system once solveSystem is called
    virtual void setSystemLHVector(VecId v) = 0;

    /// Solve the system as constructed using the previous methods
    virtual void solveSystem() = 0;

    ///
    virtual void partial_solve(std::list<int>& /*I_last_Disp*/, std::list<int>& /*I_last_Dforce*/, bool /*NewIn*/) {std::cerr<<"WARNING : partial_solve is not implemented yet"<<std::endl; }

    /// Multiply the inverse of the system matrix by the transpose of the given matrix J
    ///
    /// @param result the variable where the result will be added
    /// @param J the matrix J to use
    /// @return false if the solver does not support this operation, of it the system matrix is not invertible
    virtual bool addMInvJt(defaulttype::BaseMatrix* /*result*/, defaulttype::BaseMatrix* /*J*/, double /*fact*/)
    {
        return false;
    }

    /// Multiply the inverse of the system matrix by the transpose of the given matrix, and multiply the result with the given matrix J
    ///
    /// @param result the variable where the result will be added
    /// @param J the matrix J to use
    /// @return false if the solver does not support this operation, of it the system matrix is not invertible
    virtual bool addJMInvJt(defaulttype::BaseMatrix* /*result*/, defaulttype::BaseMatrix* /*J*/, double /*fact*/)
    {
        return false;
    }

    /// Get the linear system matrix, or NULL if this solver does not build it
    virtual defaulttype::BaseMatrix* getSystemBaseMatrix() { return NULL; }

    /// Get the linear system right-hand term vector, or NULL if this solver does not build it
    virtual defaulttype::BaseVector* getSystemRHBaseVector() { return NULL; }

    /// Get the linear system left-hand term vector, or NULL if this solver does not build it
    virtual defaulttype::BaseVector* getSystemLHBaseVector() { return NULL; }

    /// Get the linear system inverse matrix, or NULL if this solver does not build it
    virtual defaulttype::BaseMatrix* getSystemInverseBaseMatrix() { return NULL; }

protected:
};

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
