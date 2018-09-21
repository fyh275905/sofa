/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2018 INRIA, USTL, UJF, CNRS, MGH                    *
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
#ifndef SOFA_HELPER_STRING_UTILS_H
#define SOFA_HELPER_STRING_UTILS_H

#include <string>
#include <cstring>
#include "init.h"
namespace sofa
{

namespace helper
{

///@brief returns a copy of the string given in argument.
SOFA_HELPER_API char* getAStringCopy(const char *c);

///@brief replace all occurence of "search" by the "replace" string.
SOFA_HELPER_API void replaceAll(std::string& str,
                                const std::string& search,
                                const std::string& replace);

///@brief returns true if the prefix if located at the beginning of the "full" string.
SOFA_HELPER_API bool starts_with(const std::string& prefix, const std::string& full);

///@brief returns true if the suffix if located at the end of the "full" string.
SOFA_HELPER_API bool ends_with(const std::string& suffix, const std::string& full);

} // namespace helper

} // namespace sofa

#endif //SOFA_HELPER_STRING_UTILS_H
