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
#ifndef SOFA_COMPONENT_COLLISION_BARYCENTRICPENALITYCONTACT_INL
#define SOFA_COMPONENT_COLLISION_BARYCENTRICPENALITYCONTACT_INL

#include <sofa/component/collision/BarycentricPenalityContact.h>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;
using namespace core::componentmodel::collision;

template < class TCollisionModel1, class TCollisionModel2, class ResponseDataTypes >
BarycentricPenalityContact<TCollisionModel1,TCollisionModel2,ResponseDataTypes>::BarycentricPenalityContact(CollisionModel1* model1, CollisionModel2* model2, Intersection* intersectionMethod)
    : model1(model1), model2(model2), intersectionMethod(intersectionMethod), ff(NULL), parent(NULL)
{
    mapper1.setCollisionModel(model1);
    mapper2.setCollisionModel(model2);
}

template < class TCollisionModel1, class TCollisionModel2, class ResponseDataTypes >
BarycentricPenalityContact<TCollisionModel1,TCollisionModel2,ResponseDataTypes>::~BarycentricPenalityContact()
{
}


template < class TCollisionModel1, class TCollisionModel2, class ResponseDataTypes >
void BarycentricPenalityContact<TCollisionModel1,TCollisionModel2,ResponseDataTypes>::cleanup()
{
    if (ff!=NULL)
    {
        ff->cleanup();
        if (parent!=NULL) parent->removeObject(ff);
        delete ff;
        parent = NULL;
        ff = NULL;
        mapper1.cleanup();
        mapper2.cleanup();
    }
}

template < class TCollisionModel1, class TCollisionModel2, class ResponseDataTypes >
void BarycentricPenalityContact<TCollisionModel1,TCollisionModel2,ResponseDataTypes>::setDetectionOutputs(OutputVector* o)
{
    TOutputVector& outputs = *static_cast<TOutputVector*>(o);
    const bool printLog = this->f_printLog.getValue();
    if (ff==NULL)
    {
        MechanicalState1* mstate1 = mapper1.createMapping();
        MechanicalState2* mstate2 = mapper2.createMapping();
        ff = new ResponseForceField(mstate1,mstate2);
        ff->setName( getName() );
    }

    int insize = outputs.size();

    // old index for each contact
    // >0 indicate preexisting contact
    // 0  indicate new contact
    // -1 indicate ignored duplicate contact
    std::vector<int> oldIndex(insize);

    int nbnew = 0;

    for (int i=0; i<insize; i++)
    {
        DetectionOutput* o = &outputs[i];
        // find this contact in contactIndex, possibly creating a new entry initialized by 0
        int& index = contactIndex[o->id];
        if (index < 0) // duplicate contact
        {
            int i2 = -1-index;
            DetectionOutput* o2 = &outputs[i2];
            if (o2->value <= o->value)
            {
                // current contact is ignored
                oldIndex[i] = -1;
                continue;
            }
            else
            {
                // previous contact is replaced
                oldIndex[i] = oldIndex[i2];
                oldIndex[i2] = -1;
            }
        }
        else
        {
            oldIndex[i] = index;
            if (!index)
            {
                ++nbnew;
                if (printLog) std::cout << "BarycentricPenalityContact: New contact "<<o->id<<std::endl;
            }
        }
        index = -1-i; // save this index as a negative value in contactIndex map.
    }

    // compute new index of each contact
    std::vector<int> newIndex(insize);
    // number of final contacts used in the response
    int size = 0;
    for (int i=0; i<insize; i++)
    {
        if (oldIndex[i] >= 0)
        {
            ++size;
            newIndex[i] = size;
        }
    }

    // update contactMap
    for (ContactIndexMap::iterator it = contactIndex.begin(), itend = contactIndex.end(); it != itend; )
    {
        int& index = it->second;
        if (index >= 0)
        {
            if (printLog) std::cout << "BarycentricPenalityContact: Removed contact "<<it->first<<std::endl;
            ContactIndexMap::iterator oldit = it;
            ++it;
            contactIndex.erase(oldit);
        }
        else
        {
            index = newIndex[-1-index]; // write the final contact index
            ++it;
        }
    }
    if (printLog) std::cout << "BarycentricPenalityContact: "<<insize<<" input contacts, "<<size<<" contacts used for response ("<<nbnew<<" new)."<<std::endl;

    //int size = contacts.size();
    ff->clear(size);
    mapper1.resize(size);
    mapper2.resize(size);
    //int i = 0;
    const double d0 = intersectionMethod->getContactDistance() + model1->getProximity() + model2->getProximity(); // - 0.001;
    //for (std::vector<DetectionOutput>::iterator it = outputs.begin(); it!=outputs.end(); it++)
    //{
    //    DetectionOutput* o = &*it;
    for (int i=0; i<insize; i++)
    {
        int index = oldIndex[i];
        if (index < 0) continue; // this contact is ignored
        DetectionOutput* o = &outputs[i];
        CollisionElement1 elem1(o->elem.first);
        CollisionElement2 elem2(o->elem.second);
        int index1 = elem1.getIndex();
        int index2 = elem2.getIndex();
        double r1 = 0.0;
        double r2 = 0.0;
        // Create mapping for first point
        index1 = mapper1.addPoint(o->point[0], index1, r1);
        // Create mapping for second point
        index2 = mapper2.addPoint(o->point[1], index2, r2);

        double distance = d0 + r1 + r2;
        double stiffness = (elem1.getContactStiffness() * elem2.getContactStiffness());
        if (distance != 0.0) stiffness /= distance;

        double mu_v = (elem1.getContactFriction() + elem2.getContactFriction());
        ff->addContact(index1, index2, elem1.getIndex(), elem2.getIndex(), o->normal, distance, stiffness, mu_v/* *distance */, mu_v, index);
    }
    // Update mappings
    mapper1.update();
    mapper2.update();
}

template < class TCollisionModel1, class TCollisionModel2, class ResponseDataTypes >
void BarycentricPenalityContact<TCollisionModel1,TCollisionModel2,ResponseDataTypes>::createResponse(core::objectmodel::BaseContext* group)
{
    if (ff!=NULL)
    {
        if (parent!=NULL)
        {
            parent->removeObject(this);
            parent->removeObject(ff);
        }
        parent = group;
        if (parent!=NULL)
        {
            //std::cout << "Attaching contact response to "<<parent->getName()<<std::endl;
            parent->addObject(this);
            parent->addObject(ff);
        }
    }
}

template < class TCollisionModel1, class TCollisionModel2, class ResponseDataTypes >
void BarycentricPenalityContact<TCollisionModel1,TCollisionModel2,ResponseDataTypes>::removeResponse()
{
    if (ff!=NULL)
    {
        if (parent!=NULL)
        {
            //std::cout << "Removing contact response from "<<parent->getName()<<std::endl;
            parent->removeObject(this);
            parent->removeObject(ff);
        }
        parent = NULL;
    }
}

template < class TCollisionModel1, class TCollisionModel2, class ResponseDataTypes >
void BarycentricPenalityContact<TCollisionModel1,TCollisionModel2,ResponseDataTypes>::draw()
{
    //	if (ff!=NULL)
    //		ff->draw();
}

} // namespace collision

} // namespace component

} // namespace sofa

#endif
