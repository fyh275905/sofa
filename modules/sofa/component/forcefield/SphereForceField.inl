#ifndef SOFA_COMPONENT_FORCEFIELD_SPHEREFORCEFIELD_INL
#define SOFA_COMPONENT_FORCEFIELD_SPHEREFORCEFIELD_INL

#include <sofa/core/componentmodel/behavior/ForceField.inl>
#include "SphereForceField.h"
#include <sofa/helper/system/config.h>
#include <sofa/helper/rmath.h>
#include <sofa/helper/system/gl.h>
#include <sofa/helper/system/glut.h>
#include <assert.h>
#include <iostream>

namespace sofa
{

namespace component
{

namespace forcefield
{

// f  = -stiffness * (x -c ) * (|x-c|-r)/|x-c|
// fi = -stiffness * (xi-ci) * (|x-c|-r)/|x-c|
// dfi/dxj = -stiffness * ( (xi-ci) * d((|x-c|-r)/|x-c|)/dxj + d(xi-ci)/dxj * (|x-c|-r)/|x-c| )
// d(xi-ci)/dxj = 1 if i==j, 0 otherwise
// d((|x-c|-r)/|x-c|)/dxj = (|x-c|*d(|x-c|-r)/dxj - d(|x-c|)/dxj * (|x-c|-r))/|x-c|^2
//                        = (d(|x-c|)/dxj * (|x-c| - |x-c| + r))/|x-c|^2
//                        = r/|x-c|^2 * d(|x-c|)/dxj
//                        = r/|x-c|^2 * d(sqrt(sum((xi-ci)^2)))/dxj
//                        = r/|x-c|^2 * 1/2 * 1/sqrt(sum((xi-ci)^2)) * d(sum(xi-ci)^2)/dxj
//                        = r/|x-c|^2 * 1/2 * 1/|x-c| * d((xj-cj)^2)/dxj
//                        = r/|x-c|^2 * 1/2 * 1/|x-c| * (2(xj-cj))
//                        = r/|x-c|^2 * (xj-cj)/|x-c|
// dfi/dxj = -stiffness * ( (xi-ci) * r/|x-c|^2 * (xj-cj)/|x-c| + (i==j) * (|x-c|-r)/|x-c| )
//         = -stiffness * ( (xi-ci)/|x-c| * (xj-cj)/|x-c| * r/|x-c| + (i==j) * (1 - r/|x-c|) )
// df = -stiffness * ( (x-c)/|x-c| * dot(dx,(x-c)/|x-c|) * r/|x-c|   + dx * (1 - r/|x-c|) )

template<class DataTypes>
void SphereForceField<DataTypes>::addForce(VecDeriv& f1, const VecCoord& p1, const VecDeriv& v1)
{
    const Coord center = sphereCenter.getValue();
    const Real r = sphereRadius.getValue();
    const Real r2 = r*r;
    this->contacts.beginEdit()->clear();
    f1.resize(p1.size());
    for (unsigned int i=0; i<p1.size(); i++)
    {
        Coord dp = p1[i] - center;
        Real norm2 = dp.norm2();
        if (norm2<r2)
        {
            Real norm = helper::rsqrt(norm2);
            Real d = norm - r;
            Real forceIntensity = -this->stiffness.getValue()*d;
            Real dampingIntensity = -this->damping.getValue()*d;
            Deriv force = dp*(forceIntensity/norm) - v1[i]*dampingIntensity;
            f1[i]+=force;
            Contact c;
            c.index = i;
            c.normal = dp / norm;
            c.fact = r / norm;
            this->contacts.beginEdit()->push_back(c);
        }
    }
    this->contacts.endEdit();
}

template<class DataTypes>
void SphereForceField<DataTypes>::addDForce(VecDeriv& df1, const VecDeriv& dx1)
{
    df1.resize(dx1.size());
    for (unsigned int i=0; i<this->contacts.getValue().size(); i++)
    {
        const Contact& c = (*this->contacts.beginEdit())[i];
        assert((unsigned)c.index<dx1.size());
        Deriv du = dx1[c.index];
        Deriv dforce; dforce = -this->stiffness.getValue()*(c.normal * ((du*c.normal)*c.fact) + du * (1 - c.fact));
        df1[c.index] += dforce;
    }
    this->contacts.endEdit();
}

template<class DataTypes>
void SphereForceField<DataTypes>::updateStiffness( const VecCoord& x )
{
    const Coord center = sphereCenter.getValue();
    const Real r = sphereRadius.getValue();
    const Real r2 = r*r;
    this->contacts.beginEdit()->clear();
    for (unsigned int i=0; i<x.size(); i++)
    {
        Coord dp = x[i] - center;
        Real norm2 = dp.norm2();
        if (norm2<r2)
        {
            Real norm = helper::rsqrt(norm2);
            Contact c;
            c.index = i;
            c.normal = dp / norm;
            c.fact = r / norm;
            this->contacts.beginEdit()->push_back(c);
        }
    }
    this->contacts.endEdit();
}

template <class DataTypes>
double SphereForceField<DataTypes>::getPotentialEnergy(const VecCoord&)
{
    std::cerr<<"SphereForceField::getPotentialEnergy-not-implemented !!!"<<std::endl;
    return 0;
}

template<class DataTypes>
void SphereForceField<DataTypes>::draw()
{
    if (!getContext()->getShowForceFields()) return;
    if (!bDraw.getValue()) return;

    const Coord center = sphereCenter.getValue();
    const Real r = sphereRadius.getValue();

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(color.getValue()[0],color.getValue()[1],color.getValue()[2]);
    glPushMatrix();
    glTranslated(center[0], center[1], center[2]);
    glutSolidSphere(r*0.99,32,16); // slightly reduce rendered radius
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
}


} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
