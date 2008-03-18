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
#include <iostream>
#include <sofa/helper/io/Mesh.h>
#include <sofa/component/topology/MeshTopology.h>
#include <sofa/helper/io/MeshTopologyLoader.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/helper/fixed_array.h>
#include <set>


namespace sofa
{

namespace component
{

namespace topology
{

using namespace sofa::defaulttype;

SOFA_DECL_CLASS(MeshTopology)

int MeshTopologyClass = core::RegisterObject("Generic mesh topology")
        .addAlias("Mesh")
        .add< MeshTopology >()
        ;

MeshTopology::MeshTopology()
    : nbPoints(0)
    , seqEdges(initData(&seqEdges,"lines","List of line indices")), validEdges(false)
    , seqTriangles(initData(&seqTriangles,"triangles","List of triangle indices")), validTriangles(false)
    , validQuads(false), validTetras(false), validHexas(false), revision(0)
    , filename(initData(&filename,"filename","Filename of the object"))
{
}


class MeshTopology::Loader : public helper::io::MeshTopologyLoader
{
public:
    MeshTopology* dest;
    Loader(MeshTopology* dest) : dest(dest) {}
    virtual void addPoint(double px, double py, double pz)
    {
        dest->seqPoints.push_back(helper::make_array(px, py, pz));
        if (dest->seqPoints.size() > (unsigned)dest->nbPoints)
            dest->nbPoints = dest->seqPoints.size();
    }
    virtual void addEdge(int p1, int p2)
    {
        dest->seqEdges.beginEdit()->push_back(Edge(p1,p2));
        dest->seqEdges.endEdit();
    }
    virtual void addTriangle(int p1, int p2, int p3)
    {
        dest->seqTriangles.beginEdit()->push_back(Triangle(p1,p2,p3));
        dest->seqTriangles.endEdit();
    }
    virtual void addQuad(int p1, int p2, int p3, int p4)
    {
        dest->seqQuads.push_back(Quad(p1,p2,p3,p4));
    }
    virtual void addTetra(int p1, int p2, int p3, int p4)
    {
        dest->seqTetras.push_back(Tetra(p1,p2,p3,p4));
    }
    virtual void addCube(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
    {
        //dest->seqHexas.push_back(Hexa(p1,p2,p3,p4,p5,p6,p7,p8));
        dest->seqHexas.push_back(Hexa(p1,p2,p4,p3,p5,p6,p8,p7));
    }
};

void MeshTopology::clear()
{
    nbPoints = 0;
    seqEdges.beginEdit()->clear(); seqEdges.endEdit();
    seqTriangles.beginEdit()->clear(); seqTriangles.endEdit();
    seqQuads.clear();
    seqTetras.clear();
    seqHexas.clear();
    invalidate();
}

bool MeshTopology::load(const char* filename)
{
    clear();
    Loader loader(this);

    if ((strlen(filename)>4 && !strcmp(filename+strlen(filename)-4,".obj"))
        || (strlen(filename)>6 && !strcmp(filename+strlen(filename)-6,".trian")))
    {
        helper::io::Mesh* mesh = helper::io::Mesh::Create(filename);
        if (mesh==NULL) return false;

        loader.setNbPoints(mesh->getVertices().size());
        for (unsigned int i=0; i<mesh->getVertices().size(); i++)
        {
            loader.addPoint(mesh->getVertices()[i][0],mesh->getVertices()[i][1],mesh->getVertices()[i][2]);
        }

        std::set< std::pair<int,int> > edges;

        const vector< vector < vector <int> > > & facets = mesh->getFacets();
        for (unsigned int i=0; i<facets.size(); i++)
        {
            const vector<int>& facet = facets[i][0];
            if (facet.size()==2)
            {
                // Line
                if (facet[0]<facet[1])
                    loader.addEdge(facet[0],facet[1]);
                else
                    loader.addEdge(facet[1],facet[0]);
            }
            else if (facet.size()==4)
            {
                // Quat
                loader.addQuad(facet[0],facet[1],facet[2],facet[3]);
            }
            else
            {
                // Triangularize
                for (unsigned int j=2; j<facet.size(); j++)
                    loader.addTriangle(facet[0],facet[j-1],facet[j]);
            }
            // Add edges
            if (facet.size()>2)
                for (unsigned int j=0; j<facet.size(); j++)
                {
                    int i1 = facet[j];
                    int i2 = facet[(j+1)%facet.size()];
                    if (edges.count(std::make_pair(i1,i2))!=0)
                    {
                        /*
                        std::cerr << "ERROR: Duplicate edge.\n";*/
                    }
                    else if (edges.count(std::make_pair(i2,i1))==0)
                    {
                        if (i1>i2)
                            loader.addEdge(i1,i2);
                        else
                            loader.addEdge(i2,i1);
                        edges.insert(std::make_pair(i1,i2));
                    }
                }
        }
        delete mesh;
    }
    else
    {
        if (!loader.load(filename))
            return false;
    }
    return true;
}


void MeshTopology::addPoint(double px, double py, double pz)
{
    seqPoints.push_back(helper::make_array(px, py, pz));
    if (seqPoints.size() > (unsigned)nbPoints)
        nbPoints = seqPoints.size();

}

void MeshTopology::addEdge( int a, int b )
{
    seqEdges.beginEdit()->push_back(Edge(a,b));
    seqEdges.endEdit();
}

void MeshTopology::addTriangle( int a, int b, int c )
{
    seqTriangles.beginEdit()->push_back( Triangle(a,b,c) );
    seqTriangles.endEdit();
}

void MeshTopology::addTetrahedron( int a, int b, int c, int d )
{
    seqTetras.push_back( Tetra(a,b,c,d) );
}

const MeshTopology::SeqEdges& MeshTopology::getEdges()
{
    if (!validEdges)
    {
        updateEdges();
        validEdges = true;
    }
    return seqEdges.getValue();
}

const MeshTopology::SeqTriangles& MeshTopology::getTriangles()
{
    if (!validTriangles)
    {
        updateTriangles();
        validTriangles = true;
    }
    return seqTriangles.getValue();
}

const MeshTopology::SeqQuads& MeshTopology::getQuads()
{
    if (!validQuads)
    {
        updateQuads();
        validQuads = true;
    }
    return seqQuads;
}

const MeshTopology::SeqTetras& MeshTopology::getTetras()
{
    if (!validTetras)
    {
        updateTetras();
        validTetras = true;
    }
    return seqTetras;
}

const MeshTopology::SeqHexas& MeshTopology::getHexas()
{
    if (!validHexas)
    {
        updateHexas();
        validHexas = true;
    }
    return seqHexas;
}

int MeshTopology::getNbPoints() const
{
    return nbPoints;
}

int MeshTopology::getNbEdges()
{
    return getEdges().size();
}

int MeshTopology::getNbTriangles()
{
    return getTriangles().size();
}

int MeshTopology::getNbQuads()
{
    return getQuads().size();
}

int MeshTopology::getNbTetras()
{
    return getTetras().size();
}

int MeshTopology::getNbHexas()
{
    return getHexas().size();
}

MeshTopology::Edge MeshTopology::getEdge(index_type i)
{
    return getEdges()[i];
}

MeshTopology::Triangle MeshTopology::getTriangle(index_type i)
{
    return getTriangles()[i];
}

MeshTopology::Quad MeshTopology::getQuad(index_type i)
{
    return getQuads()[i];
}

MeshTopology::Tetra MeshTopology::getTetra(index_type i)
{
    return getTetras()[i];
}

MeshTopology::Hexa MeshTopology::getHexa(index_type i)
{
    return getHexas()[i];
}

bool MeshTopology::hasPos() const
{
    return !seqPoints.empty();
}

double MeshTopology::getPX(int i) const
{
    return ((unsigned)i<seqPoints.size()?seqPoints[i][0]:0.0);
}

double MeshTopology::getPY(int i) const
{
    return ((unsigned)i<seqPoints.size()?seqPoints[i][1]:0.0);
}

double MeshTopology::getPZ(int i) const
{
    return ((unsigned)i<seqPoints.size()?seqPoints[i][2]:0.0);
}

void MeshTopology::invalidate()
{
    validEdges = false;
    validTriangles = false;
    validQuads = false;
    validTetras = false;
    validHexas = false;
    ++revision;
    //std::cout << "MeshTopology::invalidate()"<<std::endl;
}

} // namespace topology

} // namespace component

} // namespace sofa

