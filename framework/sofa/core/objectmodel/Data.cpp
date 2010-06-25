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
#define SOFA_CORE_OBJECTMODEL_DATA_CPP

#include <sofa/core/objectmodel/Data.h>
#include <string>

namespace sofa
{

namespace core
{

namespace objectmodel
{

/// Specialization for reading strings
template<>
inline
bool SOFA_CORE_API TData<std::string>::read( std::string& str )
{
    virtualSetValue(str);
    return true;
}

/// Specialization for reading booleans
template<>
inline
bool SOFA_CORE_API TData<bool>::read( std::string& str )
{
    if (str.empty())
        return false;
    bool val;
    if (str[0] == 'T' || str[0] == 't')
        val = true;
    else if (str[0] == 'F' || str[0] == 'f')
        val = false;
    else if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-')
        val = (atoi(str.c_str()) != 0);
    else
        return false;
    virtualSetValue(val);
    return true;
}

template class SOFA_CORE_API TData< std::string >;
template class SOFA_CORE_API Data< std::string >;
template class SOFA_CORE_API TData< bool >;
template class SOFA_CORE_API Data< bool >;

} // objectmodel

} // core

} // sofa


