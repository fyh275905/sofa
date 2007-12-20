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
#include <sofa/core/objectmodel/Context.h>
// #include <sofa/simulation/tree/Visitor.h>
// #include <sofa/simulation/tree/Visitor.h>

namespace sofa
{

namespace core
{

namespace objectmodel
{

Context::Context()
    : is_activated(initData(&is_activated, true, "activated", "To Activate a node"))
    , worldGravity_(initData(&worldGravity_, Vec3(0,0,0),"gravity","Gravity in the world coordinate system"))
    , dt_(initData(&dt_,0.01,"dt","Time step"))
    , time_(initData(&time_,0.,"time","Current time"))
    , animate_(initData(&animate_,false,"animate","Animate the Simulation(applied at initialization only)"))
    , showVisualModels_           (initData(&showVisualModels_,           -1, "showVisualModels","display Visual Models"))
    , showBehaviorModels_         (initData(&showBehaviorModels_,         -1,"showBehaviorModels","display Behavior Models"))
    , showCollisionModels_        (initData(&showCollisionModels_,        -1,"showCollisionModels","display Collision Models"))
    , showBoundingCollisionModels_(initData(&showBoundingCollisionModels_,-1,"showBoundingCollisionModels","display Bounding Collision Models"))
    , showMappings_               (initData(&showMappings_,               -1,"showMappings","display Mappings"))
    , showMechanicalMappings_     (initData(&showMechanicalMappings_,     -1,"showMechanicalMappings","display Mechanical Mappings"))
    , showForceFields_            (initData(&showForceFields_,            -1,"showForceFields","display Force Fields"))
    , showInteractionForceFields_ (initData(&showInteractionForceFields_, -1,"showInteractionForceFields","display Interaction Force Fields"))
    , showWireFrame_              (initData(&showWireFrame_,              -1,"showWireFrame","display in WireFrame"))
    , showNormals_                (initData(&showNormals_,                -1,"showNormals","display Normals"))
    , multiThreadSimulation_(initData(&multiThreadSimulation_,false,"multiThreadSimulation","Apply multithreaded simulation"))
    , currentLevel_(initData(&currentLevel_,0,"currentLevel","Current level of details"))
    , coarsestLevel_(initData(&coarsestLevel_,3,"coarsestLevel","Coarsest level of details"))
    , finestLevel_(initData(&finestLevel_,0,"finestLevel","Finest level of details"))
{
    setPositionInWorld(objectmodel::BaseContext::getPositionInWorld());
    setGravityInWorld(objectmodel::BaseContext::getLocalGravity());
    setVelocityInWorld(objectmodel::BaseContext::getVelocityInWorld());
    setVelocityBasedLinearAccelerationInWorld(objectmodel::BaseContext::getVelocityBasedLinearAccelerationInWorld());
    //setDt(objectmodel::BaseContext::getDt());
    //setTime(objectmodel::BaseContext::getTime());
    //setAnimate(objectmodel::BaseContext::getAnimate());
    //setShowCollisionModels(objectmodel::BaseContext::getShowCollisionModels());
    //setShowBoundingCollisionModels(objectmodel::BaseContext::getShowBoundingCollisionModels());
    //setShowBehaviorModels(objectmodel::BaseContext::getShowBehaviorModels());
    //setShowVisualModels(objectmodel::BaseContext::getShowVisualModels());
    //setShowMappings(objectmodel::BaseContext::getShowMappings());
    //setShowMechanicalMappings(objectmodel::BaseContext::getShowMechanicalMappings());
    //setShowForceFields(objectmodel::BaseContext::getShowForceFields());
    //setShowInteractionForceFields(objectmodel::BaseContext::getShowInteractionForceFields());
    //setShowWireFrame(objectmodel::BaseContext::getShowWireFrame());
    //setShowNormals(objectmodel::BaseContext::getShowNormals());
    //setMultiThreadSimulation(objectmodel::BaseContext::getMultiThreadSimulation());
}

/// The Context is active
const bool Context::isActive() const {return is_activated.getValue();}

/// State of the context
void Context::setActive(bool val) { is_activated.setValue(val);}

/// Projection from the local coordinate system to the world coordinate system.
const Context::Frame& Context::getPositionInWorld() const
{
    return localFrame_;
}
/// Projection from the local coordinate system to the world coordinate system.
void Context::setPositionInWorld(const Frame& f)
{
    localFrame_ = f;
}

/// Spatial velocity (linear, angular) of the local frame with respect to the world
const Context::SpatialVector& Context::getVelocityInWorld() const
{
    return spatialVelocityInWorld_;
}
/// Spatial velocity (linear, angular) of the local frame with respect to the world
void Context::setVelocityInWorld(const SpatialVector& v)
{
    spatialVelocityInWorld_ = v;
}

/// Linear acceleration of the origin induced by the angular velocity of the ancestors
const Context::Vec3& Context::getVelocityBasedLinearAccelerationInWorld() const
{
    return velocityBasedLinearAccelerationInWorld_;
}
/// Linear acceleration of the origin induced by the angular velocity of the ancestors
void Context::setVelocityBasedLinearAccelerationInWorld(const Vec3& a )
{
    velocityBasedLinearAccelerationInWorld_ = a;
}



/// Simulation timestep
double Context::getDt() const
{
    return dt_.getValue();
}

/// Simulation time
double Context::getTime() const
{
    return time_.getValue();
}

/// Gravity vector in local coordinates
// const Context::Vec3& Context::getGravity() const
// {
// 	return gravity_;
// }

/// Gravity vector in local coordinates
Context::Vec3 Context::getLocalGravity() const
{
    return getPositionInWorld().backProjectVector(worldGravity_.getValue());
}

/// Gravity vector in world coordinates
const Context::Vec3& Context::getGravityInWorld() const
{
    return worldGravity_.getValue();
}



/// Animation flag
bool Context::getAnimate() const
{
    return animate_.getValue();
}

/// MultiThreading activated
bool Context::getMultiThreadSimulation() const
{
    return multiThreadSimulation_.getValue();
}

/// Display flags: Collision Models
bool Context::getShowCollisionModels() const
{
    if (showCollisionModels_.getValue() < 0) return false;
    else return showCollisionModels_.getValue();
}

/// Display flags: Bounding Collision Models
bool Context::getShowBoundingCollisionModels() const
{
    if (showBoundingCollisionModels_.getValue() < 0) return false;
    else return showBoundingCollisionModels_.getValue();
}

/// Display flags: Behavior Models
bool Context::getShowBehaviorModels() const
{
    if (showBehaviorModels_.getValue() < 0) return false;
    else  return showBehaviorModels_.getValue();
}

/// Display flags: Visual Models
bool Context::getShowVisualModels() const
{
    if (showVisualModels_.getValue() < 0) return true;
    else return showVisualModels_.getValue();
}

/// Display flags: Mappings
bool Context::getShowMappings() const
{
    if (showMappings_.getValue() < 0) return false;
    else return showMappings_.getValue();
}

/// Display flags: Mechanical Mappings
bool Context::getShowMechanicalMappings() const
{
    if (showMechanicalMappings_.getValue() < 0) return false;
    else return showMechanicalMappings_.getValue();
}

/// Display flags: ForceFields
bool Context::getShowForceFields() const
{
    if (showForceFields_.getValue() < 0) return false;
    else return showForceFields_.getValue();
}

/// Display flags: InteractionForceFields
bool Context::getShowInteractionForceFields() const
{
    if (showInteractionForceFields_.getValue() < 0) return false;
    else return showInteractionForceFields_.getValue();
}

/// Display flags: WireFrame
bool Context::getShowWireFrame() const
{
    if (showWireFrame_.getValue() < 0) return false;
    else return showWireFrame_.getValue();
}

/// Display flags: Normal
bool Context::getShowNormals() const
{
    if (showNormals_.getValue() < 0) return false;
    else return showNormals_.getValue();
}


// Multiresolution

int Context::getCurrentLevel() const
{
    return currentLevel_.getValue();
}
int Context::getCoarsestLevel() const
{
    return coarsestLevel_.getValue();
}
int Context::getFinestLevel() const
{
    return finestLevel_.getValue();
}


//===============================================================================

/// Simulation timestep
void Context::setDt(double val)
{
    dt_.setValue(val);
}

/// Simulation time
void Context::setTime(double val)
{
    time_.setValue(val);
}

/// Gravity vector
// void Context::setGravity(const Vec3& g)
// {
// 	gravity_ = g;
// }

/// Gravity vector
void Context::setGravityInWorld(const Vec3& g)
{
    worldGravity_ .setValue(g);
}

/// Animation flag
void Context::setAnimate(bool val)
{
    animate_.setValue(val);
}

/// MultiThreading activated
void Context::setMultiThreadSimulation(bool val)
{
    multiThreadSimulation_.setValue(val);
}

/// Display flags: Collision Models
void Context::setShowCollisionModels(bool val)
{
    showCollisionModels_.setValue(val);
}

/// Display flags: Bounding Collision Models
void Context::setShowBoundingCollisionModels(bool val)
{
    showBoundingCollisionModels_.setValue(val);
}

/// Display flags: Behavior Models
void Context::setShowBehaviorModels(bool val)
{
    showBehaviorModels_.setValue(val);
}

/// Display flags: Visual Models
void Context::setShowVisualModels(bool val)
{
    showVisualModels_.setValue(val);
}

/// Display flags: Mappings
void Context::setShowMappings(bool val)
{
    showMappings_.setValue(val);
}

/// Display flags: Mechanical Mappings
void Context::setShowMechanicalMappings(bool val)
{
    showMechanicalMappings_.setValue(val);
}

/// Display flags: ForceFields
void Context::setShowForceFields(bool val)
{
    showForceFields_.setValue(val);
}

/// Display flags: InteractionForceFields
void Context::setShowInteractionForceFields(bool val)
{
    showInteractionForceFields_.setValue(val);
}

/// Display flags: WireFrame
void Context::setShowWireFrame(bool val)
{
    showWireFrame_.setValue(val);
}

/// Display flags: Normals
void Context::setShowNormals(bool val)
{
    showNormals_.setValue(val);
}


// Multiresolution

bool Context::setCurrentLevel(int l)
{
    if( l > coarsestLevel_.getValue() )
    {
        currentLevel_.setValue(coarsestLevel_.getValue());
        return false;
    }
    else if( l < 0 /*finestLevel_.getValue()*/ )
    {
// 		currentLevel_.setValue(finestLevel_.getValue());
        currentLevel_.setValue( 0 );
        return false;
    }
    currentLevel_.setValue(l);
    if( l == coarsestLevel_.getValue() ) return false;
    return true;
}
void Context::setCoarsestLevel(int l)
{
    coarsestLevel_.setValue( l );
}
void Context::setFinestLevel(int l)
{
    finestLevel_.setValue( l );
}

//======================


void Context::copyContext(const Context& c)
{
    // BUGFIX 12/01/06 (Jeremie A.): Can't use operator= on the class as it will copy other data in the BaseContext class (such as name)...
    // *this = c;

    copySimulationContext(c);
    copyVisualContext(c);
}


void Context::copySimulationContext(const Context& c)
{
    worldGravity_.setValue(c.worldGravity_.getValue());  ///< Gravity IN THE WORLD COORDINATE SYSTEM.
    dt_.setValue(c.dt_.getValue());
    time_.setValue(c.time_.getValue());
    animate_.setValue(c.animate_.getValue());
    multiThreadSimulation_.setValue(c.multiThreadSimulation_.getValue());

    localFrame_ = c.localFrame_;
    spatialVelocityInWorld_ = c.spatialVelocityInWorld_;
    velocityBasedLinearAccelerationInWorld_ = c.velocityBasedLinearAccelerationInWorld_;

    // for multiresolution
// 	finestLevel_ = c.finestLevel_;
// 	coarsestLevel_ = c.coarsestLevel_;
// 	currentLevel_ = c.currentLevel_;

}

void Context::copyVisualContext(const Context& c)
{
    showCollisionModels_.setValue(c.showCollisionModels_.getValue());
    showBoundingCollisionModels_.setValue(c.showBoundingCollisionModels_.getValue());
    showBehaviorModels_.setValue(c.showBehaviorModels_.getValue());
    showVisualModels_.setValue(c.showVisualModels_.getValue());
    showMappings_.setValue(c.showMappings_.getValue());
    showMechanicalMappings_.setValue(c.showMechanicalMappings_.getValue());
    showForceFields_.setValue(c.showForceFields_.getValue());
    showInteractionForceFields_.setValue(c.showInteractionForceFields_.getValue());
    showWireFrame_.setValue(c.showWireFrame_.getValue());
    showNormals_.setValue(c.showNormals_.getValue());
}

std::ostream& operator << (std::ostream& out, const Context& c )
{
    out<<std::endl<<"local gravity = "<<c.getLocalGravity();
    out<<std::endl<<"transform from local to world = "<<c.getPositionInWorld();
    //out<<std::endl<<"transform from world to local = "<<c.getWorldToLocal();
    out<<std::endl<<"spatial velocity = "<<c.getVelocityInWorld();
    out<<std::endl<<"acceleration of the origin = "<<c.getVelocityBasedLinearAccelerationInWorld();
    return out;
}




} // namespace objectmodel

} // namespace core

} // namespace sofa

