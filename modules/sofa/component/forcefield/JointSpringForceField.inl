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
#ifndef SOFA_COMPONENT_FORCEFIELD_JOINTSPRINGFORCEFIELD_INL
#define SOFA_COMPONENT_FORCEFIELD_JOINTSPRINGFORCEFIELD_INL

#include <sofa/component/forcefield/JointSpringForceField.h>
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
JointSpringForceField<DataTypes>::JointSpringForceField(MechanicalState* object1, MechanicalState* object2)
    : Inherit(object1, object2)
    , springs(initData(&springs,"spring","pairs of indices, stiffness, damping, rest length"))
    , showLawfulTorsion(initData(&showLawfulTorsion, false, "showLawfulTorsion", "display the lawful part of the joint rotation"))
    , showExtraTorsion(initData(&showExtraTorsion, false, "showExtraTorsion", "display the illicit part of the joint rotation"))
    , showFactorSize(initData(&showFactorSize, 1.0f, "showFactorSize", "modify the size of the debug information of a given factor" ))
{
}

template<class DataTypes>
JointSpringForceField<DataTypes>::JointSpringForceField()
    : springs(initData(&springs,"spring","pairs of indices, stiffness, damping, rest length"))
    , showLawfulTorsion(initData(&showLawfulTorsion, false, "showLawfulTorsion", "display the lawful part of the joint rotation"))
    , showExtraTorsion(initData(&showExtraTorsion, false, "showExtraTorsion", "display the illicit part of the joint rotation"))
    , showFactorSize(initData(&showFactorSize, 1.0f, "showFactorSize", "modify the size of the debug information of a given factor" ))
{
}

template<class DataTypes>
JointSpringForceField<DataTypes>::~JointSpringForceField()
{
}

template <class DataTypes>
void JointSpringForceField<DataTypes>::bwdInit()
{
    this->Inherit::bwdInit();

    const VecCoord& x1= *this->mstate1->getX();

    const VecCoord& x2= *this->mstate2->getX();
    sofa::helper::vector<Spring> &springsVector=*(springs.beginEdit());
    for (unsigned int i=0; i<springs.getValue().size(); ++i)
    {
        Spring &s=springsVector[i];
        if (s.needToInitializeTrans)
        {
            s.initTrans = x2[s.m2].getCenter() - x1[s.m1].getCenter();
        }
        if (s.needToInitializeRot)
        {
            s.initRot   = x2[s.m2].getOrientation()*x1[s.m1].getOrientation().inverse();
        }
    }
    springs.endEdit();
}


template<class DataTypes>
void JointSpringForceField<DataTypes>::addSpringForce( double& /*potentialEnergy*/, VecDeriv& f1, const VecCoord& p1, const VecDeriv& v1, VecDeriv& f2, const VecCoord& p2, const VecDeriv& v2, int , /*const*/ Spring& spring)
{
    int a = spring.m1;
    int b = spring.m2;

    Mat Mr01, Mr10;
    p1[a].writeRotationMatrix(Mr01);
    invertMatrix(Mr10, Mr01);

    Vector damping(spring.kd, spring.kd, spring.kd);

    //store the referential of the spring (p1) to use it in addSpringDForce()
    springRef[a] = p1[a];

    //compute p2 position and velocity, relative to p1 referential
    Coord Mp1p2 = p2[b] - p1[a];
    Deriv Vp1p2 = v2[b] - v1[a];

    //compute elongation
    Mp1p2.getCenter() -= spring.initTrans;
    //compute torsion
    Mp1p2.getOrientation() = Mp1p2.getOrientation() * spring.initRot.inverse();

    //-- decomposing spring torsion in 2 parts (lawful rotation and illicit rotation) to fix bug with large rotations
    Vector dRangles = Mp1p2.getOrientation().toEulerVector();
    //lawful torsion = spring torsion in the axis where ksr is null
    Vector lawfulRots( (Real)spring.freeMovements[3], (Real)spring.freeMovements[4], (Real)spring.freeMovements[5] );
    spring.lawfulTorsion = Quat::createQuaterFromEuler(dRangles.linearProduct(lawfulRots));
    Mat MRLT;
    spring.lawfulTorsion.toMatrix(MRLT);
    //extra torsion = spring torsion in the other axis (ksr not null), expressed in the lawful torsion reference axis
    // |--> we apply rotation forces on it, and then go back to world reference axis
    spring.extraTorsion = spring.lawfulTorsion.inverse()*Mp1p2.getOrientation();

    //--
    //--test limit angles
    Vector dTorsion = spring.lawfulTorsion.toEulerVector();
    bool bloc=false;
    for (unsigned int i=0; i<3; i++)
    {
        if (spring.freeMovements[3+i] && dRangles[i] < spring.limitAngles[i*2])
        {
            spring.bloquage[i] = spring.blocStiffnessRot;
            dTorsion[i] -= spring.limitAngles[i*2];
            bloc=true;
        }
        else if (spring.freeMovements[3+i] && dRangles[i] > spring.limitAngles[i*2+1])
        {
            spring.bloquage[i] = spring.blocStiffnessRot;
            dTorsion[i] -= spring.limitAngles[i*2+1];
            bloc=true;
        }
    }
    //--

    //compute stiffnesses components
    Vector kst( spring.freeMovements[0]==0?spring.hardStiffnessTrans:spring.softStiffnessTrans, spring.freeMovements[1]==0?spring.hardStiffnessTrans:spring.softStiffnessTrans, spring.freeMovements[2]==0?spring.hardStiffnessTrans:spring.softStiffnessTrans);
    Vector ksrH( spring.freeMovements[3]!=0?(Real)0.0:spring.hardStiffnessRot, spring.freeMovements[4]!=0?(Real)0.0:spring.hardStiffnessRot, spring.freeMovements[5]!=0?(Real)0.0:spring.hardStiffnessRot);
    Vector ksrS( spring.freeMovements[3]==0?(Real)0.0:spring.softStiffnessRot, spring.freeMovements[4]==0?(Real)0.0:spring.softStiffnessRot, spring.freeMovements[5]==0?(Real)0.0:spring.softStiffnessRot);

    //compute directional force (relative translation is expressed in world coordinates)
    Vector fT0 = Mr01 * (kst.linearProduct(Mr10 * Mp1p2.getCenter())) + damping.linearProduct(Vp1p2.getVCenter());
    //compute rotational force (relative orientation is expressed in p1)
    Vector fR0 = Mr01 * MRLT * ( ksrH.linearProduct(spring.extraTorsion.toEulerVector())) + damping.linearProduct(Vp1p2.getVOrientation());
    fR0 += Mr01 * ( ksrS.linearProduct(spring.lawfulTorsion.toEulerVector())) + damping.linearProduct(Vp1p2.getVOrientation());
    //--
    if(bloc)
        fR0 += Mr01 * ( spring.bloquage.linearProduct( dTorsion)) + damping.linearProduct(Vp1p2.getVOrientation());

    const Deriv force(fT0, fR0 );
    //affect forces
    f1[a] += force;
    this->mask1->insertEntry(a);
    f2[b] -= force;
    this->mask2->insertEntry(b);

}

template<class DataTypes>
void JointSpringForceField<DataTypes>::addSpringDForce(VecDeriv& f1, const VecDeriv& dx1, VecDeriv& f2, const VecDeriv& dx2, int , /*const*/ Spring& spring)
{
    const int a = spring.m1;
    const int b = spring.m2;
    const Deriv Mdx1dx2 = dx2[b]-dx1[a];

    Mat Mr01, Mr10;
    springRef[a].writeRotationMatrix(Mr01);
    invertMatrix(Mr10, Mr01);

    Vector kst( spring.freeMovements[0]==0?spring.hardStiffnessTrans:spring.softStiffnessTrans, spring.freeMovements[1]==0?spring.hardStiffnessTrans:spring.softStiffnessTrans, spring.freeMovements[2]==0?spring.hardStiffnessTrans:spring.softStiffnessTrans);
    Vector ksr( spring.freeMovements[3]==0?spring.hardStiffnessRot:spring.softStiffnessRot+spring.bloquage[0], spring.freeMovements[4]==0?spring.hardStiffnessRot:spring.softStiffnessRot+spring.bloquage[1], spring.freeMovements[5]==0?spring.hardStiffnessRot:spring.softStiffnessRot+spring.bloquage[2]);

    //compute directional force
    Vector df0 = Mr01 * (kst.linearProduct(Mr10*Mdx1dx2.getVCenter() ));
    //compute rotational force
    Vector dR0 = Mr01 * (ksr.linearProduct(Mr10* Mdx1dx2.getVOrientation()));

    const Deriv dforce(df0,dR0);

    f1[a]+=dforce;
    f2[b]-=dforce;

    //--
    spring.bloquage=Vector();
}

template<class DataTypes>
void JointSpringForceField<DataTypes>::addForce(VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2)
{
    helper::vector<Spring>& springs = *this->springs.beginEdit();

    springRef.resize(x1.size());

    f1.resize(x1.size());
    f2.resize(x2.size());
    m_potentialEnergy = 0;
    for (unsigned int i=0; i<springs.size(); i++)
    {
        this->addSpringForce(m_potentialEnergy,f1,x1,v1,f2,x2,v2, i, springs[i]);
    }
    this->springs.endEdit();
}

template<class DataTypes>
void JointSpringForceField<DataTypes>::addDForce(VecDeriv& df1, VecDeriv& df2, const VecDeriv& dx1, const VecDeriv& dx2)
{
    df1.resize(dx1.size());
    df2.resize(dx2.size());

    //const helper::vector<Spring>& springs = this->springs.getValue();
    helper::vector<Spring>& springs = *this->springs.beginEdit();
    for (unsigned int i=0; i<springs.size(); i++)
    {
        this->addSpringDForce(df1,dx1,df2,dx2, i, springs[i]);
    }
    this->springs.endEdit();
}

template<class DataTypes>
void JointSpringForceField<DataTypes>::draw()
{
    if (!((this->mstate1 == this->mstate2)?getContext()->getShowForceFields():getContext()->getShowInteractionForceFields())) return;
    const VecCoord& p1 = *this->mstate1->getX();
    const VecCoord& p2 = *this->mstate2->getX();

    glDisable(GL_LIGHTING);
    bool external = (this->mstate1!=this->mstate2);
    const helper::vector<Spring>& springs = this->springs.getValue();

    for (unsigned int i=0; i<springs.size(); i++)
    {
        Real d = (p2[springs[i].m2]-p1[springs[i].m1]).getCenter().norm();
        if (external)
        {
            if (d<springs[i].initTrans.norm()*0.9999)
                glColor4f(1,0,0,1);
            else
                glColor4f(0,1,0,1);
        }
        else
        {
            if (d<springs[i].initTrans.norm()*0.9999)
                glColor4f(1,0.5f,0,1);
            else
                glColor4f(0,1,0.5f,1);
        }
        glBegin(GL_LINES);
        helper::gl::glVertexT(p1[springs[i].m1].getCenter());
        helper::gl::glVertexT(p2[springs[i].m2].getCenter());
        glEnd();

        if(springs[i].freeMovements[3] == 1)
        {
            helper::gl::Cylinder::draw(p1[springs[i].m1].getCenter(), p1[springs[i].m1].getOrientation(), Vector((Real)(1.0*showFactorSize.getValue()),0,0));
        }
        if(springs[i].freeMovements[4] == 1)
        {
            helper::gl::Cylinder::draw(p1[springs[i].m1].getCenter(), p1[springs[i].m1].getOrientation(), Vector(0,1*showFactorSize.getValue(),0));
        }
        if(springs[i].freeMovements[5] == 1)
        {
            helper::gl::Cylinder::draw(p1[springs[i].m1].getCenter(), p1[springs[i].m1].getOrientation(), Vector(0,0,1*showFactorSize.getValue()));
        }

        //---debugging
        if (showLawfulTorsion.getValue())
            helper::gl::Axis::draw(p1[springs[i].m1].getCenter(), p1[springs[i].m1].getOrientation()*springs[i].lawfulTorsion, 0.5*showFactorSize.getValue());
        if (showExtraTorsion.getValue())
            helper::gl::Axis::draw(p1[springs[i].m1].getCenter(), p1[springs[i].m1].getOrientation()*springs[i].extraTorsion, 0.5*showFactorSize.getValue());
    }

}

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif

