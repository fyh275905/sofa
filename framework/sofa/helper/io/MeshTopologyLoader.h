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
#ifndef SOFA_HELPER_IO_MESHTOPOLOGYLOADER_H
#define SOFA_HELPER_IO_MESHTOPOLOGYLOADER_H

#include <stdio.h>

namespace sofa
{

namespace helper
{

namespace io
{

class MeshTopologyLoader
{
public:
    virtual ~MeshTopologyLoader() {}
    bool load(const char *filename);
    virtual void setNbPoints(int /*n*/) {}
    virtual void setNbLines(int /*n*/) {}
    virtual void setNbEdges(int /*n*/) {}
    virtual void setNbTriangles(int /*n*/) {}
    virtual void setNbQuads(int /*n*/) {}
    virtual void setNbTetras(int /*n*/) {}
    virtual void setNbCubes(int /*n*/) {}
    virtual void addPoint(double /*px*/, double /*py*/, double /*pz*/) {}
    virtual void addLine(int /*p1*/, int /*p2*/) {}
    virtual void addTriangle(int /*p1*/, int /*p2*/, int /*p3*/) {}
    virtual void addQuad(int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/) {}
    virtual void addTetra(int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/) {}
    virtual void addCube(int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/) {}
private:
    bool loadObj(const char *filename);
    bool loadMesh(const char *filename);

    bool loadGmsh(FILE *, const int);
    bool loadXsp(FILE *, bool);
    bool loadMeshTopology(FILE *);
};

} // namespace io

} // namespace helper

} // namespace sofa

#endif
