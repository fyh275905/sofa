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
#ifndef SOFA_COMPONENT_TOPOLOGY_MESHTOPOLOGY_H
#define SOFA_COMPONENT_TOPOLOGY_MESHTOPOLOGY_H

#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <sofa/core/componentmodel/topology/Topology.h>
#include <sofa/helper/fixed_array.h>
#include <sofa/helper/vector.h>

namespace sofa
{

namespace component
{

namespace topology
{

using namespace sofa::defaulttype;
using helper::vector;
using helper::fixed_array;

class MeshTopology : public core::componentmodel::topology::Topology
{
public:

    //typedef int index_type;
    typedef unsigned index_type;

    typedef fixed_array<index_type,2> Line;
    typedef fixed_array<index_type,3> Triangle;
    typedef fixed_array<index_type,4> Quad;
    typedef fixed_array<index_type,4> Tetra;
    typedef fixed_array<index_type,8> Cube;

    typedef vector<Line> SeqLines;
    typedef vector<Triangle> SeqTriangles;
    typedef vector<Quad> SeqQuads;
    typedef vector<Tetra> SeqTetras;
    typedef vector<Cube> SeqCubes;

    MeshTopology();
    //virtual const char* getTypeName() const { return "Mesh"; }

    virtual void clear();

    virtual bool load(const char* filename);

    void parse(core::objectmodel::BaseObjectDescription* arg)
    {
        if (arg->getAttribute("filename"))
        {
            filename.setValue( arg->getAttribute("filename") );
            this->load(arg->getAttribute("filename"));
        }
        arg->removeAttribute("filename");
        this->core::componentmodel::topology::Topology::parse(arg);
    }

    virtual int getNbPoints() const;

    // Complete sequence accessors

    virtual const SeqLines& getLines();
    virtual const SeqTriangles& getTriangles();
    virtual const SeqQuads& getQuads();
    virtual const SeqTetras& getTetras();
    virtual const SeqCubes& getCubes();

    // Random accessors

    virtual int getNbLines();
    virtual int getNbTriangles();
    virtual int getNbQuads();
    virtual int getNbTetras();
    virtual int getNbCubes();

    virtual const Line& getLine(index_type i);
    virtual const Triangle& getTriangle(index_type i);
    virtual const Quad& getQuad(index_type i);
    virtual const Tetra& getTetra(index_type i);
    virtual const Cube& getCube(index_type i);

    /// return true if the given cube is active, i.e. it is not empty
    virtual bool isCubeActive(int /*index*/) { return true; }

    // Points accessors (not always available)

    virtual bool hasPos() const;
    virtual double getPX(int i) const;
    virtual double getPY(int i) const;
    virtual double getPZ(int i) const;

    // for procedural creation without file loader
    void addPoint(double px, double py, double pz);
    void addLine( int a, int b );
    void addTriangle( int a, int b, int c );
    void addTetrahedron( int a, int b, int c, int d );

    // get the current revision of this mesh (use to detect changes)
    int getRevision() const { return revision; }
protected:
    int nbPoints;
    vector< fixed_array<double,3> > seqPoints;

    Data<SeqLines> seqLines;
    bool validLines;

    //SeqTriangles   seqTriangles;
    Data<SeqTriangles> seqTriangles;
    bool         validTriangles;
    SeqQuads       seqQuads;
    bool         validQuads;

    SeqTetras      seqTetras;
    bool         validTetras;
    SeqCubes       seqCubes;
    bool         validCubes;

    int revision;

    Data< std::string > filename;

    void invalidate();

    virtual void updateLines()     { }
    virtual void updateTriangles() { }
    virtual void updateQuads()     { }
    virtual void updateTetras()    { }
    virtual void updateCubes()     { }

    class Loader;
    friend class Loader;
};

} // namespace topology

} // namespace component

} // namespace sofa

#endif
