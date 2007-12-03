/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_CORE_OBJECTMODEL_CONTEXT_H
#define SOFA_CORE_OBJECTMODEL_CONTEXT_H

#include <sofa/core/objectmodel/BaseContext.h>
#include <sofa/core/objectmodel/Data.h>

#include <iostream>
#include <map>

namespace sofa
{

namespace core
{

namespace objectmodel
{

/**
 *  \brief Implementation of BaseContext, storing all shared parameters in Datas.
 *
 */
class Context : public BaseContext
{
public:

    typedef BaseContext::Frame Frame;
    typedef BaseContext::Vec3 Vec3;
    typedef BaseContext::Quat Quat;
    typedef BaseContext::SpatialVector SpatialVector;

    Data<bool> is_activated;
    Data<Vec3> worldGravity_;  ///< Gravity IN THE WORLD COORDINATE SYSTEM.
    Data<double> dt_;
    Data<double> time_;
    Data<bool> animate_;
    Data<bool> showCollisionModels_;
    Data<bool> showBoundingCollisionModels_;
    Data<bool> showBehaviorModels_;
    Data<bool> showVisualModels_;
    Data<bool> showMappings_;
    Data<bool> showMechanicalMappings_;
    Data<bool> showForceFields_;
    Data<bool> showInteractionForceFields_;
    Data<bool> showWireFrame_;
    Data<bool> showNormals_;
    Data<bool> multiThreadSimulation_;

    /// @name For multiresolution (UNSTABLE)
    /// @{
    Data<int> currentLevel_;
    Data<int> coarsestLevel_;
    Data<int> finestLevel_;
    /// @}


    Frame localFrame_;
    SpatialVector spatialVelocityInWorld_;
    Vec3 velocityBasedLinearAccelerationInWorld_;

    Context();
    virtual ~Context()
    {}

    /// @name Parameters
    /// @{

    /// The Context is active
    virtual const bool isActive() const;
    /// State of the context
    virtual void setActive(bool val);
    /// Gravity in the local coordinate system
    virtual Vec3 getLocalGravity() const;
    /// Gravity in the local coordinate system
    //virtual void setGravity(const Vec3& );
    /// Gravity in world coordinates
    virtual const Vec3& getGravityInWorld() const;
    /// Gravity in world coordinates
    virtual void setGravityInWorld( const Vec3& );

    /// Simulation timestep
    virtual double getDt() const;

    /// Simulation time
    virtual double getTime() const;

    /// Animation flag
    virtual bool getAnimate() const;

    /// MultiThreading activated
    virtual bool getMultiThreadSimulation() const;

    /// Display flags: Collision Models
    virtual bool getShowCollisionModels() const;

    /// Display flags: Bounding Collision Models
    virtual bool getShowBoundingCollisionModels() const;

    /// Display flags: Behavior Models
    virtual bool getShowBehaviorModels() const;

    /// Display flags: Visual Models
    virtual bool getShowVisualModels() const;

    /// Display flags: Mappings
    virtual bool getShowMappings() const;

    /// Display flags: Mechanical Mappings
    virtual bool getShowMechanicalMappings() const;

    /// Display flags: ForceFields
    virtual bool getShowForceFields() const;

    /// Display flags: InteractionForceFields
    virtual bool getShowInteractionForceFields() const;

    /// Display flags: WireFrame
    virtual bool getShowWireFrame() const;

    /// Display flags: Normals
    virtual bool getShowNormals() const;

    /// Multiresolution support (UNSTABLE)
    virtual int getCurrentLevel() const;
    /// Multiresolution support (UNSTABLE)
    virtual int getCoarsestLevel() const;
    /// Multiresolution support (UNSTABLE)
    virtual int getFinestLevel() const;

    /// @}


    /// @name Local Coordinate System
    /// @{
    /// Projection from the local coordinate system to the world coordinate system.
    virtual const Frame& getPositionInWorld() const;
    /// Projection from the local coordinate system to the world coordinate system.
    virtual void setPositionInWorld(const Frame&);

    /// Spatial velocity (linear, angular) of the local frame with respect to the world
    virtual const SpatialVector& getVelocityInWorld() const;
    /// Spatial velocity (linear, angular) of the local frame with respect to the world
    virtual void setVelocityInWorld(const SpatialVector&);

    /// Linear acceleration of the origin induced by the angular velocity of the ancestors
    virtual const Vec3& getVelocityBasedLinearAccelerationInWorld() const;
    /// Linear acceleration of the origin induced by the angular velocity of the ancestors
    virtual void setVelocityBasedLinearAccelerationInWorld(const Vec3& );
    /// @}

    /// @name Parameters Setters
    /// @{

    /// Simulation timestep
    virtual void setDt( double dt );

    /// Simulation time
    virtual void setTime( double t );

    /// Animation flag
    virtual void setAnimate(bool val);

    /// MultiThreading activated
    virtual void setMultiThreadSimulation(bool val);

    /// Display flags: Collision Models
    virtual void setShowCollisionModels(bool val);

    /// Display flags: Behavior Models
    virtual void setShowBehaviorModels(bool val);

    /// Display flags: Bounding Collision Models
    virtual void setShowBoundingCollisionModels(bool val);

    /// Display flags: Visual Models
    virtual void setShowVisualModels(bool val);

    /// Display flags: Mappings
    virtual void setShowMappings(bool val);

    /// Display flags: Mechanical Mappings
    virtual void setShowMechanicalMappings(bool val);

    /// Display flags: ForceFields
    virtual void setShowForceFields(bool val);

    /// Display flags: InteractionForceFields
    virtual void setShowInteractionForceFields(bool val);

    /// Display flags: WireFrame
    virtual void setShowWireFrame(bool val);

    /// Display flags: Normals
    virtual void setShowNormals(bool val);

    /// Multiresolution support (UNSTABLE) : Set the current level, return false if l >= coarsestLevel
    virtual bool setCurrentLevel(int l);
    /// Multiresolution support (UNSTABLE)
    virtual void setCoarsestLevel(int l);
    /// Multiresolution support (UNSTABLE)
    virtual void setFinestLevel(int l);

    /// @}

    /// Copy the context variables from the given instance
    void copyContext(const Context& c);

    /// Copy the context variables of visualization from the given instance
    void copySimulationContext(const Context& c);

    /// Copy the context variables of visualization from the given instance
    void copyVisualContext(const Context& c);

    friend std::ostream& operator << (std::ostream& out, const Context& c );

};

} // namespace objectmodel

} // namespace core

} // namespace sofa

#endif
