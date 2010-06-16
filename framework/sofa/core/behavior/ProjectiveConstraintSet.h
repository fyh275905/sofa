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
#ifndef SOFA_CORE_BEHAVIOR_PROJECTIVECONSTRAINTSET_H
#define SOFA_CORE_BEHAVIOR_PROJECTIVECONSTRAINTSET_H

#include <sofa/core/core.h>
#include <sofa/core/behavior/BaseProjectiveConstraintSet.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/behavior/MultiMatrixAccessor.h>

#include <sofa/defaulttype/BaseMatrix.h>
#include <sofa/defaulttype/BaseVector.h>

#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>

namespace sofa
{

namespace core
{

namespace behavior
{

/**
 *  \brief Component computing constraints within a simulated body.
 *
 *  This class define the abstract API common to constraints using a given type
 *  of DOFs.
 *  A ProjectiveConstraintSet computes constraints applied to one simulated body given its
 *  current position and velocity.
 *
 */
template<class DataTypes>
class ProjectiveConstraintSet : public BaseProjectiveConstraintSet
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(ProjectiveConstraintSet,DataTypes), BaseProjectiveConstraintSet);

    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::VecConst VecConst;
    typedef typename DataTypes::SparseVecDeriv SparseVecDeriv;

    ProjectiveConstraintSet(MechanicalState<DataTypes> *mm = NULL);

    virtual ~ProjectiveConstraintSet();

    Data<Real> endTime;  ///< Time when the constraint becomes inactive (-1 for infinitely active)
    virtual bool isActive() const; ///< if false, the constraint does nothing

    virtual void init();

    /// Retrieve the associated MechanicalState
    MechanicalState<DataTypes>* getMState() { return mstate; }

    /// @name Vector operations
    /// @{

    /// Project dx to constrained space (dx models an acceleration).
    ///
    /// This method retrieves the dx vector from the MechanicalState and call
    /// the internal projectResponse(VecDeriv&) method implemented by
    /// the component.
    virtual void projectResponse();

    /// Project the L matrix of the Lagrange Multiplier equation system.
    ///
    /// This method retrieves the lines of the Jacobian Matrix from the MechanicalState and call
    /// the internal projectResponse(SparseVecDeriv&) method implemented by
    /// the component.
    virtual void projectJacobianMatrix();

    /// Project v to constrained space (v models a velocity).
    ///
    /// This method retrieves the v vector from the MechanicalState and call
    /// the internal projectVelocity(VecDeriv&) method implemented by
    /// the component.
    virtual void projectVelocity();

    /// Project x to constrained space (x models a position).
    ///
    /// This method retrieves the x vector from the MechanicalState and call
    /// the internal projectPosition(VecCoord&) method implemented by
    /// the component.
    virtual void projectPosition();

    /// Project vFree to constrained space (vFree models a velocity).
    ///
    /// This method retrieves the vFree vector from the MechanicalState and call
    /// the internal projectVelocity(VecDeriv&) method implemented by
    /// the component.
    virtual void projectFreeVelocity();

    /// Project xFree to constrained space (xFree models a position).
    ///
    /// This method retrieves the xFree vector from the MechanicalState and call
    /// the internal projectPosition(VecCoord&) method implemented by
    /// the component.
    virtual void projectFreePosition();


    /// Project dx to constrained space (dx models an acceleration).
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic ProjectiveConstraintSet::projectResponse() method.
    virtual void projectResponse(VecDeriv& dx) = 0;
    /// This method must be implemented by the component to handle Lagrange Multiplier based constraint
    virtual void projectResponse(SparseVecDeriv& dx) = 0;

    /// Project v to constrained space (v models a velocity).
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic ProjectiveConstraintSet::projectVelocity() method.
    virtual void projectVelocity(VecDeriv& v) = 0;

    /// Project x to constrained space (x models a position).
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic ProjectiveConstraintSet::projectPosition() method.
    virtual void projectPosition(VecCoord& x) = 0;

    /// @}

    /// Project the global Mechanical Matrix to constrained space using offset parameter
    virtual void applyConstraint(const sofa::core::behavior::MultiMatrixAccessor* /*matrix*/) {};
    virtual void applyConstraint(defaulttype::BaseMatrix * /*mat*/, unsigned int & /*offset*/) {};
    /// Project the global Mechanical Vector to constrained space using offset parameter
    virtual void applyConstraint(defaulttype::BaseVector* /*vector*/, const sofa::core::behavior::MultiMatrixAccessor* /*matrix*/) {};

    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T*& obj, objectmodel::BaseContext* context, objectmodel::BaseObjectDescription* arg)
    {
        if (dynamic_cast<MechanicalState<DataTypes>*>(context->getMechanicalState()) == NULL)
            return false;
        return BaseObject::canCreate(obj, context, arg);
    }

    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const ProjectiveConstraintSet<DataTypes>* = NULL)
    {
        return DataTypes::Name();
    }

protected:
    MechanicalState<DataTypes> *mstate;
};

#if defined(WIN32) && !defined(SOFA_BUILD_CORE)
extern template class SOFA_CORE_API ProjectiveConstraintSet<defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API ProjectiveConstraintSet<defaulttype::Vec2dTypes>;
extern template class SOFA_CORE_API ProjectiveConstraintSet<defaulttype::Vec1dTypes>;
extern template class SOFA_CORE_API ProjectiveConstraintSet<defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API ProjectiveConstraintSet<defaulttype::Rigid2dTypes>;

extern template class SOFA_CORE_API ProjectiveConstraintSet<defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API ProjectiveConstraintSet<defaulttype::Vec2fTypes>;
extern template class SOFA_CORE_API ProjectiveConstraintSet<defaulttype::Vec1fTypes>;
extern template class SOFA_CORE_API ProjectiveConstraintSet<defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API ProjectiveConstraintSet<defaulttype::Rigid2fTypes>;
#endif

} // namespace behavior

} // namespace core

} // namespace sofa

#endif
