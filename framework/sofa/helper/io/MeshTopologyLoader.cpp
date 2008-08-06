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
#include <sofa/helper/io/MeshTopologyLoader.h>
#include <sofa/helper/io/Mesh.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/defaulttype/Vec.h>

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

#include <set>

namespace sofa
{

namespace helper
{

namespace io
{

using namespace sofa::defaulttype;

static void skipToEOL(FILE* f)
{
    int	ch;
    while ((ch = fgetc(f)) != EOF && ch != '\n') ;
}

static bool readLine(char* buf, int size, FILE* f)
{
    buf[0] = '\0';
    if (fgets(buf, size, f) == NULL)
        return false;
    if ((int)strlen(buf)==size-1 && buf[size-1] != '\n')
        skipToEOL(f);
    return true;
}

bool MeshTopologyLoader::loadObj(const char *filename)
{
    helper::io::Mesh* mesh = helper::io::Mesh::Create(filename);
    if (mesh==NULL)
        return false;

    setNbPoints(mesh->getVertices().size());
    for (unsigned int i=0; i<mesh->getVertices().size(); i++)
    {
        addPoint((double)mesh->getVertices()[i][0],
                (double)mesh->getVertices()[i][1],
                (double)mesh->getVertices()[i][2]);
    }

    const vector< vector < vector <int> > > & facets = mesh->getFacets();
    for (unsigned int i=0; i<facets.size(); i++)
    {
        const vector<int>& facet = facets[i][0];
        if (facet.size()==2)
        {
            // Line
            if (facet[0]<facet[1])
                addLine(facet[0],facet[1]);
            else
                addLine(facet[1],facet[0]);
        }
        else if (facet.size()==4)
        {
            // Quat
            addQuad(facet[0],facet[1],facet[2],facet[3]);
        }
        else if (facet.size()==8)
        {
            // Quadralize
            addQuad(facet[0],facet[3],facet[2],facet[1]);
            addQuad(facet[4],facet[5],facet[6],facet[7]);
            addQuad(facet[0],facet[1],facet[5],facet[4]);
            addQuad(facet[1],facet[2],facet[6],facet[5]);
            addQuad(facet[2],facet[3],facet[7],facet[6]);
            addQuad(facet[3],facet[0],facet[4],facet[7]);
        }
        else
        {
            // Triangularize
            for (unsigned int j=2; j<facet.size(); j++)
                addTriangle(facet[0],facet[j-1],facet[j]);
        }

        std::set< std::pair<int,int> > edges;
        // Add edges
        if (facet.size()>2)
        {
            for (unsigned int j=0; j<facet.size(); j++)
            {
                int i1 = facet[j];
                int i2 = facet[(j+1)%facet.size()];
                if (edges.count(std::make_pair(i1,i2))!=0)
                {
                    /*	std::cerr << "ERROR: Duplicate edge.\n";*/
                }
                else if (edges.count(std::make_pair(i2,i1))==0)
                {
                    if (i1>i2)
                        addLine(i1,i2);
                    else
                        addLine(i2,i1);
                    edges.insert(std::make_pair(i1,i2));
                }
            }
        }
    }

    delete mesh;
    return true;
}

bool MeshTopologyLoader::loadGmsh(FILE *file, const int gmshFormat)
{
    char cmd[1024];
    int npoints = 0;
    int nlines = 0;
    int ntris = 0;
    int nquads = 0;
    int ntetras = 0;
    int ncubes = 0;

// 	std::cout << "Loading Gmsh topology '" << filename << "'" << std::endl;
    fscanf(file, "%d\n", &npoints);
    setNbPoints(npoints);
    std::vector<int> pmap;
    for (int i=0; i<npoints; ++i)
    {
        int index = i;
        double x,y,z;
        fscanf(file, "%d %lf %lf %lf\n", &index, &x, &y, &z);
        addPoint(x, y, z);
        if ((int)pmap.size() <= index) pmap.resize(index+1);
        pmap[index] = i;
        //std::cout << "pmap[" << index << "] = " << pmap[index] << std::endl;
    }

    readLine(cmd, sizeof(cmd), file);
    //std::cout << cmd << std::endl;
    if (strncmp(cmd,"$ENDNOD",7) && strncmp(cmd,"$EndNodes",9))
    {
        std::cerr << "'$ENDNOD' or '$EndNodes' expected, found '" << cmd << "'" << std::endl;
        fclose(file);
        return false;
    }

    readLine(cmd, sizeof(cmd), file);
    //std::cout << cmd << std::endl;
    if (strncmp(cmd,"$ELM",4) && strncmp(cmd,"$Elements",9))
    {
        std::cerr << "'$ELM' or '$Elements' expected, found '" << cmd << "'" << std::endl;
        fclose(file);
        return false;
    }

    int nelems = 0;
    fscanf(file, "%d\n", &nelems);
    for (int i=0; i<nelems; ++i)
    {
        int index, etype, rphys, relem, nnodes, ntags, tag;
        if (gmshFormat==1)
        {
            // version 1.0 format is
            // elm-number elm-type reg-phys reg-elem number-of-nodes <node-number-list ...>
            fscanf(file, "%d %d %d %d %d", &index, &etype, &rphys, &relem, &nnodes);
        }
        else if (gmshFormat == 2)
        {
            // version 2.0 format is
            // elm-number elm-type number-of-tags < tag > ... node-number-list
            fscanf(file, "%d %d %d", &index, &etype, &ntags);
            for (int t=0; t<ntags; t++)
                fscanf(file, "%d", &tag); // read the tag but don't use it

            switch (etype)
            {
            case 1: // Line
                nnodes = 2;
                break;
            case 2: // Triangle
                nnodes = 3;
                break;
            case 3: // Quad
                nnodes = 4;
                break;
            case 4: // Tetra
                nnodes = 4;
                break;
            case 5: // Hexa
                nnodes = 8;
                break;
            default:
                std::cerr << "Elements of type 1, 2, 3, 4, 5, or 6 expected. Element of type " << etype << " found." << std::endl;
                //fclose(file);
                //return false;
                nnodes = 0;
            }
        }

        std::vector<int> nodes;
        nodes.resize(nnodes);
        for (int n=0; n<nnodes; ++n)
        {
            int t = 0;
            fscanf(file, "%d",&t);
            nodes[n] = (((unsigned int)t)<pmap.size())?pmap[t]:0;
            //std::cout << "nodes[" << n << "] = " << nodes[n] << std::endl;
        }
        switch (etype)
        {
        case 1: // Line
            addLine(nodes[0], nodes[1]);
            ++nlines;
            break;
        case 2: // Triangle
            addTriangle(nodes[0], nodes[1], nodes[2]);
            //std::cout << "Adding triangle (" << nodes[0] << ", " << nodes[1] << ", " << nodes[2] << ")" << std::endl;
            ++ntris;
            break;
        case 3: // Quad
            addQuad(nodes[0], nodes[1], nodes[2], nodes[3]);
            ++nquads;
            break;
        case 4: // Tetra
            addTetra(nodes[0], nodes[1], nodes[2], nodes[3]);
            ++ntetras;
            break;
        case 5: // Hexa
            addCube(nodes[0], nodes[1], nodes[2], nodes[3],nodes[4], nodes[5], nodes[6], nodes[7]);
            ++ncubes;
            break;
        }
        skipToEOL(file);
    }
    readLine(cmd, sizeof(cmd), file);
    if (strncmp(cmd,"$ENDELM",7) && strncmp(cmd,"$EndElements",12))
    {
        std::cerr << "'$ENDELM' or '$EndElements' expected, found '" << cmd << "'" << std::endl;
        fclose(file);
        return false;
    }
    else
    {
// 			std::cout << "Done parsing Gmsh file." << std::endl;
    }
// 	std::cout << "Loading topology complete:";
// 	if (npoints>0) std::cout << ' ' << npoints << " points";
// 	if (nlines>0)  std::cout << ' ' << nlines  << " lines";
// 	if (ntris>0)   std::cout << ' ' << ntris   << " triangles";
// 	if (nquads>0)  std::cout << ' ' << nquads  << " quads";
// 	if (ntetras>0) std::cout << ' ' << ntetras << " tetrahedra";
// 	if (ncubes>0)  std::cout << ' ' << ncubes  << " cubes";
// 	std::cout << std::endl;

    return true;
}

bool MeshTopologyLoader::loadXsp(FILE *file, bool vector_spring)
{
    char cmd[1024];
    int npoints = 0;
    int nlines = 0;
//	int ntris = 0;
//	int nquads = 0;
//	int ntetras = 0;
//	int ncubes = 0;

    int totalNumMasses;
    int totalNumSprings;

    //		skipToEOL(file);

    // then find out number of masses and springs
    if (fscanf(file, "%s", cmd) != EOF && !strcmp(cmd,"numm"))
    {
        fscanf(file, "%d", &totalNumMasses);
        setNbPoints(totalNumMasses);
        npoints=totalNumMasses;
    }

    if (fscanf(file, "%s", cmd) != EOF && !strcmp(cmd,"nums"))
    {
        fscanf(file, "%d", &totalNumSprings);
        setNbLines(totalNumSprings);
        nlines=totalNumSprings;
        //		setNumSprings(totalNumSprings);
    }

// 		std::cout << "Model contains "<< totalNumMasses <<" masses and "<< totalNumSprings <<" springs"<<std::endl;

    while (fscanf(file, "%s", cmd) != EOF)
    {
        if (!strcmp(cmd,"mass"))
        {
            int index;
            char location;
            double px,py,pz,vx,vy,vz,mass=0.0,elastic=0.0;
            bool fixed=false;
            fscanf(file, "%d %c %lf %lf %lf %lf %lf %lf %lf %lf\n",
                    &index, &location,
                    &px, &py, &pz, &vx, &vy, &vz,
                    &mass, &elastic);

            if (mass < 0)
            {
                // fixed point initialization
                mass = -mass;
                fixed = true;
            }
            addPoint(px,py,pz);
        }
        else if (!strcmp(cmd,"lspg"))	// linear springs connector
        {
            int	index;
            int m1,m2;
            double ks=0.0,kd=0.0,initpos=-1;
            double restx=0.0,resty=0.0,restz=0.0;
            if (vector_spring)
                fscanf(file, "%d %d %d %lf %lf %lf %lf %lf %lf\n",
                        &index,&m1,&m2,&ks,&kd,&initpos, &restx,&resty,&restz);
            else
                fscanf(file, "%d %d %d %lf %lf %lf\n",
                        &index,&m1,&m2,&ks,&kd,&initpos);
            --m1;
            --m2;

            addLine(m1,m2);
        }
        else if (!strcmp(cmd,"grav"))
        {
            double gx,gy,gz;
            fscanf(file, "%lf %lf %lf\n", &gx, &gy, &gz);
        }
        else if (!strcmp(cmd,"visc"))
        {
            double viscosity;
            fscanf(file, "%lf\n", &viscosity);
        }
        else if (!strcmp(cmd,"step"))
        {
            //fscanf(file, "%lf\n", &(MSparams.default_dt));
            skipToEOL(file);
        }
        else if (!strcmp(cmd,"frce"))
        {
            skipToEOL(file);
        }
        else if (cmd[0] == '#')	// it's a comment
        {
            skipToEOL(file);
        }
        else		// it's an unknown keyword
        {
            printf("Unknown MassSpring keyword: %s\n", cmd);
            skipToEOL(file);
            return false;
        }
    }
// 	std::cout << "Loading topology complete:";
// 	if (npoints>0) std::cout << ' ' << npoints << " points";
// 	if (nlines>0)  std::cout << ' ' << nlines  << " lines";
// 	if (ntris>0)   std::cout << ' ' << ntris   << " triangles";
// 	if (nquads>0)  std::cout << ' ' << nquads  << " quads";
// 	if (ntetras>0) std::cout << ' ' << ntetras << " tetrahedra";
// 	if (ncubes>0)  std::cout << ' ' << ncubes  << " cubes";
// 	std::cout << std::endl;

    return true;
}

bool MeshTopologyLoader::loadMesh(FILE *file)
{
    char cmd[1024];
    int npoints = 0;
    int nlines = 0;
    int ntris = 0;
    int nquads = 0;
    int ntetras = 0;
    int ncubes = 0;

// 	std::cout << "Loading mesh topology '" << filename << "'" << std::endl;
    while (fscanf(file, "%s", cmd) != EOF)
    {
        if (!strcmp(cmd,"line"))
        {
            int p1,p2;
            fscanf(file, "%d %d\n",
                    &p1, &p2);
            addLine(p1, p2);
            ++nlines;
        }
        else if (!strcmp(cmd,"triangle"))
        {
            int p1,p2,p3;
            fscanf(file, "%d %d %d\n",
                    &p1, &p2, &p3);
            addTriangle(p1, p2, p3);
            ++ntris;
        }
        else if (!strcmp(cmd,"quad"))
        {
            int p1,p2,p3,p4;
            fscanf(file, "%d %d %d %d\n",
                    &p1, &p2, &p3, &p4);
            addQuad(p1, p2, p3, p4);
            ++nquads;
        }
        else if (!strcmp(cmd,"tetra"))
        {
            int p1,p2,p3,p4;
            fscanf(file, "%d %d %d %d\n",
                    &p1, &p2, &p3, &p4);
            addTetra(p1, p2, p3, p4);
            ++ntetras;
        }
        else if (!strcmp(cmd,"cube"))
        {
            int p1,p2,p3,p4,p5,p6,p7,p8;
            fscanf(file, "%d %d %d %d %d %d %d %d\n",
                    &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8);
            addCube(p1, p2, p3, p4, p5, p6, p7, p8);
            ++ncubes;
        }
        else if (!strcmp(cmd,"point"))
        {
            double px,py,pz;
            fscanf(file, "%lf %lf %lf\n",
                    &px, &py, &pz);
            addPoint(px, py, pz);
            ++npoints;
        }
        else if (!strcmp(cmd,"v"))
        {
            double px,py,pz;
            fscanf(file, "%lf %lf %lf\n",
                    &px, &py, &pz);
            addPoint(px, py, pz);
            ++npoints;
        }
        else if (!strcmp(cmd,"f"))
        {
            int p1,p2,p3,p4=0;
            fscanf(file, "%d %d %d %d\n",
                    &p1, &p2, &p3, &p4);
            if (p4)
            {
                addQuad(p1-1, p2-1, p3-1, p4-1);
                ++nquads;
            }
            else
            {
                addTriangle(p1-1, p2-1, p3-1);
                ++ntris;
            }
        }
        else if (!strcmp(cmd,"mass"))
        {
            int index;
            char location;
            double px,py,pz,vx,vy,vz,mass=0.0,elastic=0.0;
            fscanf(file, "%d %c %lf %lf %lf %lf %lf %lf %lf %lf\n",
                    &index, &location,
                    &px, &py, &pz, &vx, &vy, &vz,
                    &mass, &elastic);
            addPoint(px, py, pz);
            ++npoints;
        }
        else if (!strcmp(cmd,"lspg"))
        {
            int	index;
            int m1,m2;
            double ks=0.0,kd=0.0,initpos=-1;
            fscanf(file, "%d %d %d %lf %lf %lf\n", &index,
                    &m1,&m2,&ks,&kd,&initpos);
            --m1;
            --m2;
            addLine(m1,m2);
            ++nlines;
        }
        else if (cmd[0] == '#')	// it's a comment
        {
            skipToEOL(file);
        }
        else		// it's an unknown keyword
        {
            printf("Unknown Mesh keyword: %s\n", cmd);
            skipToEOL(file);
            return false;
        }
    }
// 	std::cout << "Loading topology complete:";
// 	if (npoints>0) std::cout << ' ' << npoints << " points";
// 	if (nlines>0)  std::cout << ' ' << nlines  << " lines";
// 	if (ntris>0)   std::cout << ' ' << ntris   << " triangles";
// 	if (nquads>0)  std::cout << ' ' << nquads  << " quads";
// 	if (ntetras>0) std::cout << ' ' << ntetras << " tetrahedra";
// 	if (ncubes>0)  std::cout << ' ' << ncubes  << " cubes";
// 	std::cout << std::endl;

    return true;
}

bool MeshTopologyLoader::loadMeshFile(const char *filename)
{
    FILE* file;
    char cmd[1024];
    bool fileLoaded = false;

    if ((file = fopen(filename, "r")) == NULL)
    {
        std::cout << "ERROR: cannot read file '" << filename << "'. Exiting..." << std::endl;
        return false;
    }

    // Check first line
    if (!readLine(cmd, sizeof(cmd), file))
    {
        fclose(file);
        return false;
    }

// 	std::cout << "Loading Gmsh topology '" << filename << "' - ";

    int gmshFormat = 0;
    if (!strncmp(cmd,"$MeshFormat",11)) // Reading gmsh 2.0 file
    {
        gmshFormat = 2;
// 		std::cout << "Gmsh format 2.0" << std::endl;
        readLine(cmd, sizeof(cmd), file); // we don't care about this line
        readLine(cmd, sizeof(cmd), file);
        if (strncmp(cmd,"$EndMeshFormat",14)) // it should end with $EndMeshFormat
        {
            fclose(file);
            return false;
        }
        else
            readLine(cmd, sizeof(cmd), file);
    }
    else
    {
        gmshFormat = 1;
// 		std::cout << "Gmsh format 1.0" << std::endl;
    }

    if (!strncmp(cmd,"$NOD",4) || !strncmp(cmd,"$Nodes",6)) // Gmsh format
    {
        fileLoaded = loadGmsh(file, gmshFormat);
    }
    else if (!strncmp(cmd,"Xsp",3))
    {
        float version = 0.0f;
        sscanf(cmd, "Xsp %f", &version);

        if (version == 3.0)
            fileLoaded = loadXsp(file, false);
        else if (version == 4.0)
            fileLoaded = loadXsp(file, true);
    }
    else
    {
        fileLoaded = loadMesh(file);
    }

    fclose(file);
    return 	fileLoaded;
}

bool MeshTopologyLoader::load(const char *filename)
{
    std::string	fname(filename);
    if (!sofa::helper::system::DataRepository.findFile(fname))
    {
        printf("Cannot find file: %s \n", filename);
        return false;
    }

    bool fileLoaded;

    if ((strlen(filename)>4 && !strcmp(filename+strlen(filename)-4,".obj"))
        || (strlen(filename)>6 && !strcmp(filename+strlen(filename)-6,".trian")))
    {
        fileLoaded = loadObj(fname.c_str());
    }
    else
        fileLoaded = loadMeshFile(fname.c_str());

    if(!fileLoaded)
        printf("Error loading mesh file: %s \n", fname.c_str());

    return fileLoaded;
}

} // namespace io

} // namespace helper

} // namespace sofa

