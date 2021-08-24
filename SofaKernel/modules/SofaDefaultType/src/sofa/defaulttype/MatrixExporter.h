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
#pragma once

#include <sofa/defaulttype/fwd.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <sofa/helper/OptionsGroup.h>

namespace sofa::defaulttype
{

bool writeMatrixTxt(const std::string& filename, sofa::defaulttype::BaseMatrix* matrix);
bool writeMatrixCsv(const std::string& filename, sofa::defaulttype::BaseMatrix* matrix);

inline std::unordered_map<std::string, std::function<bool(const std::string&, sofa::defaulttype::BaseMatrix*)> > matrixExporterMap =
{
    {"txt", writeMatrixTxt},
    {"csv", writeMatrixCsv},
};

inline sofa::helper::OptionsGroup matrixExporterOptionsGroup(2, "txt", "csv");

}