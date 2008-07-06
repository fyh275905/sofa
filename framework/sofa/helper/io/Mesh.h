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
#ifndef SOFA_HELPER_IO_MESH_H
#define SOFA_HELPER_IO_MESH_H

#include <sofa/helper/vector.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/helper/Factory.h>
//#include <sofa/core/objectmodel/Data.h>
namespace sofa
{

namespace helper
{

namespace io
{

using sofa::helper::vector;
using sofa::defaulttype::Vector3;

using sofa::defaulttype::Vec4f;

class Mesh
{
public:

    class Material
    {
    public:
        std::string 	name;		/* name of material */
        Vec4f  diffuse ;	/* diffuse component */
        Vec4f  ambient ;	/* ambient component */
        Vec4f  specular;	/* specular component */
        Vec4f  emissive;	/* emmissive component */
        float  shininess;	/* specular exponent */
        bool   useDiffuse;
        bool   useSpecular;
        bool   useAmbient;
        bool   useEmissive;
        bool   useShininess;
        bool   activated;

        void setColor(float r, float g, float b, float a);

        inline friend std::ostream& operator << (std::ostream& out, const Material& m )
        {
            out   << m.name         << " ";
            out  << "Diffuse"       << " " <<  m.useDiffuse   << " " <<  m.diffuse      << " ";
            out  << "Ambient"       << " " <<  m.useAmbient   << " " <<  m.ambient      << " ";
            out  << "Specular"      << " " <<  m.useSpecular  << " " <<  m.specular     << " ";
            out  << "Emissive"      << " " <<  m.useEmissive  << " " <<  m.emissive     << " ";
            out  << "Shininess"     << " " <<  m.useShininess << " " <<  m.shininess ;
            return out;
        }
        inline friend std::istream& operator >> (std::istream& in, Material &m )
        {

            std::string element;
            in  >>  m.name ;
            for (unsigned int i=0; i<5; ++i)
            {
                in  >>  element;
                if      (element == std::string("Diffuse")   || element == std::string("diffuse")   ) { in  >>  m.useDiffuse   ; in >> m.diffuse;   }
                else if (element == std::string("Ambient")   || element == std::string("ambient")   ) { in  >>  m.useAmbient   ; in >> m.ambient;   }
                else if (element == std::string("Specular")  || element == std::string("specular")  ) { in  >>  m.useSpecular  ; in >> m.specular;  }
                else if (element == std::string("Emissive")  || element == std::string("emissive")  ) { in  >>  m.useEmissive  ; in >> m.emissive;  }
                else if (element == std::string("Shininess") || element == std::string("shininess") ) { in  >>  m.useShininess ; in >> m.shininess; }
            }
            return in;
        }

        Material();
    };

protected:
    vector<Vector3> vertices;
    vector<Vector3> texCoords; // for the moment, we suppose that texCoords is order 2 (2 texCoords for a vertex)
    vector<Vector3> normals;
    vector< vector < vector <int> > > facets;
    //sofa::core::objectmodel::Data< Material > material;
    Material material;

    std::string textureName;
public:

    vector<Vector3> & getVertices()
    {
        //std::cout << "vertices size : " << vertices.size() << std::endl;
        return vertices;
    };
    vector<Vector3> & getTexCoords() { return texCoords; }
    vector<Vector3> & getNormals() { return normals; }
    vector< vector < vector <int> > > & getFacets()
    {
        //std::cout << "facets size : " << facets.size() << std::endl;
        return facets;
    };
    const Material& getMaterial() {return material; }

    std::string& getTextureName()
    {
        return textureName;
    };

    typedef Factory<std::string, Mesh, std::string> FactoryMesh;

    static Mesh* Create(std::string filename);

    template<class Object>
    static void create(Object*& obj, std::string arg)
    {
        obj = new Object(arg);
    }

};

} // namespace io

} // namespace helper

} // namespace sofa

#endif
