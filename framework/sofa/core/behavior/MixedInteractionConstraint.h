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
#ifndef SOFA_CORE_BEHAVIOR_MIXEDINTERACTIONCONSTRAINT_H
#define SOFA_CORE_BEHAVIOR_MIXEDINTERACTIONCONSTRAINT_H

#include <sofa/core/core.h>
#include <sofa/core/behavior/InteractionConstraint.h>
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
template<class TDataTypes1, class TDataTypes2>
class  MixedInteractionConstraint : public InteractionConstraint
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(MixedInteractionConstraint,TDataTypes1,TDataTypes2), InteractionConstraint);

    typedef TDataTypes1 DataTypes1;
    typedef typename DataTypes1::VecCoord VecCoord1;
    typedef typename DataTypes1::VecDeriv VecDeriv1;
    typedef typename DataTypes1::VecConst VecConst1;
    typedef typename DataTypes1::Coord Coord1;
    typedef typename DataTypes1::Deriv Deriv1;
    typedef TDataTypes2 DataTypes2;
    typedef typename DataTypes2::VecCoord VecCoord2;
    typedef typename DataTypes2::VecDeriv VecDeriv2;
    typedef typename DataTypes2::VecConst VecConst2;
    typedef typename DataTypes2::Coord Coord2;
    typedef typename DataTypes2::Deriv Deriv2;
    typedef helper::ParticleMask ParticleMask;

    MixedInteractionConstraint(MechanicalState<DataTypes1> *mm1 = NULL, MechanicalState<DataTypes2> *mm2 = NULL);

    virtual ~MixedInteractionConstraint();

    Data<double> endTime;  ///< Time when the constraint becomes inactive (-1 for infinitely active)
    virtual bool isActive() const; ///< if false, the constraint does nothing

    virtual void init();

    /// Retrieve the associated MechanicalState
    MechanicalState<DataTypes1>* getMState1() { return mstate1; }
    BaseMechanicalState* getMechModel1() { return mstate1; }
    /// Retrieve the associated MechanicalState
    MechanicalState<DataTypes2>* getMState2() { return mstate2; }
    BaseMechanicalState* getMechModel2() { return mstate2; }

    /// @name Vector operations
    /// @{

    /// Project dx to constrained space (dx models an acceleration).
    ///
    /// This method retrieves the dx vector from the MechanicalState and call
    /// the internal projectResponse(VecDeriv1&,VecDeriv2&) method implemented by
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
    /// the internal projectVelocity(VecDeriv1&,VecDeriv2&) method implemented by
    /// the component.
    virtual void projectVelocity();

    /// Project x to constrained space (x models a position).
    ///
    /// This method retrieves the x vector from the MechanicalState and call
    /// the internal projectPosition(VecCoord1&,VecCoord2&) method implemented by
    /// the component.
    virtual void projectPosition();

    /// Project vFree to constrained space (vFree models a velocity).
    ///
    /// This method retrieves the vFree vector from the MechanicalState and call
    /// the internal projectVelocity(VecDeriv1&,VecDeriv2&) method implemented by
    /// the component.
    virtual void projectFreeVelocity();

    /// Project xFree to constrained space (xFree models a position).
    ///
    /// This method retrieves the xFree vector from the MechanicalState and call
    /// the internal projectPosition(VecCoord1&,VecCoord2&) method implemented by
    /// the component.
    virtual void projectFreePosition();

    /// Project dx to constrained space (dx models an acceleration).
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic Constraint::projectResponse() method.
    virtual void projectResponse(VecDeriv1& dx1, VecDeriv2& dx2) = 0;

    /// Project v to constrained space (v models a velocity).
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic Constraint::projectVelocity() method.
    virtual void projectVelocity(VecDeriv1& v1, VecDeriv2& v2) = 0;

    /// Project x to constrained space (x models a position).
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic Constraint::projectPosition() method.
    virtual void projectPosition(VecCoord1& x1, VecCoord2& x2) = 0;

    /// @}

    /// \todo What is the difference with BaseConstraint::applyConstraint(unsigned int&, double&) ?
    virtual void applyConstraint(unsigned int & contactId); // Pure virtual would be better

    virtual void applyConstraint(VecConst1& /*c1*/, VecConst2& /*c2*/, unsigned int & /*contactId*/) {}

    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T*& obj, objectmodel::BaseContext* context, objectmodel::BaseObjectDescription* arg)
    {
        if (arg->getAttribute("object1") || arg->getAttribute("object2"))
        {
            if (dynamic_cast<MechanicalState<DataTypes1>*>(arg->findObject(arg->getAttribute("object1",".."))) == NULL)
                return false;
            if (dynamic_cast<MechanicalState<DataTypes2>*>(arg->findObject(arg->getAttribute("object2",".."))) == NULL)
                return false;
        }
        return InteractionConstraint::canCreate(obj, context, arg);
    }

    /// Construction method called by ObjectFactory.
    template<class T>
    static void create(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        core::behavior::InteractionConstraint::create(obj, context, arg);
        if (arg && (arg->getAttribute("object1") || arg->getAttribute("object2")))
        {
            obj->mstate1 = dynamic_cast<MechanicalState<DataTypes1>*>(arg->findObject(arg->getAttribute("object1","..")));
            obj->mstate2 = dynamic_cast<MechanicalState<DataTypes2>*>(arg->findObject(arg->getAttribute("object2","..")));
        }
    }

    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const MixedInteractionConstraint<DataTypes1,DataTypes2>* = NULL)
    {
        return DataTypes1::Name();
    }

protected:
    Data< std::string > object1;
    Data< std::string > object2;
    MechanicalState<DataTypes1> *mstate1;
    MechanicalState<DataTypes2> *mstate2;
    ParticleMask *mask1;
    ParticleMask *mask2;
};

#if defined(WIN32) && !defined(SOFA_BUILD_CORE)
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Vec3dTypes, defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Vec2dTypes, defaulttype::Vec2dTypes>;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Vec1dTypes, defaulttype::Vec1dTypes>;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Rigid3dTypes, defaulttype::Rigid3dTypes> ;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Rigid2dTypes, defaulttype::Rigid2dTypes> ;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Vec3dTypes, defaulttype::Rigid3dTypes> ;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Vec2dTypes, defaulttype::Rigid2dTypes> ;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Rigid3dTypes, defaulttype::Vec3dTypes> ;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Rigid2dTypes, defaulttype::Vec2dTypes> ;

extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Vec3fTypes, defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Vec2fTypes, defaulttype::Vec2fTypes>;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Vec1fTypes, defaulttype::Vec1fTypes>;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Rigid3fTypes, defaulttype::Rigid3fTypes> ;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Rigid2fTypes, defaulttype::Rigid2fTypes> ;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Vec3fTypes, defaulttype::Rigid3fTypes> ;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Vec2fTypes, defaulttype::Rigid2fTypes> ;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Rigid3fTypes, defaulttype::Vec3fTypes> ;
extern template class SOFA_CORE_API MixedInteractionConstraint<defaulttype::Rigid2fTypes, defaulttype::Vec2fTypes> ;
#endif

} // namespace behavior

} // namespace core

} // namespace sofa

#endif
