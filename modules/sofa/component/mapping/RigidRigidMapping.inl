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
#ifndef SOFA_COMPONENT_MAPPING_RIGIDRIGIDMAPPING_INL
#define SOFA_COMPONENT_MAPPING_RIGIDRIGIDMAPPING_INL

#include <sofa/component/mapping/RigidRigidMapping.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/helper/io/MassSpringLoader.h>
#include <sofa/helper/io/SphereLoader.h>
#include <sofa/helper/io/Mesh.h>
#include <sofa/helper/gl/template.h>
#include <sofa/core/componentmodel/behavior/MechanicalMapping.inl>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/helper/gl/Axis.h>
#include <string.h>
#include <iostream>

using std::cerr;
using std::endl;



namespace sofa
{

namespace component
{

namespace mapping
{

using namespace sofa::defaulttype;

template <class BasicMapping>
class RigidRigidMapping<BasicMapping>::Loader : public helper::io::MassSpringLoader, public helper::io::SphereLoader
{
public:
    RigidRigidMapping<BasicMapping>* dest;
    Loader(RigidRigidMapping<BasicMapping>* dest) : dest(dest) {}
    virtual void addMass(SReal px, SReal py, SReal pz, SReal, SReal, SReal, SReal, SReal, bool, bool)
    {
        Coord c;
        Out::DataTypes::set(c,px,py,pz);
        dest->points.beginEdit()->push_back(c); //Coord((Real)px,(Real)py,(Real)pz));
    }
    virtual void addSphere(SReal px, SReal py, SReal pz, SReal)
    {
        Coord c;
        Out::DataTypes::set(c,px,py,pz);
        dest->points.beginEdit()->push_back(c); //Coord((Real)px,(Real)py,(Real)pz));
    }
};

template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::load(const char *filename)
{
    points.beginEdit()->resize(0);

    if (strlen(filename)>4 && !strcmp(filename+strlen(filename)-4,".xs3"))
    {
        Loader loader(this);
        loader.helper::io::MassSpringLoader::load(filename);
    }
    else if (strlen(filename)>4 && !strcmp(filename+strlen(filename)-4,".sph"))
    {
        Loader loader(this);
        loader.helper::io::SphereLoader::load(filename);
    }
    else if (strlen(filename)>0)
    {
        // Default to mesh loader
        helper::io::Mesh* mesh = helper::io::Mesh::Create(filename);
        if (mesh!=NULL)
        {
            points.beginEdit()->resize(mesh->getVertices().size());
            for (unsigned int i=0; i<mesh->getVertices().size(); i++)
            {
                Out::DataTypes::set((*points.beginEdit())[i], mesh->getVertices()[i][0], mesh->getVertices()[i][1], mesh->getVertices()[i][2]);
            }
            delete mesh;
        }
    }
}

template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::init()
{

    if (!fileRigidRigidMapping.getValue().empty())
        this->load(fileRigidRigidMapping.getValue().c_str());

    if (this->points.getValue().empty() && this->toModel!=NULL)
    {
        VecCoord& x = *this->toModel->getX();
        points.beginEdit()->resize(x.size());
        for (unsigned int i=0; i<x.size(); i++)
            (*points.beginEdit())[i] = x[i];
    }
    this->BasicMapping::init();
}

template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::clear()
{
    (*this->points.beginEdit()).clear();
}

/*
template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::disable()
{
	if (!this->points.getValue().empty() && this->toModel!=NULL)
	{
		VecCoord& x = *this->toModel->getX();
		x.resize(points.getValue().size());
		for (unsigned int i=0;i<points.getValue().size();i++)
			x[i] = points.getValue()[i];
	}
}
*/

template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::setRepartition(unsigned int value)
{
    vector<unsigned int>& rep = *this->repartition.beginEdit();
    rep.clear();
    rep.push_back(value);
    this->repartition.endEdit();
}

template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::setRepartition(sofa::helper::vector<unsigned int> values)
{
    vector<unsigned int>& rep = *this->repartition.beginEdit();
    rep.clear();
    rep.reserve(values.size());
    //repartition.setValue(values);
    sofa::helper::vector<unsigned int>::iterator it = values.begin();
    while (it != values.end())
    {
        rep.push_back(*it);
        it++;
    }
    this->repartition.endEdit();
}

template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::apply( typename Out::VecCoord& out, const typename In::VecCoord& in )
{
    unsigned int cptOut;
    unsigned int val;

    out.resize(points.getValue().size());
    pointsR0.resize(points.getValue().size());

    switch (repartition.getValue().size())
    {
    case 0 : //no value specified : simple rigid mapping
        in[index.getValue()].writeRotationMatrix(rotation);
        for(unsigned int i=0; i<points.getValue().size(); i++)
        {
            pointsR0[i].getCenter() = rotation*(points.getValue()[i]).getCenter();
            out[i] = in[index.getValue()].mult(points.getValue()[i]);
        }
        break;

    case 1 : //one value specified : uniform repartition.getValue() mapping on the input dofs
        val = repartition.getValue()[0];
        cptOut=0;

        for (unsigned int ifrom=0 ; ifrom<in.size() ; ifrom++)
        {
            in[ifrom].writeRotationMatrix(rotation);
            for(unsigned int ito=0; ito<val; ito++)
            {
                pointsR0[cptOut].getCenter() = rotation*(points.getValue()[cptOut]).getCenter();
                out[cptOut] = in[ifrom].mult(points.getValue()[cptOut]);
                cptOut++;
            }
        }
        break;

    default: //n values are specified : heterogen repartition.getValue() mapping on the input dofs
        if (repartition.getValue().size() != in.size())
        {
            std::cerr<<"Error : mapping dofs repartition.getValue() is not correct"<<std::endl;
            return;
        }
        cptOut=0;

        for (unsigned int ifrom=0 ; ifrom<in.size() ; ifrom++)
        {
            in[ifrom].writeRotationMatrix(rotation);
            for(unsigned int ito=0; ito<repartition.getValue()[ifrom]; ito++)
            {
                pointsR0[cptOut].getCenter() = rotation*(points.getValue()[cptOut]).getCenter();
                out[cptOut] = in[ifrom].mult(points.getValue()[cptOut]);
                cptOut++;
            }
        }
        break;
    }
}

template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::applyJ( typename Out::VecDeriv& childForces, const typename In::VecDeriv& parentForces )
{
    Vector v,omega;
    childForces.resize(points.getValue().size());
    unsigned int cptchildForces;
    unsigned int val;

    switch (repartition.getValue().size())
    {
    case 0:
        v = parentForces[index.getValue()].getVCenter();
        omega = parentForces[index.getValue()].getVOrientation();
        for(unsigned int i=0; i<points.getValue().size(); i++)
        {
            childForces[i].getVCenter() =  v + cross(omega,pointsR0[i].getCenter());
            childForces[i].getVOrientation() = omega;
        }
        break;

    case 1:
        val = repartition.getValue()[0];
        cptchildForces=0;
        for (unsigned int ifrom=0 ; ifrom<parentForces.size() ; ifrom++)
        {
            v = parentForces[ifrom].getVCenter();
            omega = parentForces[ifrom].getVOrientation();

            for(unsigned int ito=0; ito<val; ito++)
            {
                childForces[cptchildForces].getVCenter() =  v + cross(omega,(pointsR0[cptchildForces]).getCenter());
                childForces[cptchildForces].getVOrientation() = omega;
                cptchildForces++;
            }
        }
        break;

    default:
        if (repartition.getValue().size() != parentForces.size())
        {
            std::cerr<<"Error : mapping dofs repartition.getValue() is not correct"<<std::endl;
            return;
        }
        cptchildForces=0;
        for (unsigned int ifrom=0 ; ifrom<parentForces.size() ; ifrom++)
        {
            v = parentForces[ifrom].getVCenter();
            omega = parentForces[ifrom].getVOrientation();

            for(unsigned int ito=0; ito<repartition.getValue()[ifrom]; ito++)
            {
                childForces[cptchildForces].getVCenter() =  v + cross(omega,(pointsR0[cptchildForces]).getCenter());
                childForces[cptchildForces].getVOrientation() = omega;
                cptchildForces++;
            }
        }
        break;
    }

}


template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::applyJT( typename In::VecDeriv& parentForces, const typename Out::VecDeriv& childForces )
{
    Vector v,omega;
    unsigned int val;
    unsigned int cpt;
    switch(repartition.getValue().size())
    {
    case 0 :
        for(unsigned int i=0; i<points.getValue().size(); i++)
        {
            // out = Jt in
            // Jt = [ I     ]
            //      [ -OM^t ]
            // -OM^t = OM^

            Vector f = childForces[i].getVCenter();
            v += f;
            omega += childForces[i].getVOrientation() + cross(f,-pointsR0[i].getCenter());
        }
        parentForces[index.getValue()].getVCenter() += v;
        parentForces[index.getValue()].getVOrientation() += omega;
        break;
    case 1 :
        val = repartition.getValue()[0];
        cpt=0;
        for(unsigned int ito=0; ito<parentForces.size(); ito++)
        {
            v=Vector();
            omega=Vector();
            for(unsigned int i=0; i<val; i++)
            {
                Vector f = childForces[cpt].getVCenter();
                v += f;
                omega += childForces[cpt].getVOrientation() + cross(f,-pointsR0[cpt].getCenter());
                cpt++;
            }
            parentForces[ito].getVCenter() += v;
            parentForces[ito].getVOrientation() += omega;
        }
        break;
    default :
        if (repartition.getValue().size() != parentForces.size())
        {
            std::cerr<<"Error : mapping dofs repartition.getValue() is not correct"<<std::endl;
            return;
        }
        cpt=0;
        for(unsigned int ito=0; ito<parentForces.size(); ito++)
        {
            v=Vector();
            omega=Vector();
            for(unsigned int i=0; i<repartition.getValue()[ito]; i++)
            {
                Vector f = childForces[cpt].getVCenter();
                v += f;
                omega += childForces[cpt].getVOrientation() + cross(f,-pointsR0[cpt].getCenter());
                cpt++;
            }
            parentForces[ito].getVCenter() += v;
            parentForces[ito].getVOrientation() += omega;

        }
        break;
    }

}
template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::computeAccFromMapping(  typename Out::VecDeriv& acc_out, const typename In::VecDeriv& v_in, const typename In::VecDeriv& acc_in)
{
    acc_out.clear();
    acc_out.resize(points.getValue().size());


    // current acceleration on acc_in is applied on the child (when more than one mapping)
    applyJ(acc_out,acc_in);

    // computation of the acceleration due to the current velocity
    // a+= w^(w^OM)

    Vector omega;
    unsigned int cptchildV;
    unsigned int val;

    switch (repartition.getValue().size())
    {
    case 0:
        omega = v_in[index.getValue()].getVOrientation();
        for(unsigned int i=0; i<points.getValue().size(); i++)
        {
            acc_out[i].getVCenter() +=   cross(omega, cross(omega,pointsR0[i].getCenter()) );
        }
        break;

    case 1:
        val = repartition.getValue()[0];
        cptchildV=0;
        for (unsigned int ifrom=0 ; ifrom<v_in.size() ; ifrom++)
        {
            omega = v_in[ifrom].getVOrientation();

            for(unsigned int ito=0; ito<val; ito++)
            {
                acc_out[cptchildV].getVCenter() +=  cross(omega, cross(omega,(pointsR0[cptchildV]).getCenter()) );
                cptchildV++;
            }
        }
        break;

    default:
        if (repartition.getValue().size() != v_in.size())
        {
            std::cerr<<"Error : mapping dofs repartition.getValue() is not correct"<<std::endl;
            return;
        }
        cptchildV=0;
        for (unsigned int ifrom=0 ; ifrom<v_in.size() ; ifrom++)
        {
            omega = v_in[ifrom].getVOrientation();

            for(unsigned int ito=0; ito<repartition.getValue()[ifrom]; ito++)
            {
                acc_out[cptchildV].getVCenter() += cross(omega, cross(omega,(pointsR0[cptchildV]).getCenter()) );
                cptchildV++;
            }
        }
        break;
    }



}

template <class BasicMapping>
void RigidRigidMapping<BasicMapping>::draw()
{
    if (!getShow(this)) return;
    const typename Out::VecCoord& x = *this->toModel->getX();
    for (unsigned int i=0; i<x.size(); i++)
    {
        helper::gl::Axis::draw(x[i].getCenter(), x[i].getOrientation(), axisLength.getValue());
    }
    glEnd();
}

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
