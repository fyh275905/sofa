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
#include <sofa/helper/io/MeshVTK.h>

#include <sstream>

#define TIXML_USE_STL
#include <tinyxml/tinyxml.h>

#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/SetDirectory.h>

namespace sofa
{

namespace helper
{

namespace io
{

using namespace sofa::defaulttype;
using namespace sofa::core::loader;

SOFA_DECL_CLASS(MeshVTK)

Creator<Mesh::FactoryMesh,MeshVTK> MeshVTKClass("vtu");

void MeshVTK::init(std::string filename)
{
    if (!sofa::helper::system::DataRepository.findFile(filename))
    {
        std::cerr << "File " << filename << " not found " << std::endl;
        return;
    }
    loaderType = "vtu";
    readVTU(filename);
}

void MeshVTK::readVTU(const std::string &filename)
{

    tinyxml::TiXmlDocument vtu;
    vtu.LoadFile(filename);
    if (vtu.Error())
    {
        std::cerr << "Error while loading file " << filename << std::endl;
        std::cerr << vtu.ErrorDesc() << std::endl;
        return;
    }
    tinyxml::TiXmlElement* piece = vtu.FirstChildElement("VTKFile")->FirstChildElement("UnstructuredGrid")->FirstChildElement("Piece");

    unsigned int nbPoints, nbCells;
    piece->QueryUnsignedAttribute("NumberOfPoints", &nbPoints);
    piece->QueryUnsignedAttribute("NumberOfCells", &nbCells);

    // read vertices
    Vec3d vertex;
    std::stringstream pointsDataArray(piece->FirstChildElement("Points")->FirstChildElement("DataArray")->GetText());
    for (std::size_t i = 0 ; i < nbPoints ; ++i)
    {
        pointsDataArray >> vertex[0] >> vertex[1] >> vertex[2];
        vertices.push_back(vertex);
    }


    // read triangles
    std::stringstream connectivityDataArray;
    std::stringstream typesDataArray;
    tinyxml::TiXmlElement* cellDataArray = piece->FirstChildElement("Cells")->FirstChildElement("DataArray");
    std::string name;
    while(cellDataArray)
    {
        cellDataArray->QueryStringAttribute("Name", &name);
        if (name.compare("connectivity")==0)
            connectivityDataArray << cellDataArray->GetText();
        if (name.compare("types")==0)
            typesDataArray << cellDataArray->GetText();
        cellDataArray = cellDataArray->NextSiblingElement();
    }

    int cellType;
    vector< vector<int> > vertNormTexIndices;
    vector<int> vIndices, nIndices, tIndices;
    for (std::size_t i = 0 ; i < nbCells ; ++i)
    {
        vIndices.clear();
        vertNormTexIndices.clear();
        typesDataArray >> cellType;
        switch(cellType) {
        case 5:
            vIndices.resize(3);
            connectivityDataArray >> vIndices[0]  >> vIndices[1] >> vIndices[2];
            break;
        default:
            std::cerr << "Unsupported cell type" << std::endl;
            return;
        }
        vertNormTexIndices.push_back (vIndices);
        vertNormTexIndices.push_back (nIndices);
        vertNormTexIndices.push_back (tIndices);
        facets.push_back(vertNormTexIndices);
    }
}

} // namespace io

} // namespace helper

} // namespace sofa

