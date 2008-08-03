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
#include <sofa/core/objectmodel/BaseContext.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalState.h>
#include <sofa/core/componentmodel/behavior/BaseMass.h>
#include <sofa/core/componentmodel/topology/Topology.h>
#include <sofa/core/componentmodel/topology/BaseTopology.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/core/Shader.h>
#include <iostream>
using std::cerr;
using std::endl;

namespace sofa
{

namespace core
{

namespace objectmodel
{

BaseContext::BaseContext()
{}

BaseContext::~BaseContext()
{}

BaseContext* BaseContext::getDefault()
{
    static BaseContext defaultContext;
    return &defaultContext;
}

////////////////
// Parameters //
////////////////

/// The Context is active
bool BaseContext::isActive() const { return true;};

/// Gravity in the local coordinate system
BaseContext::Vec3 BaseContext::getLocalGravity() const
{
    static const Vec3 G((SReal)0,(SReal)-9.81,(SReal)0);
    return G;
}

/// Gravity in the world coordinate system
const BaseContext::Vec3& BaseContext::getGravityInWorld() const
{
    static const Vec3 G((SReal)0,(SReal)-9.81,(SReal)0);
    return G;
}

/// Simulation timestep
double BaseContext::getDt() const
{
    return 0.01;
}

/// Simulation time
double BaseContext::getTime() const
{
    return 0.0;
}

/// Animation flag
bool BaseContext::getAnimate() const
{
    return true;
}

/// MultiThreading activated
bool BaseContext::getMultiThreadSimulation() const
{
    return false;
}

/// Display flags: Collision Models
bool BaseContext::getShowCollisionModels() const
{
    return false;
}

/// Display flags: Bounding Collision Models
bool BaseContext::getShowBoundingCollisionModels() const
{
    return false;
}

/// Display flags: Behavior Models
bool BaseContext::getShowBehaviorModels() const
{
    return true;
}

/// Display flags: Visual Models
bool BaseContext::getShowVisualModels() const
{
    return true;
}

/// Display flags: Mappings
bool BaseContext::getShowMappings() const
{
    return false;
}

/// Display flags: Mechanical Mappings
bool BaseContext::getShowMechanicalMappings() const
{
    return false;
}

/// Display flags: ForceFields
bool BaseContext::getShowForceFields() const
{
    return false;
}

/// Display flags: InteractionForceFields
bool BaseContext::getShowInteractionForceFields() const
{
    return false;
}

/// Display flags: WireFrame
bool BaseContext::getShowWireFrame() const
{
    return false;
}

/// Display flags: Normals
bool BaseContext::getShowNormals() const
{
    return false;
}

/// Multiresolution
int BaseContext::getCurrentLevel() const
{
    return 0;
}
int BaseContext::getCoarsestLevel() const
{
    return 0;
}
int BaseContext::getFinestLevel() const
{
    return 0;
}
// unsigned int BaseContext::nbLevels() const
// {
// 	return getCoarsestLevel() - getFinestLevel() + 1;
// }

//////////////////////////////
// Local Coordinates System //
//////////////////////////////


/// Projection from the local coordinate system to the world coordinate system.
const BaseContext::Frame& BaseContext::getPositionInWorld() const
{
    static const Frame f;
    return f;
}

/// Spatial velocity (linear, angular) of the local frame with respect to the world
const BaseContext::SpatialVector& BaseContext::getVelocityInWorld() const
{
    static const SpatialVector v( Vec3(0,0,0), Vec3(0,0,0) );
    return v;
}

/// Linear acceleration of the origin induced by the angular velocity of the ancestors
const BaseContext::Vec3& BaseContext::getVelocityBasedLinearAccelerationInWorld() const
{
    static const Vec3 a(0,0,0);
    return a;
}


////////////////
// Containers //
////////////////

/// Generic object access, possibly searching up or down from the current context
///
/// Note that the template wrapper method should generally be used to have the correct return type,
void* BaseContext::getObject(const ClassInfo& /*class_info*/, SearchDirection /*dir*/) const
{
    return NULL;
}

/// Generic object access, given a path from the current context
///
/// Note that the template wrapper method should generally be used to have the correct return type,
void* BaseContext::getObject(const ClassInfo& /*class_info*/, const std::string& /*path*/) const
{
    return NULL;
}

/// Generic list of objects access, possibly searching up or down from the current context
///
/// Note that the template wrapper method should generally be used to have the correct return type,
void BaseContext::getObjects(const ClassInfo& /*class_info*/, GetObjectsCallBack& /*container*/, SearchDirection /*dir*/) const
{
}

/// Mechanical Degrees-of-Freedom
BaseObject* BaseContext::getMechanicalState() const
{
    return this->get<sofa::core::componentmodel::behavior::BaseMechanicalState>();
}

/// Mass
BaseObject* BaseContext::getMass() const
{
    return this->get<sofa::core::componentmodel::behavior::BaseMass>();
}


/// Topology
core::componentmodel::topology::Topology* BaseContext::getTopology() const
{
    return this->get<sofa::core::componentmodel::topology::Topology>();
}
/// Mesh Topology (unified interface for both static and dynamic topologies)
core::componentmodel::topology::BaseMeshTopology* BaseContext::getMeshTopology() const
{
    return this->get<sofa::core::componentmodel::topology::BaseMeshTopology>();
}

/// Shader
BaseObject* BaseContext::getShader() const
{
    return this->get<sofa::core::Shader>();
    return NULL;
}

/// Propagate an event
void BaseContext::propagateEvent( Event* )
{
    cerr<<"WARNING !!! BaseContext::propagateEvent not overloaded, does nothing"<<endl;
}

void BaseContext::executeVisitor( simulation::Visitor* )
{
    cerr<<"WARNING !!! BaseContext::executeVisitor not overloaded, does nothing"<<endl;
    //assert(false);
}


} // namespace objectmodel

} // namespace core

} // namespace sofa

