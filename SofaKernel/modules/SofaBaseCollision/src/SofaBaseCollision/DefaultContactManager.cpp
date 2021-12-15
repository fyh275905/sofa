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
#include <SofaBaseCollision/DefaultContactManager.h>

#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/objectmodel/Tag.h>
#include <sofa/simulation/Node.h>
#include <sofa/core/collision/Pipeline.h>

namespace sofa::component::collision
{

int DefaultContactManagerClass = core::RegisterObject("Default class to create reactions to the collisions")
        .add< DefaultContactManager >()
        .addAlias("CollisionResponse")
        ;

DefaultContactManager::DefaultContactManager()
    : response(initData(&response, "response", "contact response class"))
    , responseParams(initData(&responseParams, "responseParams", "contact response parameters (syntax: name1=value1&name2=value2&...)"))
{
}

sofa::helper::OptionsGroup DefaultContactManager::initializeResponseOptions(sofa::core::objectmodel::BaseContext *context)
{
    std::set<std::string> listResponse;

    sofa::simulation::Node* node = sofa::simulation::node::getNodeFrom(context);
    sofa::core::collision::Pipeline* pipeline = node->collisionPipeline;
    if (pipeline)
    {
        listResponse=pipeline->getResponseList();
    }
    else
    {
        core::collision::Contact::Factory::iterator it;
        for (it=core::collision::Contact::Factory::getInstance()->begin(); it!=core::collision::Contact::Factory::getInstance()->end(); ++it)
        {
            listResponse.insert(it->first);
        }
    }

    sofa::helper::OptionsGroup responseOptions(listResponse);
    if (listResponse.find("PenalityContactForceField") != listResponse.end())
        responseOptions.setSelectedItem("PenalityContactForceField");

    return responseOptions;
}

void DefaultContactManager::init()
{
    if (response.getValue().size() == 0)
    {
        response.setValue(initializeResponseOptions(getContext()));
    }
}

void DefaultContactManager::cleanup()
{
    for (auto& contact : contacts)
    {
        contact->removeResponse();
        contact->cleanup();
        //delete *it;
        contact.reset();
    }
    contacts.clear();
    contactMap.clear();
}

void DefaultContactManager::reset()
{
    cleanup();
}

void DefaultContactManager::setDefaultResponseType(const std::string &responseT)
{
    if (response.getValue().size() == 0)
    {
        type::vector<std::string> listResponse(1,responseT);
        sofa::helper::OptionsGroup responseOptions(listResponse);
        response.setValue(responseOptions);
    }
    else
    {
        sofa::helper::OptionsGroup* options = response.beginEdit();
        options->setSelectedItem(responseT);
        response.endEdit();
    }
}


void DefaultContactManager::changeInstance(Instance inst)
{
    core::collision::ContactManager::changeInstance(inst);
    storedContactMap[instance].swap(contactMap);
    contactMap.swap(storedContactMap[inst]);
}

void DefaultContactManager::createContacts(const DetectionOutputMap& outputsMap)
{
    Size nbContacts = 0;

    // First iterate on the collision detection outputs and look for existing or new contacts
    createNewContacts(outputsMap, nbContacts);

    // Then look at previous contacts
    // and remove inactive contacts
    removeInactiveContacts(outputsMap, nbContacts);

    // now update contact vector from the contact map
    contacts.clear();
    contacts.reserve(nbContacts);
    std::transform(contactMap.cbegin(), contactMap.cend(), std::back_inserter(contacts),
                   [](const auto& pair){return pair.second;});

    // notify each collision model how many contacts has been detected on it
    setNumberOfContacts();
}

void DefaultContactManager::createNewContacts(const core::collision::ContactManager::DetectionOutputMap &outputsMap,
                                              Size &nbContact)
{
    std::stringstream errorStream;

    for (DetectionOutputMap::const_iterator outputsIt = outputsMap.begin(),
        outputsItEnd = outputsMap.end(); outputsIt != outputsItEnd ; ++outputsIt)
    {
        const auto contactInsert = contactMap.insert(ContactMap::value_type(outputsIt->first, core::collision::Contact::SPtr()));
        ContactMap::iterator contactIt = contactInsert.first;
        if (contactInsert.second) //insertion success
        {
            // new contact
            core::CollisionModel* model1 = outputsIt->first.first;
            core::CollisionModel* model2 = outputsIt->first.second;

            dmsg_error_when(model1 == nullptr || model2 == nullptr) << "Contact found with an invalid collision model";

            std::string responseUsed = getContactResponse(model1, model2);

            // We can create rules in order to not respond to specific collisions
            if (!responseUsed.compare("nullptr"))
            {
                contactMap.erase(contactIt);
            }
            else
            {
                auto contact = core::collision::Contact::Create(responseUsed, model1, model2, intersectionMethod,notMuted());

                if (contact == nullptr)
                {
                    //contact couln't be created: write an error and collision detection output is no longer considered
                    contactCreationError(errorStream, model1, model2, responseUsed);
                    contactMap.erase(contactIt);
                }
                else
                {
                    //add the contact to the list of contacts and setup some data
                    contactIt->second = contact;
                    contact->setName(model1->getName() + std::string("-") + model2->getName());
                    setContactTags(model1, model2, contact);
                    contact->f_printLog.setValue(notMuted());
                    contact->init();
                    contact->setDetectionOutputs(outputsIt->second);
                    ++nbContact;
                }
            }
        }
        else
        {
            // pre-existing and still active contact
            contactIt->second->setDetectionOutputs(outputsIt->second);
            ++nbContact;
        }
    }

    msg_error_when(!errorStream.str().empty()) << errorStream.str();
}

void
DefaultContactManager::removeInactiveContacts(const core::collision::ContactManager::DetectionOutputMap &outputsMap,
                                              Size& nbContact)
{
    for (auto contactIt = contactMap.begin(), contactItEnd = contactMap.end();
         contactIt != contactItEnd;)
    {
        core::collision::Contact::SPtr contact = contactIt->second;
        dmsg_error_when(contact == nullptr) << "Checking if inactive on invalid contact";

        if (outputsMap.find(contactIt->first) == outputsMap.end())
        {
            //contact is not found among the result of the collision detection during this time step
            //the contact comes from a previous time step

            if (contact->keepAlive())
            {
                contact->setDetectionOutputs(nullptr);
                ++nbContact;
                ++contactIt;
            }
            else
            {
                contact->removeResponse();
                contact->cleanup();
                contact.reset();
                contactIt = contactMap.erase(contactIt);
            }
        }
        else
        {
            ++contactIt;
        }
    }
}

void
DefaultContactManager::contactCreationError(std::stringstream &errorStream, const core::CollisionModel *model1,
                                            const core::CollisionModel *model2, std::string &responseUsed)
{
    const std::string model1class = model1->getClassName();
    const std::string model2class = model2->getClassName();
    int count = ++errorMsgCount[std::make_pair(responseUsed,
                                               std::make_pair(model1class, model2class))];
    constexpr int nbMaxMessages { 10 };
    if (count <= nbMaxMessages)
    {
        errorStream << "Contact " << responseUsed << " between " << model1->getClassName()
            << " and " << model2->getClassName() << " creation failed \n";
        if (count == 1)
        {
            errorStream << "Supported models for contact " << responseUsed << ":\n";
            for (const auto& contact : *core::collision::Contact::Factory::getInstance())
            {
                if (contact.first == responseUsed)
                {
                    errorStream << "   " << helper::gettypename(contact.second->type()) << "\n";
                }
            }
            errorStream << "\n";
        }
        if (count == nbMaxMessages)
        {
            errorStream << "further messages suppressed.\n";
        }
    }
}

void DefaultContactManager::setNumberOfContacts() const
{
    std::map< core::CollisionModel*, int > nbContactsMap;
    for (const auto& contact: contacts)
    {
        const std::pair< core::CollisionModel*, core::CollisionModel* > cms = contact->getCollisionModels();
        nbContactsMap[cms.first]++;
        if (cms.second != cms.first)
            nbContactsMap[cms.second]++;
    }

    for (const auto& [collisionModel, nbContacts] : nbContactsMap)
    {
        if (collisionModel != nullptr)
        {
            collisionModel->setNumberOfContacts(nbContacts);
        }
    }
}

std::string DefaultContactManager::getContactResponse(core::CollisionModel* model1, core::CollisionModel* model2)
{
    std::string responseUsed = response.getValue().getSelectedItem();
    std::string params = responseParams.getValue();
    if (!params.empty())
    {
        responseUsed += '?';
        responseUsed += params;
    }
    std::string response1 = model1->getContactResponse();
    std::string response2 = model2->getContactResponse();

    if (response1.empty()  &&  response2.empty()) return responseUsed;
    if (response1.empty()  && !response2.empty()) return response2;
    if (!response1.empty() &&  response2.empty()) return response1;

    if (response1 != response2) return responseUsed;
    else return response1;
}

void DefaultContactManager::draw(const core::visual::VisualParams* vparams)
{
    for (const auto& contact : contacts)
    {
        if (contact != nullptr)
        {
            contact->draw(vparams);
        }
    }
}

void DefaultContactManager::removeContacts(const ContactVector &c)
{
    auto remove_it = c.begin();
    auto remove_itEnd = c.end();

    ContactVector::iterator it;
    ContactVector::iterator itEnd;

    ContactMap::iterator map_it;
    ContactMap::iterator map_itEnd;

    while (remove_it != remove_itEnd)
    {
        // Whole scene contacts
        it = contacts.begin();
        itEnd = contacts.end();

        while (it != itEnd)
        {
            if (*it == *remove_it)
            {
                (*it)->removeResponse();
                (*it)->cleanup();
                it->reset();
                contacts.erase(it);
                break;
            }

            ++it;
        }

        // Stored contacts (keeping alive)
        map_it = contactMap.begin();
        map_itEnd = contactMap.end();

        while (map_it != map_itEnd)
        {
            if (map_it->second == *remove_it)
            {
                auto erase_it = map_it;
                ++map_it;

                erase_it->second->removeResponse();
                erase_it->second->cleanup();
                erase_it->second.reset();
                contactMap.erase(erase_it);
            }
            else
            {
                ++map_it;
            }
        }

        ++remove_it;
    }
}

void DefaultContactManager::setContactTags(core::CollisionModel* model1, core::CollisionModel* model2, core::collision::Contact::SPtr contact)
{
    if (contact != nullptr)
    {
        for (auto* model : {model1, model2})
        {
            if (model)
            {
                for (const auto &tag : model->getTags())
                {
                    contact->addTag(tag);
                }
            }
        }
    }
}

} // namespace sofa::component::collision
