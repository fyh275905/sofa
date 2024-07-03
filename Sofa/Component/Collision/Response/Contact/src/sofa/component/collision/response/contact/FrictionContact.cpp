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
#include <sofa/component/collision/response/contact/FrictionContact.inl>

#include <sofa/component/collision/response/mapper/RigidContactMapper.inl>
#include <sofa/component/collision/response/mapper/BarycentricContactMapper.inl>

namespace sofa::component::collision::response::contact
{

using namespace defaulttype;
using namespace sofa::helper;
using namespace sofa::component::collision::geometry;
using simulation::Node;

Creator<sofa::core::collision::Contact::Factory, FrictionContact<PointCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > PointPointFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > LineSphereFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > LinePointFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > LineLineFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TriangleSphereFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TrianglePointFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TriangleLineFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, TriangleCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TriangleTriangleFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > SphereSphereFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > SpherePointFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<RigidSphereModel, RigidSphereModel,FrictionType::COULOMB> > RigidSphereRigidSphereFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::COULOMB> > SphereRigidSphereFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::COULOMB> > LineRigidSphereFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::COULOMB> > TriangleRigidSphereFrictionContactClass("FrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<RigidSphereModel, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > RigidSpherePointFrictionContactClass("FrictionContactConstraint",true);

template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<PointCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, TriangleCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<RigidSphereModel, RigidSphereModel,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<RigidSphereModel, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;


Creator<sofa::core::collision::Contact::Factory, FrictionContact<PointCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > PointPointViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > LineSphereViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > LinePointViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > LineLineViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TriangleSphereViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TrianglePointViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TriangleLineViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, TriangleCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TriangleTriangleViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > SphereSphereViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > SpherePointViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<RigidSphereModel, RigidSphereModel,FrictionType::VISCOUS> > RigidSphereRigidSphereViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::VISCOUS> > SphereRigidSphereViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::VISCOUS> > LineRigidSphereViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::VISCOUS> > TriangleRigidSphereViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<sofa::core::collision::Contact::Factory, FrictionContact<RigidSphereModel, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > RigidSpherePointViscousFrictionContactClass("ViscousFrictionContactConstraint",true);

template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<PointCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, TriangleCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<RigidSphereModel, RigidSphereModel,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<SphereCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<LineCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, RigidSphereModel,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API FrictionContact<RigidSphereModel, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;

} //namespace sofa::component::collision::response::contact
