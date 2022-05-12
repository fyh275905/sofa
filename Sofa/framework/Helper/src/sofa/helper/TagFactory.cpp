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
#include <sofa/helper/TagFactory.h>
#include <sofa/helper/logging/Messaging.h>

namespace sofa::helper
{

TagFactory::TagFactory()
{
    tagsList.emplace_back("0"); // ID 0 == "0" or empty string
    // Add standard tags
    tagsList.emplace_back("Visual");
}

unsigned int TagFactory::getID(const std::string& name)
{
    if (name.empty()) return 0;

    TagFactory * tagfac = TagFactory::getInstance();
    std::lock_guard lockit(tagfac->m_mutex);

    auto it = tagfac->tagsList.begin();
    unsigned int i=0;

    while(it != tagfac->tagsList.end() && *it != name)
    {
        ++it;
        i++;
    }

    if (it != tagfac->tagsList.end())
        return i;

#ifndef NDEBUG
        msg_info("TagFactory") << "creating new tag " << i << ": " <<name;
#endif

    tagfac->tagsList.push_back(name);
    return i;
}

std::string TagFactory::getName(const unsigned int id)
{
    TagFactory * tagfac = TagFactory::getInstance();
    std::lock_guard lockit(tagfac->m_mutex);

    if( id < tagfac->tagsList.size() )
        return tagfac->tagsList[id];
    return "";
}

TagFactory* TagFactory::getInstance()
{
    static TagFactory instance;
    return &instance;
}
} // namespace sofa::helper

