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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_FORCEFIELD_FRAMESPRINGFORCEFIELD_INL
#define SOFA_COMPONENT_FORCEFIELD_FRAMESPRINGFORCEFIELD_INL

#include <sofa/component/forcefield/FrameSpringForceField.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/helper/io/MassSpringLoader.h>
#include <sofa/helper/gl/template.h>
#include <sofa/helper/gl/Cylinder.h>
#include <sofa/helper/gl/Axis.h>
#include <sofa/helper/system/config.h>
#include <assert.h>
#include <iostream>




namespace sofa
{

namespace component
{

namespace forcefield
{


template<class DataTypes>
FrameSpringForceField<DataTypes>::FrameSpringForceField ( MechanicalState* object1, MechanicalState* object2 )
    : Inherit ( object1, object2 )
    , springs ( initData ( &springs,"spring","pairs of indices, stiffness, damping, rest length" ) )
    , showLawfulTorsion ( initData ( &showLawfulTorsion, false, "show lawful Torsion", "dislpay the lawful part of the joint rotation" ) )
    , showExtraTorsion ( initData ( &showExtraTorsion, false, "show illicit Torsion", "dislpay the illicit part of the joint rotation" ) )
{
}

template<class DataTypes>
FrameSpringForceField<DataTypes>::FrameSpringForceField()
    : springs ( initData ( &springs,"spring","pairs of indices, stiffness, damping, rest length" ) )
    , showLawfulTorsion ( initData ( &showLawfulTorsion, false, "show lawful Torsion", "dislpay the lawful part of the joint rotation" ) )
    , showExtraTorsion ( initData ( &showExtraTorsion, false, "show illicit Torsion", "dislpay the illicit part of the joint rotation" ) )
{
}


template <class DataTypes>
void FrameSpringForceField<DataTypes>::init()
{
    this->Inherit::init();
}

template<class DataTypes>
void FrameSpringForceField<DataTypes>::addSpringForce ( double& /*potentialEnergy*/, VecDeriv& f1, const VecCoord& p1, const VecDeriv& v1, VecDeriv& f2, const VecCoord& p2, const VecDeriv& v2, int , const Spring& spring )
{
    int a = spring.m1;
    int b = spring.m2;

    Mat Mr01, Mr10, Mr02, Mr20;
    p1[a].writeRotationMatrix ( Mr01 );
    invertMatrix ( Mr10, Mr01 );
    p2[b].writeRotationMatrix ( Mr02 );
    invertMatrix ( Mr20, Mr02 );

    Deriv Vp1p2 = v2[b] - v1[a];

    VecN damping ( spring.kd, spring.kd, spring.kd );
    VecN kst ( spring.stiffnessTrans, spring.stiffnessTrans, spring.stiffnessTrans );
    VecN ksr ( spring.stiffnessRot, spring.stiffnessRot, spring.stiffnessRot );

    //store the referential of the spring (p1) to use it in addSpringDForce()
    springRef[a] = p1[a];

    VecN fT = kst.linearProduct( ( p2[b].getCenter() + Mr02 * ( spring.vec2)) - ( p1[a].getCenter() + Mr01 * ( spring.vec1))) + damping.linearProduct ( Vp1p2.getVCenter());
    VecN fR = ksr.linearProduct( ( p1[a].getOrientation().inverse() * p2[b].getOrientation()).toEulerVector());

    VecN C1 = fR + cross( Mr01 * ( spring.vec1), fT) + damping.linearProduct ( Vp1p2.getVOrientation() );
    VecN C2 = fR + cross( Mr02 * ( spring.vec2), fT) + damping.linearProduct ( -Vp1p2.getVOrientation() );

    f1[a] += Deriv ( fT, C1);
    f2[b] -= Deriv ( fT, C2);

    /*serr << "f1: " << fT1 << ", " << fR1 << endl;
    serr << "f2: " << fT2 << ", " << fR2 << endl;
    serr << "sum: " << fT2 + fT1 << ", " << fR2 + fR1 << endl;
    serr << "diff: " << fT2 - fT1 << ", " << fR2 - fR1 << endl;*/
}

template<class DataTypes>
void FrameSpringForceField<DataTypes>::addSpringDForce ( VecDeriv& f1, const VecDeriv& dx1, VecDeriv& f2, const VecDeriv& dx2, int , const Spring& spring )
{
    const int a = spring.m1;
    const int b = spring.m2;
    const Deriv Mdx1dx2 = dx2[b] - dx1[a];

    Mat Mr01, Mr10;
    springRef[a].writeRotationMatrix ( Mr01 );
    invertMatrix ( Mr10, Mr01 );

    VecN kst ( spring.stiffnessTrans, spring.stiffnessTrans, spring.stiffnessTrans );
    VecN ksr ( spring.stiffnessRot, spring.stiffnessRot, spring.stiffnessRot );

    //compute directional force
    VecN df0 = Mr01 * ( kst.linearProduct ( Mr10*Mdx1dx2.getVCenter() ) );
    //compute rotational force
    VecN dR0 = Mr01 * ( ksr.linearProduct ( Mr10* Mdx1dx2.getVOrientation() ) );

    const Deriv dforce ( df0,dR0);

    f1[a] += dforce;
    f2[b] -= dforce;
}

template<class DataTypes>
void FrameSpringForceField<DataTypes>::addForce ( VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2 )
{
    springRef.resize ( x1.size() );
    f1.resize ( x1.size() );
    f2.resize ( x2.size() );
    m_potentialEnergy = 0;
    const sofa::helper::vector<Spring>& springsVec = springs.getValue();
    for ( unsigned int i=0; i<springsVec.size(); i++ )
    {
        this->addSpringForce ( m_potentialEnergy,f1,x1,v1,f2,x2,v2, i, springsVec[i] );
    }
}

template<class DataTypes>
void FrameSpringForceField<DataTypes>::addDForce ( VecDeriv& df1, VecDeriv& df2, const VecDeriv& dx1, const VecDeriv& dx2 )
{
    df1.resize ( dx1.size() );
    df2.resize ( dx2.size() );

    const sofa::helper::vector<Spring>& springsVec = springs.getValue();
    for ( unsigned int i=0; i<springsVec.size(); i++ )
    {
        this->addSpringDForce ( df1,dx1,df2,dx2, i, springsVec[i] );
    }
}

template<class DataTypes>
void FrameSpringForceField<DataTypes>::draw()
{
    if ( ! ( ( this->mstate1 == this->mstate2 ) ?this->getContext()->getShowForceFields() :this->getContext()->getShowInteractionForceFields() ) ) return;
    const VecCoord& p1 = *this->mstate1->getX();
    const VecCoord& p2 = *this->mstate2->getX();

    glDisable ( GL_LIGHTING );
    bool external = ( this->mstate1!=this->mstate2 );
    const helper::vector<Spring>& springs = this->springs.getValue();

    for ( unsigned int i=0; i<springs.size(); i++ )
    {
        double restLength = (springs[i].vec1.norm() + springs[i].vec2.norm());
        Real d = ( p2[springs[i].m2].getCenter()-p1[springs[i].m1].getCenter()).norm();
        if ( external )
        {
            if ( d < restLength *0.9999 )
                glColor4f ( 1,0,0,1 );
            else
                glColor4f ( 0,1,0,1 );
        }
        else
        {
            if ( d < restLength *0.9999 )
                glColor4f ( 1,0.5f,0,1 );
            else
                glColor4f ( 0,1,0.5f,1 );
        }
        glBegin ( GL_LINES );
        helper::gl::glVertexT ( p1[springs[i].m1].getCenter() );
        helper::gl::glVertexT ( p2[springs[i].m2].getCenter() );

        //Debug: display fT: the virtual displacement of the spring( see the model on top of FrameSpringForceField.h
        /*
        glColor4f ( 1,1,1,1 );
        helper::gl::glVertexT ( p1[springs[i].m1].getCenter() + p1[springs[i].m1].getOrientation().rotate ( springs[i].initRot1.rotate ( VecN ( springs[i].initLength/2, 0, 0 ) ) ) );
        helper::gl::glVertexT ( p2[springs[i].m2].getCenter() + p2[springs[i].m2].getOrientation().rotate ( springs[i].initRot2.rotate ( VecN ( -springs[i].initLength/2, 0, 0 ) ) ) );
        //*/
        glEnd();
    }
}


template<class DataTypes>
void FrameSpringForceField<DataTypes>::clear ( int reserve )
{
    helper::vector<Spring>& springs = *this->springs.beginEdit();
    springs.clear();
    if ( reserve ) springs.reserve ( reserve );
    this->springs.endEdit();
}

template<class DataTypes>
void FrameSpringForceField<DataTypes>::addSpring ( const Spring& s )
{
    springs.beginEdit()->push_back ( s );
    springs.endEdit();
}


template<class DataTypes>
void FrameSpringForceField<DataTypes>::addSpring ( int m1, int m2, Real softKst, Real softKsr, Real kd )
{
    Spring s ( m1,m2,softKst,softKsr, kd );
    //TODO// Init vec1 et vec2. Encore mieux a la creation du ressort mais il manque les positions des DOFs...
//const MechanicalState<DataTypes> obj1 = *(getMState1());
//const MechanicalState<DataTypes> obj2 = *(getMState2());

    springs.beginEdit()->push_back ( s );
    springs.endEdit();
}

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif

