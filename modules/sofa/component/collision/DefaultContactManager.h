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
#ifndef SOFA_COMPONENT_COLLISION_DEFAULTCONTACTMANAGER_H
#define SOFA_COMPONENT_COLLISION_DEFAULTCONTACTMANAGER_H

#include <sofa/core/collision/ContactManager.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/component/component.h>
#include <sofa/helper/OptionsGroup.h>
#include <vector>


namespace sofa
{

namespace component
{

namespace collision
{

class SOFA_COMPONENT_COLLISION_API DefaultContactManager : public core::collision::ContactManager
{
public :
    SOFA_CLASS(DefaultContactManager,sofa::core::collision::ContactManager);

protected:
    typedef std::map<std::pair<core::CollisionModel*,core::CollisionModel*>,core::collision::Contact*> ContactMap;
    ContactMap contactMap;

    void cleanup();
public:
    Data<sofa::helper::OptionsGroup> response;

    DefaultContactManager();
    ~DefaultContactManager();

    void createContacts(DetectionOutputMap& outputs);

    void draw();

    template<class T>
    static void create(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        obj = new T;

        if (context)
        {
            context->addObject(obj);
            core::collision::Pipeline *pipeline=static_cast<simulation::Node*>(context)->collisionPipeline;

            helper::set<std::string> listResponse;
            if (pipeline) listResponse=pipeline->getResponseList();
            else
            {
                obj->serr << "No collision pipeline found: using default options for collision response" << obj->sendl;

                core::collision::Contact::Factory::iterator it;
                for (it=core::collision::Contact::Factory::getInstance()->begin(); it!=core::collision::Contact::Factory::getInstance()->end(); ++it)
                {
                    listResponse.insert(it->first);
                }
            }
            sofa::helper::OptionsGroup responseOptions(listResponse);
            if (listResponse.find("default") != listResponse.end())
                responseOptions.setSelectedItem("default");
            obj->response.setValue(responseOptions);
        }
        if (arg) obj->parse(arg);

    }

    virtual std::string getContactResponse(core::CollisionModel* model1, core::CollisionModel* model2);

    /// virtual methods used for cleaning the pipeline after a dynamic graph node deletion.
    /**
     * Contacts can be attached to a deleted node and their deletion is a problem for the pipeline.
     * @param c is the list of deleted contacts.
     */
    virtual void removeContacts(const ContactVector &/*c*/);


protected:

    std::map<Instance,ContactMap> storedContactMap;

    virtual void changeInstance(Instance inst)
    {
        core::collision::ContactManager::changeInstance(inst);
        storedContactMap[instance].swap(contactMap);
        contactMap.swap(storedContactMap[inst]);
    }
};

} // namespace collision

} // namespace component

} // namespace sofa

#endif
