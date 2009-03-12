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
#include "CudaDistanceGridCollisionModel.h"
#include "CudaSphereModel.h"
#include "CudaPointModel.h"
#include <sofa/component/collision/NewProximityIntersection.inl>
#include <sofa/component/collision/DiscreteIntersection.inl>
#include <sofa/component/collision/RayPickInteractor.inl>
#include <sofa/component/collision/RayContact.h>
#include "CudaContactMapper.h"
#include <sofa/component/collision/BarycentricPenalityContact.inl>
#include <sofa/component/collision/BarycentricContactMapper.inl>
#include <sofa/component/forcefield/PenalityContactForceField.h>
#include "CudaPenalityContactForceField.h"
#include "CudaSpringForceField.h"
#include <sofa/component/forcefield/VectorSpringForceField.h>
#include <fstream>
#include <sofa/helper/system/gl.h>
#include <sofa/helper/Factory.inl>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::gpu::cuda;

template <>
void BarycentricPenalityContact<CudaPointModel,CudaRigidDistanceGridCollisionModel,CudaVec3fTypes>::setDetectionOutputs(OutputVector* o)
{
    TOutputVector& outputs = *static_cast<TOutputVector*>(o);
    //const bool printLog = this->f_printLog.getValue();
    if (ff==NULL)
    {
        MechanicalState1* mstate1 = mapper1.createMapping("contactPointsCUDA");
        MechanicalState2* mstate2 = mapper2.createMapping("contactPointsCUDA");
        ff = new ResponseForceField(mstate1,mstate2); ff->setName( getName());
    }

    mapper1.setPoints1(&outputs);
    mapper2.setPoints2(&outputs);
    const double d0 = intersectionMethod->getContactDistance() + model1->getProximity() + model2->getProximity(); // - 0.001;
#if 0
    int insize = outputs.size();
    int size = insize;
    ff->clear(size);
    //int i = 0;
    for (int i=0; i<insize; i++)
    {
        int index = i; //oldIndex[i];
        if (index < 0) continue; // this contact is ignored
        //DetectionOutput* o = &outputs[i];
        //CollisionElement1 elem1(o->elem.first);
        //CollisionElement2 elem2(o->elem.second);
        //int index1 = elem1.getIndex();
        //int index2 = elem2.getIndex();
        int index1 = index;
        int index2 = index;
        //// Create mapping for first point
        //index1 = mapper1.addPoint(o->point[0], index1);
        //// Create mapping for second point
        //index2 = mapper2.addPoint(o->point[1], index2);
        double distance = d0 + outputs.get(i)->distance; // + mapper1.radius(elem1) + mapper2.radius(elem2);
        double stiffness = (model1->getContactStiffness(0) * model1->getContactStiffness(0))/distance;
        double mu_v = (model1->getContactFriction(0) + model1->getContactFriction(0));
        ff->addContact(index1, index2, outputs.get(i)->normal, (float)distance, (float)stiffness, (float)mu_v, (float)mu_v, index);
    }
#else
    double distance = d0; // + mapper1.radius(elem1) + mapper2.radius(elem2);
    double stiffness = (model1->getContactStiffness(0) * model1->getContactStiffness(0)); ///distance;
    ff->setContacts((float)distance, (float)stiffness, &outputs, true);
#endif
    // Update mappings
    mapper1.update();
    mapper2.update();
}

template <>
void BarycentricPenalityContact<CudaSphereModel,CudaRigidDistanceGridCollisionModel,CudaVec3fTypes>::setDetectionOutputs(OutputVector* o)
{

    TOutputVector& outputs = *static_cast<TOutputVector*>(o);
    //const bool printLog = this->f_printLog.getValue();
    if (ff==NULL)
    {
        MechanicalState1* mstate1 = mapper1.createMapping("contactPointsCUDA");
        MechanicalState2* mstate2 = mapper2.createMapping("contactPointsCUDA");
        ff = new ResponseForceField(mstate1,mstate2); ff->setName( getName());
    }

    mapper1.setPoints1(&outputs);
    mapper2.setPoints2(&outputs);
    const double d0 = intersectionMethod->getContactDistance() + model1->getProximity() + model2->getProximity(); // - 0.001;
#if 0
    int insize = outputs.size();
    int size = insize;
    ff->clear(size);
    //int i = 0;
    for (int i=0; i<insize; i++)
    {
        int index = i; //oldIndex[i];
        if (index < 0) continue; // this contact is ignored
        //DetectionOutput* o = &outputs[i];
        //CollisionElement1 elem1(o->elem.first);
        //CollisionElement2 elem2(o->elem.second);
        //int index1 = elem1.getIndex();
        //int index2 = elem2.getIndex();
        int index1 = index;
        int index2 = index;
        //// Create mapping for first point
        //index1 = mapper1.addPoint(o->point[0], index1);
        //// Create mapping for second point
        //index2 = mapper2.addPoint(o->point[1], index2);
        double distance = d0 + outputs.get(i)->distance; // + mapper1.radius(elem1) + mapper2.radius(elem2);
        double stiffness = (model1->getContactStiffness(0) * model1->getContactStiffness(0))/distance;
        double mu_v = (model1->getContactFriction(0) + model1->getContactFriction(0));
        ff->addContact(index1, index2, outputs.get(i)->normal, (float)distance, (float)stiffness, (float)mu_v, (float)mu_v, index);
    }
#else
    double distance = d0; // + mapper1.radius(elem1) + mapper2.radius(elem2);
    double stiffness = (model1->getContactStiffness(0) * model1->getContactStiffness(0)); ///distance;
    ff->setContacts((float)distance, (float)stiffness, &outputs, true);
#endif
    // Update mappings
    mapper1.update();
    mapper2.update();

}

ContactMapperCreator< ContactMapper<CudaSphereModel> > CudaSphereContactMapperClass("default",true);

template<>
void DefaultPickingManager<CudaVec3fTypes,forcefield::StiffSpringForceField<CudaVec3fTypes> >::addContact(forcefield::StiffSpringForceField<CudaVec3fTypes>* ff, int index1, int index2, double stiffness, double mu_v, double length, const Vector3& /*p1*/, const Vector3& /*p2*/)
{
    ff->addSpring(index1, index2, stiffness, mu_v, length);
}

template class DefaultPickingManager< CudaVec3fTypes, forcefield::StiffSpringForceField<CudaVec3fTypes> >;


helper::Creator<BasePickingManager::PickingManagerFactory, DefaultPickingManager< CudaVec3fTypes, forcefield::StiffSpringForceField<CudaVec3fTypes> > > PickingVectorSpringCudaClass ("VectorSpringCUDA",true);

} //namespace collision


} //namespace component


namespace gpu
{

namespace cuda
{

SOFA_DECL_CLASS(CudaCollision)

using namespace sofa::component::collision;

class CudaProximityIntersection : public sofa::component::collision::NewProximityIntersection
{
public:

    virtual void init()
    {
        sofa::component::collision::NewProximityIntersection::init();
        intersectors.add<RayModel,        CudaSphereModel,   DiscreteIntersection>(this);
        //intersectors.add<CudaSphereModel, PointModel,        DiscreteIntersection>(this);
        intersectors.add<CudaSphereModel, CudaSphereModel,   DiscreteIntersection>(this);
        //intersectors.add<LineModel,       CudaSphereModel,   CudaProximityIntersection>(this);
        intersectors.add<TriangleModel,   CudaSphereModel,   CudaProximityIntersection>(this);
    }

};


int CudaProximityIntersectionClass = core::RegisterObject("GPGPU Proximity Intersection based on CUDA")
        .add< CudaProximityIntersection >()
        ;

sofa::helper::Creator<core::componentmodel::collision::Contact::Factory, component::collision::RayContact<CudaSphereModel> > RayCudaSphereContactClass("ray",true);

//sofa::helper::Creator<sofa::core::componentmodel::collision::Contact::Factory, sofa::component::collision::BarycentricPenalityContact<CudaRigidDistanceGridCollisionModel, CudaRigidDistanceGridCollisionModel,CudaVec3fTypes> > CudaDistanceGridCudaDistanceGridContactClass("default", true);
sofa::helper::Creator<sofa::core::componentmodel::collision::Contact::Factory, sofa::component::collision::BarycentricPenalityContact<CudaPointModel, CudaRigidDistanceGridCollisionModel,CudaVec3fTypes> > CudaPointCudaDistanceGridContactClass("default", true);
sofa::helper::Creator<sofa::core::componentmodel::collision::Contact::Factory, sofa::component::collision::BarycentricPenalityContact<CudaSphereModel, CudaRigidDistanceGridCollisionModel,CudaVec3fTypes> > CudaSphereCudaDistanceGridContactClass("default", true);
//sofa::helper::Creator<sofa::core::componentmodel::collision::Contact::Factory, sofa::component::collision::BarycentricPenalityContact<CudaRigidDistanceGridCollisionModel, sofa::component::collision::RigidDistanceGridCollisionModel> > CudaDistanceGridDistanceGridContactClass("default", true);
//sofa::helper::Creator<sofa::core::componentmodel::collision::Contact::Factory, sofa::component::collision::BarycentricPenalityContact<CudaRigidDistanceGridCollisionModel, sofa::component::collision::PointModel> > CudaDistanceGridPointContactClass("default", true);
//sofa::helper::Creator<sofa::core::componentmodel::collision::Contact::Factory, sofa::component::collision::BarycentricPenalityContact<CudaRigidDistanceGridCollisionModel, sofa::component::collision::SphereModel> > CudaDistanceGridSphereContactClass("default", true);
//sofa::helper::Creator<sofa::core::componentmodel::collision::Contact::Factory, sofa::component::collision::BarycentricPenalityContact<CudaRigidDistanceGridCollisionModel, sofa::component::collision::TriangleModel> > CudaDistanceGridTriangleContactClass("default", true);


} // namespace cuda

} // namespace gpu

} // namespace sofa
