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
#ifndef SOFA_COMPONENT_FORCEFIELD_PENALITYCONTACTFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_PENALITYCONTACTFORCEFIELD_H

#include <sofa/core/componentmodel/behavior/PairInteractionForceField.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/CollisionModel.h>
#include <sofa/defaulttype/VecTypes.h>
#include <vector>


namespace sofa
{

namespace component
{

namespace forcefield
{

template<class DataTypes>
class PenalityContactForceField : public core::componentmodel::behavior::PairInteractionForceField<DataTypes>, public virtual core::objectmodel::BaseObject
{
public:
    typedef typename core::componentmodel::behavior::PairInteractionForceField<DataTypes> Inherit;
    typedef DataTypes DataTypes1;
    typedef DataTypes DataTypes2;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    typedef core::componentmodel::behavior::MechanicalState<DataTypes> MechanicalState;
protected:

    class Contact
    {
    public:

        int m1, m2;   ///< the two extremities of the spring: masses m1 and m2
        int index1, index2; ///< the index of the two collision elements
        Deriv norm;   ///< contact normal, from m1 to m2
        Real dist;    ///< minimum distance between the points
        Real ks;      ///< spring stiffness
        Real mu_s;    ///< coulomb friction coefficient (currently unused)
        Real mu_v;    ///< viscous friction coefficient
        Real pen;     ///< current penetration
        int age;      ///< how old is this contact


        Contact(int _m1=0, int _m2=0, int _index1=0, int _index2=0, Deriv _norm=Deriv(), Real _dist=Real(), Real _ks=Real(), Real _mu_s=Real(), Real _mu_v=Real(), Real _pen=Real(), int _age=0)
            : m1(_m1),m2(_m2),index1(_index1),index2(_index2),norm(_norm),dist(_dist),ks(_ks),mu_s(_mu_s),mu_v(_mu_v),pen(_pen),age(_age)
        {
        }


        inline friend std::istream& operator >> ( std::istream& in, Contact& c )
        {
            in>>c.m1>>c.m2>>c.index1>>c.index2>>c.norm>>c.dist>>c.ks>>c.mu_s>>c.mu_v>>c.pen>>c.age;
            return in;
        }

        inline friend std::ostream& operator << ( std::ostream& out, const Contact& c )
        {
            out << c.m1<< " " <<c.m2<< " " << c.index1<< " " <<c.index2<< " " <<c.norm<< " " <<c.dist<<" " <<c.ks<<" " <<c.mu_s<<" " <<c.mu_v<<" " <<c.pen<<" " <<c.age;
            return out;
        }
    };

    Data<sofa::helper::vector<Contact> > contacts;

    // contacts from previous frame
    sofa::helper::vector<Contact> prevContacts;

public:

    PenalityContactForceField(MechanicalState* object1, MechanicalState* object2)
        : Inherit(object1, object2), contacts(initData(&contacts,"contacts", "Contacts"))
    {
    }

    PenalityContactForceField()
    {
    }

    void clear(int reserve = 0);

    void addContact(int m1, int m2, int index1, int index2, const Deriv& norm, Real dist, Real ks, Real mu_s = 0.0f, Real mu_v = 0.0f, int oldIndex = 0);

    virtual void addForce(VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2);

    virtual void addDForce(VecDeriv& df1, VecDeriv& df2, const VecDeriv& dx1, const VecDeriv& dx2);

    virtual double getPotentialEnergy(const VecCoord&, const VecCoord&);

    // -- tool grabing utility
    void grabPoint( const core::componentmodel::behavior::MechanicalState<defaulttype::Vec3Types> *tool,
            const helper::vector< unsigned int > &index,
            helper::vector< std::pair< core::objectmodel::BaseObject*, defaulttype::Vec3f> > &result,
            helper::vector< unsigned int > &triangle,
            helper::vector< unsigned int > &index_point) ;


    void draw();
};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
