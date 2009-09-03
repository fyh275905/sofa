/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
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
#include <sofa/helper/io/MeshOBJ.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/SetDirectory.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

namespace sofa
{

namespace helper
{

namespace io
{

using namespace sofa::defaulttype;

SOFA_DECL_CLASS(MeshOBJ)

Creator<Mesh::FactoryMesh,MeshOBJ> MeshOBJClass("obj");

void MeshOBJ::init (std::string filename)
{
    if (!sofa::helper::system::DataRepository.findFile(filename))
    {
        std::cerr << "File " << filename << " not found " << std::endl;
        return;
    }
    FILE *f = fopen(filename.c_str(), "r");
    if (f)
    {
        readOBJ (f,filename.c_str());
        fclose(f);
    }
    else
        std::cerr << "File " << filename << " not found " << std::endl;
}

void MeshOBJ::readOBJ (FILE* file, const char* filename)
{
    std::cout << "MeshOBJ::readOBJ" <<std::endl;

    vector< vector<int> > vertNormTexIndices;
    vector<int>vIndices, nIndices, tIndices;
    int vtn[3];
    char buf[128], matName[1024];
    Vec3d result;
    Vec3d texCoord;
    Vec3d normal;
    const char *token;

    std::string face, tmp;

    while (fscanf(file, "%s", buf) != EOF)
    {
        switch (buf[0])
        {
        case '#':
            /* comment */
            /* eat up rest of line */
            if ( fgets(buf, sizeof(buf), file) == NULL)
            {
                if (feof (file) )
                    std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                else
                    std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
            }

            break;
        case 'v':
            /* v, vn, vt */
            switch (buf[1])
            {
            case '\0':
                /* vertex */
                /* eat up rest of line */
                if ( fgets(buf, sizeof(buf), file) == NULL)
                {
                    if (feof (file) )
                        std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                    else
                        std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
                }
                sscanf(buf, "%lf %lf %lf", &result[0], &result[1], &result[2]);
                vertices.push_back(Vector3(result[0],result[1], result[2]));
                break;
            case 'n':
                /* normal */
                /* eat up rest of line */
                if ( fgets(buf, sizeof(buf), file) == NULL)
                {
                    if (feof (file) )
                        std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                    else
                        std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
                }
                sscanf(buf, "%lf %lf %lf", &result[0], &result[1], &result[2]);
                normals.push_back(Vector3(result[0],result[1], result[2]));
                break;
            case 't':
                /* texcoord */
                /* eat up rest of line */
                if ( fgets(buf, sizeof(buf), file) == NULL)
                {
                    if (feof (file) )
                        std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                    else
                        std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
                }
                sscanf (buf, "%lf %lf", &result[0], &result[1]);
                texCoords.push_back(Vector3(result[0],result[1], result[2]));
                break;
            default:
                printf("readObj : Unknown token \"%s\".\n", buf);
                exit(1);
                break;
            }
            break;
        case 'm':
        {
            if ( fgets(buf, sizeof(buf), file) == NULL)
            {
                if (feof (file) )
                    std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                else
                    std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
            }
            sscanf(buf, "%s %s", buf, buf);
            //mtllibname = strdup(buf);
            //fscanf(file, "%s", buf);
            std::string mtlfile = sofa::helper::system::SetDirectory::GetRelativeFromFile(buf, filename);
//std::cerr << "Buf = " << buf << std::endl;
//std::cerr << "Filename = " << filename << std::endl;

            readMTL(mtlfile.c_str());
        }
        break;
        case 'u':
        {
            /* eat up rest of line */
            if ( fgets(buf, sizeof(buf), file) == NULL)
            {
                if (feof (file) )
                    std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                else
                    std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
            }
            sscanf(buf, "%s", matName);
            vector<Material>::iterator it = materials.begin();
            vector<Material>::iterator itEnd = materials.end();
            for (; it != itEnd; it++)
            {
                if (it->name == matName)
                {
//  							std::cout << "Using material "<<it->name<<std::endl;
                    (*it).activated = true;
                    material = *it;
                }
            }
        }
        break;
        case 'g':
            /* group */
            /* eat up rest of line */
            if ( fgets(buf, sizeof(buf), file) == NULL)
            {
                if (feof (file) )
                    std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                else
                    std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
            }
            sscanf(buf, "%s", buf);
            break;
        case 'l': // for now we consider a line as a 2-vertices face
        case 'f':
            // face
            if ( fgets(buf, sizeof(buf), file) == NULL)
            {
                if (feof (file) )
                    std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                else
                    std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
            }
            token = strtok(buf, " ");

            vIndices.clear();
            nIndices.clear();
            tIndices.clear();
            vertNormTexIndices.clear();

            while(token!=NULL && token[0]>='0' && token[0]<='9')
            {
                face = token;
                for (int j = 0; j < 3; j++)
                {
                    vtn[j] = 0;
                    std::string::size_type pos = face.find('/');
                    tmp = face.substr(0, pos);
                    if (tmp != "")
                        vtn[j] = atoi(tmp.c_str()) - 1; // -1 because the numerotation begins at 1 and a vector begins at 0
                    if (pos == std::string::npos)
                        face = "";
                    else
                        face = face.substr(pos + 1);
                }
                vIndices.push_back(vtn[0]);
                nIndices.push_back(vtn[1]);
                tIndices.push_back(vtn[2]);
                token = strtok(NULL, " ");
            }
            vertNormTexIndices.push_back (vIndices);
            vertNormTexIndices.push_back (nIndices);
            vertNormTexIndices.push_back (tIndices);
            facets.push_back(vertNormTexIndices);
            break;

        default:
            // eat up rest of line
            if ( fgets(buf, sizeof(buf), file) == NULL)
            {
                if (feof (file) )
                    std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                else
                    std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
            }
            break;
        }
    }

    // announce the model statistics
// 	std::cout << " Vertices: " << vertices.size() << std::endl;
// 	std::cout << " Normals: " << normals.size() << std::endl;
// 	std::cout << " Texcoords: " << texCoords.size() << std::endl;
// 	std::cout << " Triangles: " << facets.size() << std::endl;
    if (vertices.size()>0)
    {
        // compute bbox
        Vector3 minBB = vertices[0];
        Vector3 maxBB = vertices[0];
        for (unsigned int i=1; i<vertices.size(); i++)
        {
            Vector3 p = vertices[i];
            for (int c=0; c<3; c++)
            {
                if (minBB[c] > p[c])
                    minBB[c] = p[c];
                if (maxBB[c] < p[c])
                    maxBB[c] = p[c];
            }
        }

// 		std::cout << "BBox: <"<<minBB[0]<<','<<minBB[1]<<','<<minBB[2]<<">-<"<<maxBB[0]<<','<<maxBB[1]<<','<<maxBB[2]<<">\n";
    }

}

// -----------------------------------------------------
// readMTL: read a wavefront material library file
//
//    model - properly initialized GLMmodel structure
//    name  - name of the material library
// -----------------------------------------------------
void MeshOBJ::readMTL(const char* filename)
{
    std::cout << "MeshOBJ::readMTL" << std::endl;
    FILE* file;
    char buf[128];
    file = fopen(filename, "r");
    Material *mat = NULL;
    if (!file);//std::cerr << "readMTL() failed: can't open material file " << filename << std::endl;
    else
    {
        /* now, read in the data */
        while (fscanf(file, "%s", buf) != EOF)
        {

            switch (buf[0])
            {
            case '#':
                /* comment */
                /* eat up rest of line */
                if ( fgets(buf, sizeof(buf), file) == NULL)
                {
                    if (feof (file) )
                        std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                    else
                        std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
                }
                break;
            case 'n':
                /* newmtl */
                if (mat != NULL)
                {
                    materials.push_back(*mat);
                    delete mat;
                    mat = NULL;
                }
                mat = new Material();
                if ( fgets(buf, sizeof(buf), file) == NULL)
                {
                    if (feof (file) )
                        std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                    else
                        std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
                }
                sscanf(buf, "%s %s", buf, buf);
                mat->name = buf;
                break;
            case 'N':
                switch (buf[1])
                {
                case 'i':
                {
                    float optical_density;
                    if (fscanf(file, "%f", &optical_density) == EOF)
                        std::cerr << "Error: MeshOBJ: fscanf has encountered an error" << std::endl;

                    break;
                }
                case 's':
                    if( fscanf(file, "%f", &mat->shininess) == EOF)
                        std::cerr << "Error: MeshOBJ: fscanf has encountered an error" << std::endl;
                    // wavefront shininess is from [0, 1000], so scale for OpenGL
                    //mat->shininess /= 1000.0;
                    //mat->shininess *= 128.0;
                    mat->useShininess = true;
                    break;
                default:
                    /* eat up rest of line */
                    if ( fgets(buf, sizeof(buf), file) == NULL)
                    {
                        if (feof (file) )
                            std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                        else
                            std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
                    }
                    break;
                }
                break;
            case 'K':
                switch (buf[1])
                {
                case 'd':
                    if( fscanf(file, "%f %f %f", &mat->diffuse[0], &mat->diffuse[1], &mat->diffuse[2]) == EOF)
                        std::cerr << "Error: MeshOBJ: fscanf has encountered an error" << std::endl;
                    mat->useDiffuse = true;
                    /*std::cout << mat->name << " diffuse = "<<mat->diffuse[0]<<' '<<mat->diffuse[1]<<'*/ /*'<<mat->diffuse[2]<<std::endl;*/
                    break;
                case 's':
                    if( fscanf(file, "%f %f %f", &mat->specular[0], &mat->specular[1], &mat->specular[2]) == EOF)
                        std::cerr << "Error: MeshOBJ: fscanf has encountered an error" << std::endl;
                    mat->useSpecular = true;
                    /*std::cout << mat->name << " specular = "<<mat->specular[0]<<' '<<mat->specular[1]<<'*/ /*'<<mat->specular[2]<<std::endl;*/
                    break;
                case 'a':
                    if( fscanf(file, "%f %f %f", &mat->ambient[0], &mat->ambient[1], &mat->ambient[2]) == EOF)
                        std::cerr << "Error: MeshOBJ: fscanf has encountered an error" << std::endl;
                    mat->useAmbient = true;
                    /*std::cout << mat->name << " ambient = "<<mat->ambient[0]<<' '<<mat->ambient[1]<<'*/ /*'<<mat->ambient[2]<<std::endl;*/
                    break;
                default:
                    /* eat up rest of line */
                    fgets(buf, sizeof(buf), file);
                    {
                        if (feof (file) )
                            std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                        else
                            std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
                    }

                    break;
                }
                break;
            case 'd':
            case 'T':
                // transparency value
                if( fscanf(file, "%f", &mat->diffuse[3]) == EOF)
                    std::cerr << "Error: MeshOBJ: fscanf has encountered an error" << std::endl;
                break;
            default:
                /* eat up rest of line */
                if ( fgets(buf, sizeof(buf), file) == NULL)
                {
                    if (feof (file) )
                        std::cerr << "Error: MeshOBJ: fgets function has encountered end of file." << std::endl;
                    else
                        std::cerr << "Error: MeshOBJ: fgets function has encountered an error." << std::endl;
                }
                break;
            }

        }
        fclose(file);
    }
    if (mat != NULL)
    {
        materials.push_back(*mat);
        delete mat;
        mat = NULL;
    }
}

} // namespace io

} // namespace helper

} // namespace sofa

