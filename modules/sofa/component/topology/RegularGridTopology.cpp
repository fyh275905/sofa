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
#include <sofa/component/topology/RegularGridTopology.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace topology
{

using namespace sofa::defaulttype;
using std::cout;
using std::endl;

void RegularGridTopology::parse(core::objectmodel::BaseObjectDescription* arg)
{
    float scale=1.0f;
    if (arg->getAttribute("scale")!=NULL)
    {
        scale = (float)atof(arg->getAttribute("scale"));
    }

    this->GridTopology::parse(arg);
    if (arg->getAttribute("xmin") != NULL &&
        arg->getAttribute("ymin") != NULL &&
        arg->getAttribute("zmin") != NULL &&
        arg->getAttribute("xmax") != NULL &&
        arg->getAttribute("ymax") != NULL &&
        arg->getAttribute("zmax") != NULL )
    {
        const char* xmin = arg->getAttribute("xmin");
        const char* ymin = arg->getAttribute("ymin");
        const char* zmin = arg->getAttribute("zmin");
        const char* xmax = arg->getAttribute("xmax");
        const char* ymax = arg->getAttribute("ymax");
        const char* zmax = arg->getAttribute("zmax");
        min.setValue(Vector3(atof(xmin)*scale,atof(ymin)*scale,atof(zmin)*scale));
        max.setValue(Vector3(atof(xmax)*scale,atof(ymax)*scale,atof(zmax)*scale));
    }
    this->setPos(min.getValue()[0],max.getValue()[0],min.getValue()[1],max.getValue()[1],min.getValue()[2],max.getValue()[2]);

}

SOFA_DECL_CLASS(RegularGridTopology)

int RegularGridTopologyClass = core::RegisterObject("Regular grid in 3D")
        .addAlias("RegularGrid")
        .add< RegularGridTopology >()
        ;

RegularGridTopology::RegularGridTopology(int nx, int ny, int nz)
    : GridTopology(nx, ny, nz),
      min(initData(&min,Vector3(0.0f,0.0f,0.0f),"min", "Min")),
      max(initData(&max,Vector3(1.0f,1.0f,1.0f),"max", "Max"))
{
}

RegularGridTopology::RegularGridTopology()
    : min(initData(&min,Vector3(0.0f,0.0f,0.0f),"min", "Min")),
      max(initData(&max,Vector3(1.0f,1.0f,1.0f),"max", "Max"))
{
}

void RegularGridTopology::setPos(SReal xmin, SReal xmax, SReal ymin, SReal ymax, SReal zmin, SReal zmax)
{
    min.setValue(Vector3(xmin,ymin,zmin));
    max.setValue(Vector3(xmax,ymax,zmax));
    setP0(Vector3(xmin,ymin,zmin));
    if (n.getValue()[0]>1)
        setDx(Vector3((xmax-xmin)/(n.getValue()[0]-1),0,0));
    else
        setDx(Vector3(0,0,0));
    if (n.getValue()[1]>1)
        setDy(Vector3(0,(ymax-ymin)/(n.getValue()[1]-1),0));
    else
        setDy(Vector3(0,0,0));
    if (n.getValue()[2]>1)
        setDz(Vector3(0,0,(zmax-zmin)/(n.getValue()[2]-1)));
    else
        setDz(Vector3(0,0,0));
}

unsigned RegularGridTopology::getIndex( int i, int j, int k ) const
{
    return n.getValue()[0]* ( n.getValue()[1]*k + j ) + i;
}


Vector3 RegularGridTopology::getPoint(int i) const
{
    int x = i%n.getValue()[0]; i/=n.getValue()[0];
    int y = i%n.getValue()[1]; i/=n.getValue()[1];
    int z = i;
    return getPoint(x,y,z);
}

Vector3 RegularGridTopology::getPoint(int x, int y, int z) const
{
    return p0+dx*x+dy*y+dz*z;
}

/// return the cube containing the given point (or -1 if not found).
int RegularGridTopology::findCube(const Vector3& pos)
{
    if (n.getValue()[0]<2 || n.getValue()[1]<2 || n.getValue()[2]<2)
        return -1;

    Vector3 p = pos-p0;
    SReal x = p*dx*inv_dx2;
    SReal y = p*dy*inv_dy2;
    SReal z = p*dz*inv_dz2;
    int ix = int(x+1000000)-1000000; // Do not round toward 0...
    int iy = int(y+1000000)-1000000;
    int iz = int(z+1000000)-1000000;
    if (   (unsigned)ix <= (unsigned)n.getValue()[0]-2
            && (unsigned)iy <= (unsigned)n.getValue()[1]-2
            && (unsigned)iz <= (unsigned)n.getValue()[2]-2 )
    {
        return cube(ix,iy,iz);
    }
    else
    {
        return -1;
    }
}

/// return the nearest cube (or -1 if not found).
int RegularGridTopology::findNearestCube(const Vector3& pos)
{
    if (n.getValue()[0]<2 || n.getValue()[1]<2 || n.getValue()[2]<2) return -1;
    Vector3 p = pos-p0;
    SReal x = p*dx*inv_dx2;
    SReal y = p*dy*inv_dy2;
    SReal z = p*dz*inv_dz2;
    int ix = int(x+1000000)-1000000; // Do not round toward 0...
    int iy = int(y+1000000)-1000000;
    int iz = int(z+1000000)-1000000;
    if (ix<0) ix=0; else if (ix>n.getValue()[0]-2) ix=n.getValue()[0]-2;
    if (iy<0) iy=0; else if (iy>n.getValue()[1]-2) iy=n.getValue()[1]-2;
    if (iz<0) iz=0; else if (iz>n.getValue()[2]-2) iz=n.getValue()[2]-2;
    return cube(ix,iy,iz);
}

/// return the cube containing the given point (or -1 if not found),
/// as well as deplacements from its first corner in terms of dx, dy, dz (i.e. barycentric coordinates).
int RegularGridTopology::findCube(const Vector3& pos, SReal& fx, SReal &fy, SReal &fz)
{
    if (n.getValue()[0]<2 || n.getValue()[1]<2 || n.getValue()[2]<2) return -1;
    Vector3 p = pos-p0;
    SReal x = p*dx*inv_dx2;
    SReal y = p*dy*inv_dy2;
    SReal z = p*dz*inv_dz2;
    int ix = int(x+1000000)-1000000; // Do not round toward 0...
    int iy = int(y+1000000)-1000000;
    int iz = int(z+1000000)-1000000;
    if ((unsigned)ix<=(unsigned)n.getValue()[0]-2 && (unsigned)iy<=(unsigned)n.getValue()[1]-2 && (unsigned)iz<=(unsigned)n.getValue()[2]-2)
    {
        fx = x-ix;
        fy = y-iy;
        fz = z-iz;
        return cube(ix,iy,iz);
    }
    else
    {
        return -1;
    }
}

/// return the cube containing the given point (or -1 if not found),
/// as well as deplacements from its first corner in terms of dx, dy, dz (i.e. barycentric coordinates).
int RegularGridTopology::findNearestCube(const Vector3& pos, SReal& fx, SReal &fy, SReal &fz)
{
    if (n.getValue()[0]<2 || n.getValue()[1]<2 || n.getValue()[2]<2) return -1;
    Vector3 p = pos-p0;
    SReal x = p*dx*inv_dx2;
    SReal y = p*dy*inv_dy2;
    SReal z = p*dz*inv_dz2;
    int ix = int(x+1000000)-1000000; // Do not round toward 0...
    int iy = int(y+1000000)-1000000;
    int iz = int(z+1000000)-1000000;
    if (ix<0) ix=0; else if (ix>n.getValue()[0]-2) ix=n.getValue()[0]-2;
    if (iy<0) iy=0; else if (iy>n.getValue()[1]-2) iy=n.getValue()[1]-2;
    if (iz<0) iz=0; else if (iz>n.getValue()[2]-2) iz=n.getValue()[2]-2;
    fx = x-ix;
    fy = y-iy;
    fz = z-iz;
    return cube(ix,iy,iz);
}


unsigned RegularGridTopology::getCubeIndex( int i, int j, int k ) const
{
    return (n.getValue()[0]-1)* ( (n.getValue()[1]-1)*k + j ) + i;
}

Vector3 RegularGridTopology::getCubeCoordinate(int i) const
{
    Vector3 result;
    result[0] = i%(n.getValue()[0]-1); i/=(n.getValue()[0]-1);
    result[1] = i%(n.getValue()[1]-1); i/=(n.getValue()[1]-1);
    result[2] = i;
    return result;
}


} // namespace topology

} // namespace component

} // namespace sofa

