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
#ifndef SOFA_COMPONENT_COLLISION_ATTACHBODYPERFORMER_H
#define SOFA_COMPONENT_COLLISION_ATTACHBODYPERFORMER_H

#include "InteractionPerformer.h"
#include "BarycentricContactMapper.h"
#include <sofa/core/componentmodel/behavior/BaseForceField.h>

namespace sofa
{
namespace core
{
namespace objectmodel
{
class TagSet;
}
}
namespace component
{

namespace collision
{
struct BodyPicked;

class AttachBodyPerformerConfiguration
{
public:
    void setStiffness(double s) {stiffness=s;}
protected:
    SReal stiffness;
};

template <class DataTypes>
class AttachBodyPerformer: public TInteractionPerformer<DataTypes>, public AttachBodyPerformerConfiguration
{

    typedef sofa::component::collision::BaseContactMapper< DataTypes >        MouseContactMapper;
    typedef sofa::component::container::MechanicalObject< DataTypes >         MouseContainer;
    typedef sofa::core::componentmodel::behavior::BaseForceField              MouseForceField;

public:
    AttachBodyPerformer(BaseMouseInteractor *i);
    ~AttachBodyPerformer();

    void start();
    void execute();
    void draw();
    void clear();



protected:
    void start_partial(const BodyPicked& picked);
    /*
    initialise MouseForceField according to template.
    StiffSpringForceField for Vec3
    JointSpringForceField for Rigid3
    */

    MouseContactMapper   *mapper;
    MouseForceField      *forcefield;
};



#if defined(WIN32) && !defined(SOFA_COMPONENT_COLLISION_ATTACHBODYPERFORMER_CPP)
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_COLLISION_API  AttachBodyPerformer<defaulttype::Vec3fTypes>;
extern template class SOFA_COMPONENT_COLLISION_API  AttachBodyPerformer<defaulttype::Rigid3fTypes>;

#endif
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_COLLISION_API  AttachBodyPerformer<defaulttype::Vec3dTypes>;
extern template class SOFA_COMPONENT_COLLISION_API  AttachBodyPerformer<defaulttype::Rigid3dTypes>;
#endif
#endif


}
}
}

#endif
