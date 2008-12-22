/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#include <sofa/helper/io/ImageRAW.h>
#include <sofa/helper/system/FileRepository.h>
#include <iostream>

namespace sofa
{

namespace helper
{

namespace io
{

ImageRAW::ImageRAW ()
    : Image(),
      depth(1),
      headerSize(0)
{}

void ImageRAW::init(int w, int h, int d, int nbb, int hsize)
{
    clear();
    width = w;
    height = h;
    depth = d;
    headerSize = hsize;
    nbBits = nbb;
    data = (unsigned char*) malloc(getDataSize());
    header = (unsigned char*) malloc(headerSize);
}

bool ImageRAW::load(std::string filename)
{
    if (!sofa::helper::system::DataRepository.findFile(filename))
    {
        std::cerr << "File " << filename << " not found " << std::endl;
        return false;
    }
    FILE *file;
    /* make sure the file is there and open it read-only (binary) */
    if ((file = fopen(filename.c_str(), "rb")) == NULL)
    {
        std::cerr << "File not found : " << filename << std::endl;
        return false;
    }

    // read header and ignore it as we don't know how to interpret it
    for ( int i=0; i<headerSize; ++i )
    {
        int c = getc ( file );

        if ( c == EOF )
        {
            fclose ( file );
            return false;
        }
        else
            header[i] = ( unsigned char ) c;
    }

    const unsigned int numVoxels = getDataSize();

    // get the voxels from the file
    for ( unsigned int i=0; i<numVoxels; ++i )
    {
        int c = getc ( file );

        if ( c == EOF )
        {
            fclose ( file );
            return false;
        }
        else
            data[i] = ( unsigned char ) c;
    }


    fclose(file);

    return true;
}

bool ImageRAW::save(std::string filename, int)
{
    FILE *file;
    std::cout << "Writing RAW file " << filename << std::endl;
    /* make sure the file is there and open it read-only (binary) */
    if ((file = fopen(filename.c_str(), "wb")) == NULL)
    {
        std::cerr << "File write access failed : " << filename << std::endl;
        return false;
    }

    if(headerSize > 0)
        if(!fwrite(header, headerSize, 1, file)) return false;

    if (!fwrite(data, getDataSize(), 1, file)) return false;

    fclose(file);
    return true;
}

} // namespace io

} // namespace helper

} // namespace sofa

