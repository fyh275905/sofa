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
#ifndef SOFA_CORE_BEHAVIOR_PAIRINTERACTIONPROJECTIVECONSTRAINTSET_H
#define SOFA_CORE_BEHAVIOR_PAIRINTERACTIONPROJECTIVECONSTRAINTSET_H

#include <sofa/core/core.h>
#include <sofa/core/behavior/InteractionProjectiveConstraintSet.h>
#include <sofa/core/behavior/MechanicalState.h>

#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>

namespace sofa
{

namespace core
{

namespace behavior
{

/**
 *  \brief Component computing constraints between a pair of simulated body.
 *
 *  This class define the abstract API common to interaction constraints
 *  between a pair of bodies using a given type of DOFs.
 */
template<class TDataTypes>
class PairInteractionProjectiveConstraintSet : public InteractionProjectiveConstraintSet
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(PairInteractionProjectiveConstraintSet,TDataTypes), InteractionProjectiveConstraintSet);

    typedef TDataTypes DataTypes;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::VecConst VecConst;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;
    typedef core::behavior::BaseMechanicalState::ParticleMask ParticleMask;

    PairInteractionProjectiveConstraintSet(MechanicalState<DataTypes> *mm1 = NULL, MechanicalState<DataTypes> *mm2 = NULL);

    virtual ~PairInteractionProjectiveConstraintSet();

    Data<double> endTime;  ///< Time when the constraint becomes inactive (-1 for infinitely active)
    virtual bool isActive() const; ///< if false, the constraint does nothing

    virtual void init();

    /// Retrieve the associated MechanicalState
    MechanicalState<DataTypes>* getMState1() { return mstate1; }
    BaseMechanicalState* getMechModel1() { return mstate1; }
    /// Retrieve the associated MechanicalState
    MechanicalState<DataTypes>* getMState2() { return mstate2; }
    BaseMechanicalState* getMechModel2() { return mstate2; }

    /// @name Vector operations
    /// @{

    /// Project dx to constrained space (dx models an acceleration).
    ///
    /// This method retrieves the dx vector from the MechanicalState and call
    /// the internal projectResponse(VecDeriv&,VecDeriv&) method implemented by
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
    /// the internal projectVelocity(VecDeriv&,VecDeriv&) method implemented by
    /// the component.
    virtual void projectVelocity();

    /// Project x to constrained space (x models a position).
    ///
    /// This method retrieves the x vector from the MechanicalState and call
    /// the internal projectPosition(VecCoord&,VecCoord&) method implemented by
    /// the component.
    virtual void projectPosition();

    /// Project vFree to constrained space (vFree models a velocity).
    ///
    /// This method retrieves the vFree vector from the MechanicalState and call
    /// the internal projectVelocity(VecDeriv&,VecDeriv&) method implemented by
    /// the component.
    virtual void projectFreeVelocity();

    /// Project xFree to constrained space (xFree models a position).
    ///
    /// This method retrieves the xFree vector from the MechanicalState and call
    /// the internal projectPosition(VecCoord&,VecCoord&) method implemented by
    /// the component.
    virtual void projectFreePosition();

    /// Project dx to constrained space (dx models an acceleration).
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic Constraint::projectResponse() method.
    virtual void projectResponse(VecDeriv& dx1, VecDeriv& dx2) = 0;

    /// Project v to constrained space (v models a velocity).
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic Constraint::projectVelocity() method.
    virtual void projectVelocity(VecDeriv& v1, VecDeriv& v2) = 0;

    /// Project x to constrained space (x models a position).
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic Constraint::projectPosition() method.
    virtual void projectPosition(VecCoord& x1, VecCoord& x2) = 0;

    /// @}

    /// \todo What is the difference with BaseConstraint::applyConstraint(unsigned int&, double&) ?
    virtual void applyConstraint(unsigned int & contactId); // Pure virtual would be better

    virtual void applyConstraint(VecConst& /*c1*/, VecConst& /*c2*/, unsigned int & /*contactId*/) {}

    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T*& obj, objectmodel::BaseContext* context, objectmodel::BaseObjectDescription* arg)
    {
        if (arg->getAttribute("object1") || arg->getAttribute("object2"))
        {
            if (dynamic_cast<MechanicalState<DataTypes>*>(arg->findObject(arg->getAttribute("object1",".."))) == NULL)
                return false;
            if (dynamic_cast<MechanicalState<DataTypes>*>(arg->findObject(arg->getAttribute("object2",".."))) == NULL)
                return false;
        }
        else
        {
            if (dynamic_cast<MechanicalState<DataTypes>*>(context->getMechanicalState()) == NULL)
                return false;
        }
        return InteractionProjectiveConstraintSet::canCreate(obj, context, arg);
    }

    /// Construction method called by ObjectFactory.
    template<class T>
    static void create(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        core::behavior::InteractionProjectiveConstraintSet::create(obj, context, arg);
        if (arg && (arg->getAttribute("object1") || arg->getAttribute("object2")))
        {
            obj->mstate1 = dynamic_cast<MechanicalState<DataTypes>*>(arg->findObject(arg->getAttribute("object1","..")));
            obj->mstate2 = dynamic_cast<MechanicalState<DataTypes>*>(arg->findObject(arg->getAttribute("object2","..")));
        }
        else if (context)
        {
            obj->mstate1 =
                obj->mstate2 =
                        dynamic_cast<MechanicalState<DataTypes>*>(context->getMechanicalState());
        }
    }

    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const PairInteractionProjectiveConstraintSet<DataTypes>* = NULL)
    {
        return DataTypes::Name();
    }

protected:
    Data< std::string > object1;
    Data< std::string > object2;
    MechanicalState<DataTypes> *mstate1;
    MechanicalState<DataTypes> *mstate2;
    ParticleMask *mask1;
    ParticleMask *mask2;
};

#if defined(WIN32) && !defined(SOFA_BUILD_CORE)
extern template class SOFA_CORE_API PairInteractionProjectiveConstraintSet<defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API PairInteractionProjectiveConstraintSet<defaulttype::Vec2dTypes>;
extern template class SOFA_CORE_API PairInteractionProjectiveConstraintSet<defaulttype::Vec1dTypes>;
extern template class SOFA_CORE_API PairInteractionProjectiveConstraintSet<defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API PairInteractionProjectiveConstraintSet<defaulttype::Rigid2dTypes>;

extern template class SOFA_CORE_API PairInteractionProjectiveConstraintSet<defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API PairInteractionProjectiveConstraintSet<defaulttype::Vec2fTypes>;
extern template class SOFA_CORE_API PairInteractionProjectiveConstraintSet<defaulttype::Vec1fTypes>;
extern template class SOFA_CORE_API PairInteractionProjectiveConstraintSet<defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API PairInteractionProjectiveConstraintSet<defaulttype::Rigid2fTypes>;
#endif

} // namespace behavior

} // namespace core

} // namespace sofa

#endif
