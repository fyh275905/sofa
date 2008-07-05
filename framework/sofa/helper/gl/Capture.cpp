/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#include <sofa/helper/gl/Capture.h>
#include <sofa/helper/io/ImageBMP.h>
#ifdef SOFA_HAVE_PNG
#include <sofa/helper/io/ImagePNG.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

namespace sofa
{

namespace helper
{

namespace gl
{

Capture::Capture()
    : prefix("capture"), counter(-1)
{
}

bool Capture::saveScreen(const std::string& filename, int compression_level)
{
#ifdef SOFA_HAVE_PNG
    io::ImagePNG img;
#else
    io::ImageBMP img;
#endif
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    img.init(viewport[2],viewport[3],24);
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGB, GL_UNSIGNED_BYTE, img.getData());

#ifdef SOFA_HAVE_PNG
    if (compression_level != -1)
    {
        if (!img.save(filename, compression_level)) return false;
    }
    else
    {
        if (!img.save(filename)) return false;
    }
#else
    if (!img.save(filename)) return false;
#endif
    std::cout << "Saved "<<img.getWidth()<<"x"<<img.getHeight()<<" screen image to "<<filename<<std::endl;
    glReadBuffer(GL_BACK);
    return true;
}

std::string Capture::findFilename()
{
    std::string filename;
    char buf[32];
    int c;
    c = 0;
    struct stat st;
    do
    {
        ++c;
        sprintf(buf, "%04d",c);
        filename = prefix;
        filename += buf;
#ifdef SOFA_HAVE_PNG
        filename += ".png";
#else
        filename += ".bmp";
#endif
    }
    while (stat(filename.c_str(),&st)==0);
    counter = c+1;

    sprintf(buf, "%04d",c);
    filename = prefix;
    filename += buf;
#ifdef SOFA_HAVE_PNG
    filename += ".png";
#else
    filename += ".bmp";
#endif

    return filename;
}


bool Capture::saveScreen(int compression_level)
{
    return saveScreen(findFilename(), compression_level);
}

} // namespace gl

} // namespace helper

} // namespace sofa

