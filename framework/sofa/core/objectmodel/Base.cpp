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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
//
// C++ Implementation: Base
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <sofa/core/objectmodel/Base.h>
#include <sofa/helper/Factory.h>
#include <map>
#include <typeinfo>
#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include <string.h>
namespace sofa
{

namespace core
{

namespace objectmodel
{

using std::string;

Base::Base():sendl(f_printLog.getValue())
    , serr(*(sendl.serr)), sout(*(sendl.sout))
{
    name = initData(&name,std::string("unnamed"),"name","object name");
    f_printLog = initData( &f_printLog, false, "printLog", "if true, print logs at run-time");
}

Base::~Base()
{}

std::string Base::getName() const
{
    return name.getValue();
}

void Base::setName(const std::string& na)
{
    name.setValue(na);
    sendl.nameComponent = na;
    sendl.nameClass = getClassName();
}


/// Helper method to decode the type name
std::string Base::decodeFullName(const std::type_info& t)
{
    std::string name = t.name();
    char* allocname = strdup(name.c_str());
#ifdef __GNUC__
    int status;
    allocname = abi::__cxa_demangle(allocname, 0, 0, &status);
#endif
    return allocname;
}
/// Decode the type's name to a more readable form if possible
std::string Base::decodeTypeName(const std::type_info& t)
{
    std::string name = t.name();
    const char* realname = NULL;
    char* allocname = strdup(name.c_str());
#ifdef __GNUC__
    int status;
    realname = allocname = abi::__cxa_demangle(allocname, 0, 0, &status);
    if (realname==NULL)
#endif
        realname = allocname;
    int len = strlen(realname);
    char* newname = (char*)malloc(len+1);
    int start = 0;
    int dest = 0;
    char cprev = '\0';
    //sout << "name = "<<realname<<sendl;
    for (int i=0; i<len; i++)
    {
        char c = realname[i];
        if (c == ':') // && cprev == ':')
        {
            start = i+1;
        }
        else if (c == ' ' && i >= 5 && realname[i-5] == 'c' && realname[i-4] == 'l' && realname[i-3] == 'a' && realname[i-2] == 's' && realname[i-1] == 's')
        {
            start = i+1;
        }
        else if (c != ':' && c != '_' && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z'))
        {
            // write result
            while (start < i)
            {
                newname[dest++] = realname[start++];
                newname[dest] = 0;
            }
        }
        cprev = c;
        //sout << "i = "<<i<<" start = "<<start<<" dest = "<<dest<<" newname = "<<newname<<sendl;
    }
    while (start < len)
    {
        newname[dest++] = realname[start++];
        newname[dest] = 0;
    }
    newname[dest] = '\0';
    //sout << "newname = "<<newname<<sendl;
    name = newname;
    free(newname);
    //if (allocname)
    //    free(allocname);
    return name;
    /*
        // Remove namespaces
        std::string cname;
        for(;;)
        {
            std::string::size_type pos = name.find("::");
            if (pos == std::string::npos) break;
            std::string::size_type first = name.find_last_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_",pos-1);
            if (first == std::string::npos) first = 0;
            else first++;
            name.erase(first,pos-first+2);
            //sout << "name="<<name<<sendl;
        }
        // Remove "class "
        for(;;)
        {
            std::string::size_type pos = name.find("class ");
            if (pos == std::string::npos) break;
            name.erase(pos,6);
        }
        //sout << "TYPE NAME="<<name<<sendl;
        return name;
    */
}

/// Extract the class name (removing namespaces and templates)
std::string Base::decodeClassName(const std::type_info& t)
{
    std::string name = t.name();
    const char* realname = NULL;
    char* allocname = strdup(name.c_str());
#ifdef __GNUC__
    int status;
    realname = allocname = abi::__cxa_demangle(allocname, 0, 0, &status);
    if (realname==NULL)
#endif
        realname = allocname;
    int len = strlen(realname);
    char* newname = (char*)malloc(len+1);
    int start = 0;
    int dest = 0;
    int i;
    char cprev = '\0';
    //sout << "name = "<<realname<<sendl;
    for (i=0; i<len; i++)
    {
        char c = realname[i];
        if (c == '<') break;
        if (c == ':') // && cprev == ':')
        {
            start = i+1;
        }
        else if (c == ' ' && i >= 5 && realname[i-5] == 'c' && realname[i-4] == 'l' && realname[i-3] == 'a' && realname[i-2] == 's' && realname[i-1] == 's')
        {
            start = i+1;
        }
        else if (c == ' ' && i >= 6 && realname[i-6] == 's' && realname[i-5] == 't' && realname[i-4] == 'r' && realname[i-3] == 'u' && realname[i-2] == 'c' && realname[i-1] == 't')
        {
            start = i+1;
        }
        else if (c != ':' && c != '_' && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z'))
        {
            // write result
            while (start < i)
            {
                newname[dest++] = realname[start++];
                newname[dest] = 0;
            }
        }
        cprev = c;
        //sout << "i = "<<i<<" start = "<<start<<" dest = "<<dest<<" newname = "<<newname<<sendl;
    }

    while (start < i)
    {
        newname[dest++] = realname[start++];
        newname[dest] = 0;
    }
    newname[dest] = '\0';
    //sout << "newname = "<<newname<<sendl;
    name = newname;
    free(newname);
    //if (allocname)
    //    free(allocname);
    return name;
    /*
        std::string name = decodeTypeName(t);
        // Find template
        std::string::size_type pos = name.find("<");
        if (pos != std::string::npos)
        {
            name.erase(pos,name.length()-pos);
        }
        //sout << "CLASS NAME="<<name<<sendl;
        return name;
    */
}

/// Extract the namespace (removing class name and templates)
std::string Base::decodeNamespaceName(const std::type_info& t)
{
    std::string name = t.name();
    const char* realname = NULL;
    char* allocname = strdup(name.c_str());
#ifdef __GNUC__
    int status;
    realname = allocname = abi::__cxa_demangle(allocname, 0, 0, &status);
    if (realname==NULL)
#endif
        realname = allocname;
    int len = strlen(realname);
    int start = 0;
    int last = len-1;
    int i;
    for (i=0; i<len; i++)
    {
        char c = realname[i];
        if (c == ' ' && i >= 5 && realname[i-5] == 'c' && realname[i-4] == 'l' && realname[i-3] == 'a' && realname[i-2] == 's' && realname[i-1] == 's')
        {
            start = i+1;
        }
        else if (c == ':' && (i<1 || realname[i-1]!=':'))
        {
            last = i-1;
        }
        else if (c != ':' && c != '_' && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z'))
        {
            // write result
            break;
        }
    }
    name.assign(realname+start, realname+last+1);
    //if (allocname)
    //    free(allocname);
    return name;
}

/// Decode the template name (removing namespaces and class name)
std::string Base::decodeTemplateName(const std::type_info& t)
{
    std::string name = t.name();
    const char* realname = NULL;
    char* allocname = strdup(name.c_str());
#ifdef __GNUC__
    int status;
    realname = allocname = abi::__cxa_demangle(allocname, 0, 0, &status);
    if (realname==NULL)
#endif
        realname = allocname;
    int len = strlen(realname);
    char* newname = (char*)malloc(len+1);
    newname[0] = '\0';
    int start = 0;
    int dest = 0;
    int i = 0;
    char cprev = '\0';
//     std::cerr  << "name = "<<realname<<std::endl;
    while (i < len && realname[i]!='<')
        ++i;
    start = i+1; ++i;
    for (; i<len; i++)
    {
        char c = realname[i];
        //if (c == '<') break;
        if (c == ':') // && cprev == ':')
        {
            start = i+1;
        }
        else if (c == ' ' && i >= 5 && realname[i-5] == 'c' && realname[i-4] == 'l' && realname[i-3] == 'a' && realname[i-2] == 's' && realname[i-1] == 's')
        {
            start = i+1;
        }
        else if (c != ':' && c != '_' && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z'))
        {
            // write result
            while (start <= i)
            {
                newname[dest++] = realname[start++];
                newname[dest] = 0;
            }
        }
        cprev = c;
        //sout << "i = "<<i<<" start = "<<start<<" dest = "<<dest<<" newname = "<<newname<<sendl;
    }
    while (start < i)
    {
        newname[dest++] = realname[start++];
        newname[dest] = 0;
    }
    newname[dest] = '\0';
    //sout << "newname = "<<newname<<sendl;
    name = newname;
    free(newname);
    //if (allocname)
    //    free(allocname);
    return name;
    /*
        std::string name = decodeTypeName(t);
        // Find template
        std::string::size_type pos = name.find("<");
        if (pos != std::string::npos)
        {
            name = name.substr(pos+1,name.length()-pos-2);
        }
        else
        {
            name = "";
        }
        //sout << "TEMPLATE NAME="<<name<<sendl;
        return name;
    */
}

/// Find a field given its name. Return NULL if not found. If more than one field is found (due to aliases), only the first is returned.
BaseData* Base::findField( const std::string &name ) const
{
    std::string ln(name);
    //Search in the aliases
    typedef std::multimap< std::string, BaseData* >::const_iterator multimapIterator;
    std::pair< multimapIterator, multimapIterator> range = m_aliasData.equal_range(name);
    if (range.first != range.second)
        return range.first->second;
    else
        return NULL;
}

/// Find fields given a name: several can be found as we look into the alias map
std::vector< BaseData* > Base::findGlobalField( const std::string &name ) const
{
    std::string ln(name);
    std::vector<BaseData*> result;
    //Search in the aliases
    typedef std::multimap< std::string, BaseData* >::const_iterator multimapIterator;
    std::pair< multimapIterator, multimapIterator> range = m_aliasData.equal_range(name);
    for (multimapIterator itAlias=range.first; itAlias!=range.second; itAlias++)
        result.push_back(itAlias->second);
    return result;
}

void  Base::parseFields ( std::list<std::string> str )
{
    string name;
    while( !str.empty() )
    {
        name = str.front();
        str.pop_front();
        // field name
        std::vector< BaseData* > fields=findGlobalField(name);
        if( fields.size() != 0 )
        {
            std::string s = str.front();
            for (unsigned int i=0; i<fields.size(); ++i)
            {
                if( !(fields[i]->read( s ))) serr<<"could not read value for option " << name <<": "<< s << sendl;
            }
        }
        else
        {
            str.pop_front();
            serr<<"Unknown option: "<< name << sendl;
        }
    }
}

void  Base::parseFields ( const std::map<std::string,std::string*>& args )
{
    // build  std::list<std::string> str
    using std::string;
    string key,val;
    for( std::map<string,string*>::const_iterator i=args.begin(), iend=args.end(); i!=iend; i++ )
    {
        if( (*i).second!=NULL )
        {
            key=(*i).first;
            val=*(*i).second;
            std::vector< BaseData* > fields=findGlobalField(key);
            if( fields.size() != 0 )
            {
                for (unsigned int i=0; i<fields.size(); ++i)
                {
                    if( !(fields[i]->read( val ))) serr<<"could not read value for option "<<key<<": "<<val << sendl;
                }
            }
            else
            {
                if ((key!="name") && (key!="type")) serr<<"Unknown option: " << key << sendl;
            }
        }
    }
}

/// Parse the given description to assign values to this object's fields and potentially other parameters
void  Base::parse ( BaseObjectDescription* arg )
{
    //this->parseFields ( arg->getAttributeMap() );
    std::vector< std::string > attributeList;
    arg->getAttributeList(attributeList);
    for (unsigned int i=0; i<attributeList.size(); ++i)
    {
        std::vector< BaseData* > dataModif = findGlobalField(attributeList[i]);
        for (unsigned int d=0; d<dataModif.size(); ++d)
        {
            const char* val = arg->getAttribute(attributeList[i]);
            if (val)
            {
                std::string valueString(val);
                if( !(dataModif[d]->read( valueString ))) serr<<"could not read value for option "<< attributeList[i] <<": " << val << sendl;
            }
        }
    }
}

void  Base::writeDatas ( std::map<std::string,std::string*>& args )
{
//     for( std::map<string,BaseData*>::const_iterator a=m_fieldMap.begin(), aend=m_fieldMap.end(); a!=aend; ++a ) {
    for (unsigned int i=0; i<m_fieldVec.size(); i++)
    {
        string valueString;
        BaseData* field = m_fieldVec[i].second;

        if( args[m_fieldVec[i].first] != NULL )
            *args[ m_fieldVec[i].first] = field->getValueString();
        else
            args[ m_fieldVec[i].first] =  new string(field->getValueString());
    }
}


void Base::xmlWriteNodeDatas (std::ostream& out, unsigned /*level*/ )
{
    for (unsigned int i=0; i<m_fieldVec.size(); i++)
    {
        BaseData* field = m_fieldVec[ i ].second;
        if(  field->isPersistent() && field->isSet() && !field->getValueString().empty())
        {
            out << m_fieldVec[ i ].first << "=\""<< field->getValueString() << "\" ";
        }
    }
}
void  Base::xmlWriteDatas ( std::ostream& out, unsigned level, bool compact )
{
//     for( std::map<string,BaseData*>::const_iterator a=m_fieldMap.begin(), aend=m_fieldMap.end(); a!=aend; ++a ) {
    if (compact)
    {
        for (unsigned int i=0; i<m_fieldVec.size(); i++)
        {
            BaseData* field = m_fieldVec[ i ].second;
            if( field->isPersistent() && field->isSet() && !field->getValueString().empty())
                out << " " << m_fieldVec[ i ].first << "=\""<< field->getValueString() << "\"";
        }
    }
    else
    {
        for (unsigned int i=0; i<m_fieldVec.size(); i++)
        {
            BaseData* field = m_fieldVec[ i ].second;
            if( field->isPersistent() && field->isSet() && !field->getValueString().empty())
            {
                for (unsigned l=0; l<level; l++) out << "\t";
                out << "<Attribute type=\"" << m_fieldVec[ i ].first << "\">\n" ;

                for (unsigned l=0; l<=level; l++) out << "\t";
                out  << "<Data value=\"" << field->getValueString() << "\"/>\n";

                for (unsigned l=0; l<level; l++) out << "\t";
                out << "</Attribute>\n";
            }
        }
    }
}

} // namespace objectmodel

} // namespace core

} // namespace sofa

