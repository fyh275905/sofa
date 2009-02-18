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
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_LMCONSTRAINT_H
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_LMCONSTRAINT_H

#include <sofa/core/core.h>
#include <sofa/core/componentmodel/behavior/BaseLMConstraint.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/objectmodel/BaseObject.h>
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
 *  \brief Component computing constraints within a simulated body.
 *
 *  This class define the abstract API common to constraints using a given type
 *  of DOFs.
 *  A LMConstraint computes complex constraints applied to two bodies
 *
 */
template<class DataTypes1,class DataTypes2>
class LMConstraint : public BaseLMConstraint
{
public:
    typedef typename DataTypes1::Real Real1;
    typedef typename DataTypes1::VecCoord VecCoord1;
    typedef typename DataTypes1::VecDeriv VecDeriv1;
    typedef typename DataTypes1::Coord Coord1;
    typedef typename DataTypes1::Deriv Deriv1;
    typedef typename DataTypes1::VecConst VecConst1;

    typedef typename DataTypes2::Real Real2;
    typedef typename DataTypes2::VecCoord VecCoord2;
    typedef typename DataTypes2::VecDeriv VecDeriv2;
    typedef typename DataTypes2::Coord Coord2;
    typedef typename DataTypes2::Deriv Deriv2;
    typedef typename DataTypes2::VecConst VecConst2;


    LMConstraint( MechanicalState<DataTypes1> *dof1, MechanicalState<DataTypes2> *dof2):object1(dof1),object2(dof2)
    {}
    LMConstraint() {}

    virtual ~LMConstraint();

    virtual void init();

    /// Retrieve the associated MechanicalState
    virtual core::componentmodel::behavior::BaseMechanicalState* getMechModel1() {return object1;}
    virtual core::componentmodel::behavior::BaseMechanicalState* getMechModel2() {return object2;}



    /// Pre-construction check method called by ObjectFactory.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        if (arg->getAttribute("object1") || arg->getAttribute("object2"))
        {
            if (dynamic_cast<MechanicalState<DataTypes1>*>(arg->findObject(arg->getAttribute("object1",".."))) == NULL)
                return false;
            if (dynamic_cast<MechanicalState<DataTypes2>*>(arg->findObject(arg->getAttribute("object2",".."))) == NULL)
                return false;
        }
        else
        {
            if (dynamic_cast<MechanicalState<DataTypes1>*>(context->getMechanicalState()) == NULL)
                return false;
        }
        return sofa::core::objectmodel::BaseObject::canCreate(obj, context, arg);
    }

    /// Construction method called by ObjectFactory.
    template<class T>
    static void create(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        sofa::core::objectmodel::BaseObject::create(obj, context, arg);
        if (arg && (arg->getAttribute("object1") || arg->getAttribute("object2")))
        {
            obj->object1 = dynamic_cast<MechanicalState<DataTypes1>*>(arg->findObject(arg->getAttribute("object1","..")));
            obj->object2 = dynamic_cast<MechanicalState<DataTypes2>*>(arg->findObject(arg->getAttribute("object2","..")));
        }
        else if (context)
        {
            obj->object1 =
                obj->object2 =
                        dynamic_cast<MechanicalState<DataTypes1>*>(context->getMechanicalState());
        }
    }


    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const LMConstraint<DataTypes1,DataTypes2>* = NULL)
    {
        return std::string("[") + DataTypes1::Name() + std::string(",") + DataTypes2::Name() + std::string("]");
    }


protected:
    MechanicalState<DataTypes1> *object1;
    MechanicalState<DataTypes2> *object2;

};

#if defined(WIN32) && !defined(SOFA_BUILD_CORE)
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec3dTypes,defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec3dTypes,defaulttype::Vec2dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec3dTypes,defaulttype::Vec1dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec3dTypes,defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec3dTypes,defaulttype::Rigid2dTypes>;

extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec2dTypes,defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec2dTypes,defaulttype::Vec2dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec2dTypes,defaulttype::Vec1dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec2dTypes,defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec2dTypes,defaulttype::Rigid2dTypes>;

extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec1dTypes,defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec1dTypes,defaulttype::Vec2dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec1dTypes,defaulttype::Vec1dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec1dTypes,defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec1dTypes,defaulttype::Rigid2dTypes>;

extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid3dTypes,defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid3dTypes,defaulttype::Vec2dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid3dTypes,defaulttype::Vec1dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid3dTypes,defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid3dTypes,defaulttype::Rigid2dTypes>;

extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid2dTypes,defaulttype::Vec3dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid2dTypes,defaulttype::Vec2dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid2dTypes,defaulttype::Vec1dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid2dTypes,defaulttype::Rigid3dTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid2dTypes,defaulttype::Rigid2dTypes>;

extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec3fTypes,defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec3fTypes,defaulttype::Vec2fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec3fTypes,defaulttype::Vec1fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec3fTypes,defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec3fTypes,defaulttype::Rigid2fTypes>;

extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec2fTypes,defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec2fTypes,defaulttype::Vec2fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec2fTypes,defaulttype::Vec1fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec2fTypes,defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec2fTypes,defaulttype::Rigid2fTypes>;

extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec1fTypes,defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec1fTypes,defaulttype::Vec2fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec1fTypes,defaulttype::Vec1fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec1fTypes,defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Vec1fTypes,defaulttype::Rigid2fTypes>;

extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid3fTypes,defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid3fTypes,defaulttype::Vec2fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid3fTypes,defaulttype::Vec1fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid3fTypes,defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid3fTypes,defaulttype::Rigid2fTypes>;

extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid2fTypes,defaulttype::Vec3fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid2fTypes,defaulttype::Vec2fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid2fTypes,defaulttype::Vec1fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid2fTypes,defaulttype::Rigid3fTypes>;
extern template class SOFA_CORE_API LMConstraint<defaulttype::Rigid2fTypes,defaulttype::Rigid2fTypes>;
#endif

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
