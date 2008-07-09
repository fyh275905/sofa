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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_TOPOLOGY_GRIDTOPOLOGY_H
#define SOFA_COMPONENT_TOPOLOGY_GRIDTOPOLOGY_H

#include <sofa/component/topology/MeshTopology.h>

namespace sofa
{

namespace component
{

namespace topology
{

using namespace sofa::defaulttype;

class GridTopology : public MeshTopology
{
public:
    GridTopology();

    GridTopology(int nx, int ny, int nz);

    void setSize(int nx, int ny, int nz);

    void parse(core::objectmodel::BaseObjectDescription* arg)
    {
        this->MeshTopology::parse(arg);


        if (arg->getAttribute("nx")!=NULL && arg->getAttribute("ny")!=NULL && arg->getAttribute("nz")!=NULL )
        {
            const char* nx = arg->getAttribute("nx");
            const char* ny = arg->getAttribute("ny");
            const char* nz = arg->getAttribute("nz");
            n.setValue(Vec<3,int>(atoi(nx),atoi(ny),atoi(nz)));
        }

        this->setSize();
    }

    int getNx() const { return n.getValue()[0]; }
    int getNy() const { return n.getValue()[1]; }
    int getNz() const { return n.getValue()[2]; }

    void setNx(int n_) { (*n.beginEdit())[0] = n_; setSize(); }
    void setNy(int n_) { (*n.beginEdit())[1] = n_; setSize(); }
    void setNz(int n_) { (*n.beginEdit())[2] = n_; setSize(); }

    //int getNbPoints() const { return n.getValue()[0]*n.getValue()[1]*n.getValue()[2]; }

    virtual int getNbHexas() { return (n.getValue()[0]-1)*(n.getValue()[1]-1)*(n.getValue()[2]-1); }

    int getNbQuads()
    {
        if (n.getValue()[2] == 1)
            return (n.getValue()[0]-1)*(n.getValue()[1]-1);
        else if (n.getValue()[1] == 1)
            return (n.getValue()[0]-1)*(n.getValue()[2]-1);
        else
            return (n.getValue()[1]-1)*(n.getValue()[2]-1);
    }

    Hexa getHexaCopy(int i);
    Hexa getHexa(int x, int y, int z);

#ifndef SOFA_NEW_HEXA
    Cube getCubeCopy(int i) { return getHexaCopy(i); }
    Cube getCube(int x, int y, int z) { return getHexa(x,y,z); }
#endif

    Quad getQuadCopy(int i);
    Quad getQuad(int x, int y, int z);

    int point(int x, int y, int z) const { return x+n.getValue()[0]*(y+n.getValue()[1]*z); }
    int hexa(int x, int y, int z) const { return x+(n.getValue()[0]-1)*(y+(n.getValue()[1]-1)*z); }
    int cube(int x, int y, int z) const { return hexa(x,y,z); }

protected:
    Data< Vec<3, int> > n;

    virtual void setSize();
    virtual void updateEdges();
    virtual void updateQuads();
    virtual void updateHexas();
};

} // namespace topology

} // namespace component

} // namespace sofa

#endif
