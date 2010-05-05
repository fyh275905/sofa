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
#ifndef SOFA_COMPONENT_ENGINE_TRANSFORMPOSITION_H
#define SOFA_COMPONENT_ENGINE_TRANSFORMPOSITION_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/defaulttype/Vec.h>
#include <sofa/core/DataEngine.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/component/topology/PointSubset.h>
#include <sofa/defaulttype/Vec3Types.h>

namespace sofa
{

namespace component
{

namespace engine
{

using namespace core::behavior;
using namespace core::topology;
using namespace core::objectmodel;

/**
 * This class transforms the positions of one DataFields into new positions after applying a transformation
This transformation can be either : projection on a plane (plane defined by an origin and a normal vector),
translation, rotation, scale and some combinations of translation, rotation and scale
 */
template <class DataTypes>
class TransformPosition : public core::DataEngine
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(TransformPosition,DataTypes),core::DataEngine);
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::Real Real;

    typedef enum
    {
        PROJECT_ON_PLANE,
        TRANSLATION,
        ROTATION,
        SCALE,
        SCALE_TRANSLATION,
        SCALE_ROTATION_TRANSLATION
    } TransformationMethod;
public:

    TransformPosition();

    ~TransformPosition() {}

    void init();

    void reinit();

    void update();

    void draw();

    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        //      if (dynamic_cast<MechanicalState<DataTypes>*>(context->getMechanicalState()) == NULL)
        //        return false;
        return BaseObject::canCreate(obj, context, arg);
    }

    /// Construction method called by ObjectFactory.
    template<class T>
    static void create(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        core::objectmodel::BaseObject::create(obj, context, arg);
        //   if (context)
        // {
        //    obj->mstate = dynamic_cast<MechanicalState<DataTypes>*>(context->getMechanicalState());
        // }
    }

    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const TransformPosition<DataTypes>* = NULL)
    {
        return DataTypes::Name();
    }

protected:

    TransformationMethod transformationMethod;
    Data<Coord> f_origin; // origin used by projectOnPlane
    Data<VecCoord> f_inputX; // input position
    Data<VecCoord> f_outputX; // ouput position
    Data<Coord> f_normal; // normal used by projectOnPlane
    Data<Coord> f_translation; // translation
    Data<Coord> f_rotation; // rotation
    Data<Real> f_scale; // scale
    Data<std::string> method; // the method of the transformation

    MechanicalState<DataTypes>* mstate;
    const VecCoord* x0;

};

#if defined(WIN32) && !defined(SOFA_COMPONENT_ENGINE_TRANSFORMPOSITION_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
template class SOFA_COMPONENT_ENGINE_API TransformPosition<defaulttype::Vec3dTypes>;
#endif //SOFA_FLOAT
#ifndef SOFA_DOUBLE
template class SOFA_COMPONENT_ENGINE_API TransformPosition<defaulttype::Vec3fTypes>;
#endif //SOFA_DOUBLE
#endif

} // namespace engine

} // namespace component

} // namespace sofa

#endif
