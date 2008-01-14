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
#ifndef SOFA_COMPONENT_TOPOLOGY_CUBETOPOLOGY_H
#define SOFA_COMPONENT_TOPOLOGY_CUBETOPOLOGY_H

#include <sofa/component/topology/MeshTopology.h>
#include <sofa/defaulttype/Vec.h>

namespace sofa
{

namespace component
{

namespace topology
{

using namespace sofa::defaulttype;

class CubeTopology : public MeshTopology
{
public:
    typedef Vec3d Vec3;

    CubeTopology(int nx, int ny, int nz);
    CubeTopology();

    void setSize(int nx, int ny, int nz);

    void parse(core::objectmodel::BaseObjectDescription* arg);

    int getNx() const { return nx.getValue(); }
    int getNy() const { return ny.getValue(); }
    int getNz() const { return nz.getValue(); }

    void setNx(int n) { nx.setValue(n); setSize(); }
    void setNy(int n) { ny.setValue(n); setSize(); }
    void setNz(int n) { nz.setValue(n); setSize(); }

    virtual void init();
    virtual void reinit();

    //int getNbQuads();
    //Quad getQuad(int i);
    //Quad getQuad(int x, int y, int z);

    enum Plane { PLANE_UNKNOWN=0,
            PLANE_X0,
            PLANE_X1,
            PLANE_Y0,
            PLANE_Y1,
            PLANE_Z0,
            PLANE_Z1
               };

    int point(int x, int y, int z, Plane p = PLANE_UNKNOWN) const; // { return x+nx.getValue()*(y+ny.getValue()*z); }

    void setP0(const Vec3& val) { p0 = val; }
    void setDx(const Vec3& val) { dx = val; inv_dx2 = 1/(dx*dx); }
    void setDy(const Vec3& val) { dy = val; inv_dy2 = 1/(dy*dy); }
    void setDz(const Vec3& val) { dz = val; inv_dz2 = 1/(dz*dz); }

    void setPos(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);

    const Vec3& getP0() const { return p0; }
    const Vec3& getDx() const { return dx; }
    const Vec3& getDy() const { return dy; }
    const Vec3& getDz() const { return dz; }

    Vec3 getPoint(int i) const;
    Vec3 getPoint(int x, int y, int z) const;
    bool hasPos()  const { return true; }
    double getPX(int i)  const { return getPoint(i)[0]; }
    double getPY(int i) const { return getPoint(i)[1]; }
    double getPZ(int i) const { return getPoint(i)[2]; }

    void setSplitNormals(bool b) {splitNormals.setValue(b);}

protected:
    Data<int> nx;
    Data<int> ny;
    Data<int> nz;
    Data<bool> internalPoints;
    Data<bool> splitNormals;

    /// Position of point 0
    Vec3 p0;
    /// Distance between points in the grid. Must be perpendicular to each other
    Vec3 dx,dy,dz;
    double inv_dx2, inv_dy2, inv_dz2;

    virtual void setSize();
    void updateLines();
    void updateQuads();
    //void updateCubes();
};

} // namespace topology

} // namespace component

} // namespace sofa

#endif
