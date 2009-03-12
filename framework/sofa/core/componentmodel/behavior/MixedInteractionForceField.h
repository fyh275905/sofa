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
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MIXEDINTERACTIONFORCEFIELD_H
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MIXEDINTERACTIONFORCEFIELD_H

#include <sofa/core/core.h>
#include <sofa/core/componentmodel/behavior/InteractionForceField.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

/**
 *  \brief Component computing forces between a pair of simulated body.
 *
 *  This class define the abstract API common to interaction force fields
 *  between a pair of bodies using a given type of DOFs.
 */
template<class TDataTypes1, class TDataTypes2>
class MixedInteractionForceField : public InteractionForceField
{
public:
    typedef TDataTypes1 DataTypes1;
    typedef typename DataTypes1::VecCoord VecCoord1;
    typedef typename DataTypes1::VecDeriv VecDeriv1;
    typedef typename DataTypes1::Coord Coord1;
    typedef typename DataTypes1::Deriv Deriv1;
    typedef TDataTypes2 DataTypes2;
    typedef typename DataTypes2::VecCoord VecCoord2;
    typedef typename DataTypes2::VecDeriv VecDeriv2;
    typedef typename DataTypes2::Coord Coord2;
    typedef typename DataTypes2::Deriv Deriv2;

    MixedInteractionForceField(MechanicalState<DataTypes1> *mm1 = NULL, MechanicalState<DataTypes2> *mm2 = NULL);

    virtual ~MixedInteractionForceField();

    virtual void init();

    /// Retrieve the associated MechanicalState
    MechanicalState<DataTypes1>* getMState1() { return mstate1; }
    BaseMechanicalState* getMechModel1() { return mstate1; }
    /// Retrieve the associated MechanicalState
    MechanicalState<DataTypes2>* getMState2() { return mstate2; }
    BaseMechanicalState* getMechModel2() { return mstate2; }

    /// @name Vector operations
    /// @{

    /// Given the current position and velocity states, update the current force
    /// vector by computing and adding the forces associated with this
    /// ForceField.
    ///
    /// If the ForceField can be represented as a matrix, this method computes
    /// $ f += B v + K x $
    ///
    /// This method retrieves the force, x and v vector from the two MechanicalState
    /// and call the internal addForce(VecDeriv&,VecDeriv&,const VecCoord&,const VecCoord&,const VecDeriv&,const VecDeriv&)
    /// method implemented by the component.
    virtual void addForce();

    /// Compute the force derivative given a small displacement from the
    /// position and velocity used in the previous call to addForce().
    ///
    /// The derivative should be directly derived from the computations
    /// done by addForce. Any forces neglected in addDForce will be integrated
    /// explicitly (i.e. using its value at the beginning of the timestep).
    ///
    /// If the ForceField can be represented as a matrix, this method computes
    /// $ df += kFactor K dx + bFactor B dx $
    ///
    /// This method retrieves the force and dx vector from the two MechanicalState
    /// and call the internal addDForce(VecDeriv1&,VecDeriv2&,const VecDeriv1&,const VecDeriv2&,double,double)
    /// method implemented by the component.
    virtual void addDForce(double kFactor, double bFactor);

    /// Same as addDForce(), except the velocity vector should be used instead of dx.
    ///
    /// If the ForceField can be represented as a matrix, this method computes
    /// $ df += kFactor K v + bFactor B v $
    ///
    /// This method retrieves the force and velocity vector from the two MechanicalState
    /// and call the internal addDForce(VecDeriv1&,VecDeriv2&,const VecDeriv1&,const VecDeriv2&,double,double)
    /// method implemented by the component.
    virtual void addDForceV(double kFactor, double bFactor);

    /// Get the potential energy associated to this ForceField.
    ///
    /// Used to extimate the total energy of the system by some
    /// post-stabilization techniques.
    ///
    /// This method retrieves the x vector from the MechanicalState and call
    /// the internal getPotentialEnergy(const VecCoord&,const VecCoord&) method implemented by
    /// the component.
    virtual double getPotentialEnergy();

    /// Given the current position and velocity states, update the current force
    /// vector by computing and adding the forces associated with this
    /// ForceField.
    ///
    /// If the ForceField can be represented as a matrix, this method computes
    /// $ f += B v + K x $
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic ForceField::addForce() method.
    virtual void addForce(VecDeriv1& f1, VecDeriv2& f2, const VecCoord1& x1, const VecCoord2& x2, const VecDeriv1& v1, const VecDeriv2& v2) = 0;

    /// Compute the force derivative given a small displacement from the
    /// position and velocity used in the previous call to addForce().
    ///
    /// The derivative should be directly derived from the computations
    /// done by addForce. Any forces neglected in addDForce will be integrated
    /// explicitly (i.e. using its value at the beginning of the timestep).
    ///
    /// If the ForceField can be represented as a matrix, this method computes
    /// $ df += K dx $
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic MixedInteractionForceField::addDForce() method.
    ///
    /// @deprecated to more efficiently accumulate contributions from all terms
    ///   of the system equation, a new addDForce method allowing to pass two
    ///   coefficients for the stiffness and damping terms should now be used.
    virtual void addDForce(VecDeriv1& df1, VecDeriv2& df2, const VecDeriv1& dx1, const VecDeriv2& dx2);

    /// Compute the force derivative given a small displacement from the
    /// position and velocity used in the previous call to addForce().
    ///
    /// The derivative should be directly derived from the computations
    /// done by addForce. Any forces neglected in addDForce will be integrated
    /// explicitly (i.e. using its value at the beginning of the timestep).
    ///
    /// If the ForceField can be represented as a matrix, this method computes
    /// $ df += kFactor K dx + bFactor B dx $
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic MixedInteractionForceField::addDForce() method.
    ///
    /// To support old components that implement the deprecated addForce method
    /// without scalar coefficients, it defaults to using a temporaty vector to
    /// compute $ K dx $ and then manually scaling all values by kFactor.
    virtual void addDForce(VecDeriv1& df1, VecDeriv2& df2, const VecDeriv1& dx1, const VecDeriv2& dx2, double kFactor, double bFactor);

    /// Get the potential energy associated to this ForceField.
    ///
    /// Used to extimate the total energy of the system by some
    /// post-stabilization techniques.
    ///
    /// This method must be implemented by the component, and is usually called
    /// by the generic ForceField::getPotentialEnergy() method.
    virtual double getPotentialEnergy(const VecCoord1& x1, const VecCoord2& x2) =0;

    /// @}

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
        return InteractionForceField::canCreate(obj, context, arg);
    }

    /// Construction method called by ObjectFactory.
    template<class T>
    static void create(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        core::componentmodel::behavior::InteractionForceField::create(obj, context, arg);
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

    static std::string templateName(const MixedInteractionForceField<DataTypes1,DataTypes2>* = NULL)
    {
        return DataTypes1::Name()+std::string(",")+DataTypes2::Name();
    }

protected:
    MechanicalState<DataTypes1> *mstate1;
    MechanicalState<DataTypes2> *mstate2;
};

#if defined(WIN32) && !defined(SOFA_BUILD_CORE)
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1dTypes, defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1dTypes, defaulttype::Rigid3dTypes>;

extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec3dTypes, defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec2dTypes, defaulttype::Vec2dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1dTypes, defaulttype::Vec1dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid3dTypes, defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid2dTypes, defaulttype::Rigid2dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec3dTypes, defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec2dTypes, defaulttype::Rigid2dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid3dTypes, defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid2dTypes, defaulttype::Vec2dTypes>;

extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1fTypes, defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1fTypes, defaulttype::Rigid3fTypes>;

extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec3fTypes, defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec2fTypes, defaulttype::Vec2fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1fTypes, defaulttype::Vec1fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid3fTypes, defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid2fTypes, defaulttype::Rigid2fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec3fTypes, defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec2fTypes, defaulttype::Rigid2fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid3fTypes, defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid2fTypes, defaulttype::Vec2fTypes>;

extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1dTypes, defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1fTypes, defaulttype::Vec3dTypes>;

extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1dTypes, defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1fTypes, defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec3dTypes, defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec2dTypes, defaulttype::Vec2fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1dTypes, defaulttype::Vec1fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid3dTypes, defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid2dTypes, defaulttype::Rigid2fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec3dTypes, defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec2dTypes, defaulttype::Rigid2fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid3dTypes, defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid2dTypes, defaulttype::Vec2fTypes>;

extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec3fTypes, defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec2fTypes, defaulttype::Vec2dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec1fTypes, defaulttype::Vec1dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid3fTypes, defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid2fTypes, defaulttype::Rigid2dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec3fTypes, defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Vec2fTypes, defaulttype::Rigid2dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid3fTypes, defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API MixedInteractionForceField<defaulttype::Rigid2fTypes, defaulttype::Vec2dTypes>;
#endif

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
