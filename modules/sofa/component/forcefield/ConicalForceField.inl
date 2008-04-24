#ifndef SOFA_COMPONENT_FORCEFIELD_SPHEREFORCEFIELD_INL
#define SOFA_COMPONENT_FORCEFIELD_SPHEREFORCEFIELD_INL

#include <sofa/core/componentmodel/behavior/ForceField.inl>
#include <sofa/component/forcefield/ConicalForceField.h>
#include <sofa/defaulttype/Quat.h>

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

template<class DataTypes>
void ConicalForceField<DataTypes>::addForce(VecDeriv& f1, const VecCoord& p1, const VecDeriv& v1)
{
    const Coord center = coneCenter.getValue();
    Real d = 0.0;
    Real alpha = 0.0;
    Real length_cp_prime = 0.0;
    Coord p, p_prime, cp, dir, n_cp, cp_new, cp_prime, pp_prime, t;
    Coord n = coneHeight.getValue();
    n.normalize();
    Deriv force;

    this->contacts.beginEdit()->clear();
    f1.resize(p1.size());
    for (unsigned int i=0; i<p1.size(); i++)
    {
        p = p1[i];
        if (!isIn(p))
        {
            cp = p - center;
            //below the cone
            if (cp.norm() >  coneHeight.getValue().norm())
            {
                Real norm = cp.norm();
                d = norm - coneHeight.getValue().norm();
                dir = cp / norm;
                Real forceIntensity = -this->stiffness.getValue()*d;
                Real dampingIntensity = -this->damping.getValue()*d;
                force = cp*(forceIntensity/norm) - v1[i]*dampingIntensity;
                f1[i]+=force;
                Contact c;
                c.index = i;
                c.normal = cp / norm;
                c.pos = p;
                this->contacts.beginEdit()->push_back(c);

            }
            else
            {
                //side of the cone
                if (dot(cp,n) > 0)
                {
                    n_cp = cp/cp.norm();
                    alpha = acos(n_cp*n) - coneAngle.getValue()*3.1459/180 ;
                    t = n.cross(cp) ;
                    t /= t.norm();
                    defaulttype::Quat q(t, -alpha);
                    cp_new = q.rotate(cp);

                    cp_new.normalize();
                    length_cp_prime = dot(cp_new, cp);
                    cp_prime = cp_new * length_cp_prime;
                    p_prime = cp_prime + center;

                    pp_prime = p_prime - p;
                    d = pp_prime.norm();
                    dir = pp_prime/pp_prime.norm();
                    //std::cout << t << " " << alpha << std::endl;
                }
                //top of the cone
                else
                {
                    d = cp.norm();
                    dir = (-cp)/cp.norm();
                }

                force = dir * (stiffness.getValue()*d);
                force += dir * (damping.getValue() * (-dot(dir,v1[i])));
                f1[i] += force;
                Contact c;
                c.index = i;
                c.normal = dir;
                c.pos = p;
                //c.fact = r / norm;
                this->contacts.beginEdit()->push_back(c);
            }
        }
    }
    this->contacts.endEdit();
}

template<class DataTypes>
void ConicalForceField<DataTypes>::addDForce(VecDeriv& df1, const VecDeriv& dx1)
{
    df1.resize(dx1.size());
    for (unsigned int i=0; i<this->contacts.getValue().size(); i++)
    {
        const Contact& c = (*this->contacts.beginEdit())[i];
        assert((unsigned)c.index<dx1.size());
        Deriv du = dx1[c.index];
        Deriv dforce; dforce = -this->stiffness.getValue()*(c.normal * ((du*c.normal)));
        df1[c.index] += dforce;
    }
    this->contacts.endEdit();
}

template<class DataTypes>
void ConicalForceField<DataTypes>::updateStiffness( const VecCoord&  )
{
    std::cerr<<"SphereForceField::updateStiffness-not-implemented !!!"<<std::endl;
}

template <class DataTypes>
sofa::defaulttype::Vector3::value_type ConicalForceField<DataTypes>::getPotentialEnergy(const VecCoord&)
{
    std::cerr<<"ConicalForceField::getPotentialEnergy-not-implemented !!!"<<std::endl;
    return 0;
}

template<class DataTypes>
void ConicalForceField<DataTypes>::draw()
{
    if (!getContext()->getShowForceFields()) return;
    if (!bDraw.getValue()) return;

    const Real a = coneAngle.getValue();
    Coord height = coneHeight.getValue();
    const Real h = sqrt(pow(coneHeight.getValue()[0],2) + pow(coneHeight.getValue()[1],2) +	pow(coneHeight.getValue()[2],2));
    const Real b = tan((a/180*3.14)) * h;
    const Coord c = coneCenter.getValue();
    Coord axis = height.cross(Coord(0,0,1));

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND) ;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
    glColor4f(color.getValue()[0],color.getValue()[1],color.getValue()[2], 0.5);

    glPushMatrix();
    glTranslated(c[0], c[1], c[2]);
    glRotated(acos(height*Coord(0,0,1)/h)*180/3.14 ,-axis[0], axis[1], axis[2]);
    glRotatef(180.0,1.0,0.0,0.0);
    glTranslated(0.0, 0.0, -h);

    glutSolidCone( b, h, 32, 16);
    glPopMatrix();

    /*    glPushMatrix();

        glColor3f(1.0,0.0,0.0);
        glPointSize (16.0);
        glBegin (GL_POINTS);
        for (unsigned int i = 0 ; i <contacts.getValue().size() ; i++)
    	{

    	glVertex3f(contacts.getValue()[i].pos[0], contacts.getValue()[i].pos[1],contacts.getValue()[i].pos[2]);
    	//glVertex3f(contacts.getValue()[i].pos[0]+contacts.getValue()[i].normal[0],
    	//	   contacts.getValue()[i].pos[1]+contacts.getValue()[i].normal[1],
    	//	   contacts.getValue()[i].pos[2]+contacts.getValue()[i].normal[2]);

    	}
    	glEnd();
        glPopMatrix();*/
    glDisable(GL_BLEND) ;
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
}

template<class DataTypes>
bool ConicalForceField<DataTypes>::isIn(Coord p)
{
    const Coord c = coneCenter.getValue();
    const Coord height = coneHeight.getValue();
    const Real h = sqrt(pow(height[0],2) + pow(height[1],2) + pow(height[2],2));
    const Real distP = sqrt(pow(p[0] - c[0], 2) + pow(p[1] - c[1], 2) + pow(p[2] - c[2], 2));

    if (distP > h)
    {
        return false;
    }
    Coord vecP;

    for(unsigned i=0 ; i<3 ; ++i)
        vecP[i]=p[i]-c[i];

    if ( (acos(vecP*height/(h*distP))*180/3.14) > coneAngle.getValue() )
    {

        return false;
    }
    return true;
}

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
