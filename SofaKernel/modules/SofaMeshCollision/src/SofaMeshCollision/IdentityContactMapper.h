/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <SofaMeshCollision/config.h>

#include <sofa/helper/Factory.h>
#include <SofaBaseMechanics/IdentityMapping.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <SofaBaseCollision/BaseContactMapper.h>
#include <SofaBaseCollision/SphereModel.h>
#include <SofaMeshCollision/PointModel.h>
#include <SofaBaseMechanics/IdentityMapping.h>

namespace sofa::component::collision
{

/// Base class for IdentityMapping based mappers
template<class TCollisionModel, class DataTypes>
class IdentityContactMapper : public BaseContactMapper<DataTypes>
{
public:
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    typedef TCollisionModel MCollisionModel;
    typedef typename MCollisionModel::InDataTypes InDataTypes;
    typedef core::behavior::MechanicalState<InDataTypes> InMechanicalState;
    typedef core::behavior::MechanicalState<typename IdentityContactMapper::DataTypes> MMechanicalState;
    typedef component::container::MechanicalObject<typename IdentityContactMapper::DataTypes> MMechanicalObject;
    typedef mapping::IdentityMapping< InDataTypes, typename IdentityContactMapper::DataTypes > MMapping;
    MCollisionModel* model;
    typename MMapping::SPtr mapping;
    using Index = sofa::Index;

    IdentityContactMapper()
        : model(nullptr), mapping(nullptr)
    {
    }

    void setCollisionModel(MCollisionModel* model)
    {
        this->model = model;
    }

    void cleanup();

    MMechanicalState* createMapping(const char* name="contactPoints");

    void resize(Size /*size*/)
    {
    }

    Index addPoint(const Coord&, Index index, Real&)
    {
        return index;
    }

    void update()
    {
        if (mapping!=nullptr)
        {
            mapping->apply(core::MechanicalParams::defaultInstance(), core::VecCoordId::position(), core::ConstVecCoordId::position());
            mapping->applyJ(core::MechanicalParams::defaultInstance(), core::VecDerivId::velocity(), core::ConstVecDerivId::velocity());
        }
    }

    void updateXfree()
    {
        if (mapping!=nullptr)
        {
            mapping->apply(core::MechanicalParams::defaultInstance(), core::VecCoordId::freePosition(), core::ConstVecCoordId::freePosition());
            mapping->applyJ(core::MechanicalParams::defaultInstance(), core::VecDerivId::freeVelocity(), core::ConstVecDerivId::freeVelocity());

        }
    }
    
    void updateX0()
    {
        if(mapping!=nullptr)
        {
             mapping->apply(core::MechanicalParams::defaultInstance(), core::VecCoordId::restPosition(), core::ConstVecCoordId::restPosition());
        }
    }
};

/// Specialization of IdentityContactMapper when mapping to the same DataTypes, as no mapping is required in this case
template<class TCollisionModel>
class IdentityContactMapper<TCollisionModel, typename TCollisionModel::InDataTypes> : public BaseContactMapper<typename TCollisionModel::InDataTypes>
{
public:
    typedef TCollisionModel MCollisionModel;
    typedef typename MCollisionModel::InDataTypes InDataTypes;
    typedef typename MCollisionModel::InDataTypes DataTypes;
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    typedef core::behavior::MechanicalState<InDataTypes> InMechanicalState;
    typedef core::behavior::MechanicalState<DataTypes> MMechanicalState;
    MCollisionModel* model;
    using Index = sofa::Index;

    IdentityContactMapper()
        : model(nullptr)
    {
    }

    void setCollisionModel(MCollisionModel* model)
    {
        this->model = model;
    }

    void cleanup()
    {
    }

    MMechanicalState* createMapping(const char* /*name*/="contactPoints")
    {
        if (model==nullptr) return nullptr;
        return model->getMechanicalState();
    }

    void resize(Size /*size*/)
    {
    }

    Index addPoint(const Coord& /*P*/, Index index, Real&)
    {
        return index;
    }

    void update()
    {
    }

    void updateXfree()
    {
    }

    void updateX0()
    {
    }

};

/// Mapper for PointModel
template<class DataTypes>
class ContactMapper<PointCollisionModel<sofa::defaulttype::Vec3Types>, DataTypes> : public IdentityContactMapper<PointCollisionModel<sofa::defaulttype::Vec3Types>, DataTypes>
{
public:
};

/// Mapper for SphereModel
template<class DataTypes>
class ContactMapper<SphereCollisionModel<sofa::defaulttype::Vec3Types>, DataTypes> : public IdentityContactMapper<SphereCollisionModel<sofa::defaulttype::Vec3Types>, DataTypes>
{
public:
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    using Index = sofa::Index;

    Index addPoint(const Coord& /*P*/, Index index, Real& r)
    {
        Sphere e(this->model, index);
        r = e.r();
        return index;
    }
};

#if  !defined(SOFA_COMPONENT_COLLISION_IDENTITYCONTACTMAPPER_CPP)
extern template class SOFA_SOFAMESHCOLLISION_API ContactMapper<SphereCollisionModel<sofa::defaulttype::Vec3Types>, sofa::defaulttype::Vec3Types>;
extern template class SOFA_SOFAMESHCOLLISION_API ContactMapper<PointCollisionModel<sofa::defaulttype::Vec3Types>, sofa::defaulttype::Vec3Types>;
#endif

} //namespace sofa::component::collision
