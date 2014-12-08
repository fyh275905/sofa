/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 INRIA, USTL, UJF, CNRS, MGH                    *
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
* Authors: The SOFA Team (see Authors.txt)                                    *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_HELPER_COMPONENTVARIABLEDATA_H
#define SOFA_HELPER_COMPONENTVARIABLEDATA_H

#include <string>

#include <sofa/core/objectmodel/Base.h>
#include <sofa/core/objectmodel/Data.h>
#include <sofa/core/DataEngine.h>
#include <sofa/helper/helper.h>

namespace sofa
{

namespace helper
{

/** A helper class which implements a vector of a variable number of data
 *
 * @todo when the component is a DataEngine, the data are added as automatically inputs
 *
 * @author Thomas Lemaire @date 2014
 */
template<class T>
class vectorData : public vector< core::objectmodel::Data<T>* > {

public:
    vectorData(core::objectmodel::Base* component, std::string const& name, std::string const& help)
        : m_component(component)
        , m_name(name)
        , m_help(help)
    { }

    ~vectorData()
    {
        core::DataEngine* componentAsDataEngine = dynamic_cast<core::DataEngine*>(m_component);
        for (unsigned int i=0; i<this->size(); ++i)
        {
            if (componentAsDataEngine!=NULL)
                componentAsDataEngine->delInput((*this)[i]);
            delete (*this)[i];
        }
        this->clear();
    }

    void parseSizeData(sofa::core::objectmodel::BaseObjectDescription* arg, Data<unsigned int>& size)
    {
        const char* p = arg->getAttribute(size.getName().c_str());
        if (p) {
            std::string nbStr = p;
//            sout << "parse: setting " << size.getName() << "="<<nbStr<<sendl;
            size.read(nbStr);
            updateDataVectorSize(size.getValue());
        }
    }


    void parseFieldsSizeData(const std::map<std::string,std::string*>& str, Data<unsigned int>& size)
    {
        std::map<std::string,std::string*>::const_iterator it = str.find(size.getName());
        if (it != str.end() && it->second)
        {
            std::string nbStr = *it->second;
//            sout << "parseFields: setting "<< size.getName() << "=" <<nbStr<<sendl;
            size.read(nbStr);
            updateDataVectorSize(size.getValue());
        }
    }

    void updateDataVectorSize(unsigned int size)
    {
        core::DataEngine* componentAsDataEngine = dynamic_cast<core::DataEngine*>(m_component);
        // TODO delete if size is less
        for (unsigned int i=this->size(); i<size; ++i)
        {
            std::ostringstream oname, ohelp;
            oname << m_name << (i+1);
            ohelp << m_help << "(" << (i+1) << ")";
            Data< T >* d = new Data< T >(ohelp.str().c_str(), true, false);
            d->setName(oname.str());
            this->push_back(d);
            m_component->addData(d);
            if (componentAsDataEngine!=NULL)
                componentAsDataEngine->addInput(d);
        }
    }

protected:
    core::objectmodel::Base* m_component;
    std::string m_name, m_help;

};

} // namespace helper

} // namespace sofa

#endif // SOFA_HELPER_COMPONENTVARIABLEDATA_H
