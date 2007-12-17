#ifndef SOFA_COMPONENT_FORCEFIELD_INTERACTION_ELLIPSOIDFORCEFIELD_INL
#define SOFA_COMPONENT_FORCEFIELD_INTERACTION_ELLIPSOIDFORCEFIELD_INL

#include <sofa/core/componentmodel/behavior/ForceField.inl>
#include "InteractionEllipsoidForceField.h"
#include <sofa/helper/system/config.h>
#include <sofa/helper/system/gl.h>
#include <sofa/helper/system/glut.h>
#include <sofa/helper/rmath.h>
#include <assert.h>
#include <iostream>

namespace sofa
{

namespace component
{

namespace interactionforcefield
{

// v = sqrt(x0�/r0�+x1�/r1�+x2�/r2�)-1
// dv/dxj = xj/rj� * 1/sqrt(x0�/r0�+x1�/r1�+x2�/r2�)

// f  = -stiffness * v * (dv/dp) / norm(dv/dp)

// fi = -stiffness * (sqrt(x0�/r0�+x1�/r1�+x2�/r2�)-1) * (xi/ri�) / sqrt(x0�/r0^4+x1�/r1^4+x2�/r2^4)

// dfi/dxj = -stiffness * [ d(sqrt(x0�/r0�+x1�/r1�+x2�/r2�)-1)/dxj *   (xi/ri�) / sqrt(x0�/r0^4+x1�/r1^4+x2�/r2^4)
//                          +  (sqrt(x0�/r0�+x1�/r1�+x2�/r2�)-1)     * d(xi/ri�)/dxj / sqrt(x0�/r0^4+x1�/r1^4+x2�/r2^4)
//                          +  (sqrt(x0�/r0�+x1�/r1�+x2�/r2�)-1)     *  (xi/ri�) * d(1/sqrt(x0�/r0^4+x1�/r1^4+x2�/r2^4))/dxj ]
// dfi/dxj = -stiffness * [ xj/rj� * 1/sqrt(x0�/r0�+x1�/r1�+x2�/r2�) * (xi/ri�) / sqrt(x0�/r0^4+x1�/r1^4+x2�/r2^4)
//                          +  (sqrt(x0�/r0�+x1�/r1�+x2�/r2�)-1)       * (i==j)/ri� / sqrt(x0�/r0^4+x1�/r1^4+x2�/r2^4)
//                          +  (sqrt(x0�/r0�+x1�/r1�+x2�/r2�)-1)       * (xi/ri�) * (-1/2*2xj/rj^4*1/(x0�/r0^4+x1�/r1^4+x2�/r2^4) ]
// dfi/dxj = -stiffness * [ xj/rj� * 1/sqrt(x0�/r0�+x1�/r1�+x2�/r2�) * (xi/ri�) / sqrt(x0�/r0^4+x1�/r1^4+x2�/r2^4)
//                          +  (sqrt(x0�/r0�+x1�/r1�+x2�/r2�)-1)       * (i==j)/ri� / sqrt(x0�/r0^4+x1�/r1^4+x2�/r2^4)
//                          +  (sqrt(x0�/r0�+x1�/r1�+x2�/r2�)-1)       * (xi/ri�) * (-xj/rj^4*1/(x0�/r0^4+x1�/r1^4+x2�/r2^4) ]

// dfi/dxj = -stiffness * [ (xj/rj�) * (xi/ri�) * 1/(sqrt(x0�/r0�+x1�/r1�+x2�/r2�) * sqrt(x0�/r0^4+x1�/r1^4+x2�/r2^4))
//                          +  v       * (i==j) / (ri�*sqrt(x0�/r0^4+x1�/r1^4+x2�/r2^4))
//                          +  v       * (xi/ri�) * (xj/rj�) * 1/(rj�*(x0�/r0^4+x1�/r1^4+x2�/r2^4) ]


template<class DataTypes1, class DataTypes2>
void InteractionEllipsoidForceField<DataTypes1, DataTypes2>::initCalcF()
{
    vars.r = this->vradius.getValue();
    vars.stiffness = this->stiffness.getValue();
    vars.stiffabs =  helper::rabs(vars.stiffness);
    for (int j=0; j<N; j++) vars.inv_r2[j] = 1/(vars.r[j]*vars.r[j]);

    //printf("\n **********************");
    //printf("\n vars.inv_r2 = %f %f %f", vars.inv_r2[0], vars.inv_r2[1], vars.inv_r2[2]);
}

template<class DataTypes1, class DataTypes2>
bool InteractionEllipsoidForceField<DataTypes1, DataTypes2>::calcF(const Coord1& p1, const Deriv1& v1, Deriv1 &f1, Mat& dfdx)
{
    Coord1 dp = p1 - vars.center;
    Real1 norm2 = 0;
    for (int j=0; j<N; j++) norm2 += (dp[j]*dp[j])*vars.inv_r2[j];
    //Real1 d = (norm2-1)*s2;
    if ((norm2-1)*vars.stiffness<0)
    {
        //printf("\n norm2 = %f", norm2);
        //printf("\n dp = %f %f %f   p1 = %f %f %f   vars.center = %f %f %f", dp[0], dp[1], dp[2],
        //	p1.x(),p1.y(), p1.z(), vars.center.x(), vars.center.y(), vars.center.z());

        Real1 norm = helper::rsqrt(norm2);
        Real1 v = norm-1;
        Deriv1 grad;
        for (int j=0; j<N; j++) grad[j] = dp[j]*vars.inv_r2[j];
        Real1 gnorm2 = grad.norm2();
        Real1 gnorm = helper::rsqrt(gnorm2);
        //grad /= gnorm; //.normalize();
        Real1 forceIntensity = -vars.stiffabs*v/gnorm;
        Real1 dampingIntensity = this->damping.getValue()*helper::rabs(v);
        Deriv1 force = grad*forceIntensity - v1*dampingIntensity;
        f1=force;
        Real1 fact1 = -vars.stiffabs / (norm * gnorm);
        Real1 fact2 = -vars.stiffabs*v / gnorm;
        Real1 fact3 = -vars.stiffabs*v / gnorm2;
        for (int ci = 0; ci < N; ++ci)
        {
            for (int cj = 0; cj < N; ++cj)
                dfdx[ci][cj] = grad[ci]*grad[cj] * (fact1 + fact3*vars.inv_r2[cj]);
            dfdx[ci][ci] += fact2*vars.inv_r2[ci];
        }
        return true;
    }
    else
        return false;
}

template<class DataTypes1, class DataTypes2>
void InteractionEllipsoidForceField<DataTypes1, DataTypes2>::addForce(VecDeriv1& f1, VecDeriv2& f2,
        const VecCoord1& p1, const VecCoord2& p2,
        const VecDeriv1& v1, const VecDeriv2& v2)
{

    // debug;
    X1 = p1;
    X2 = p2;

    vars.pos6D = p2[object2_dof_index.getValue()];
    Quat Cq = vars.pos6D.getOrientation();
    Vec3d Cx = (Coord1) vars.pos6D.getCenter();
    Deriv2 V6D = v2[object2_dof_index.getValue()];
    Vec3d Cv = (Vec3d) V6D.getVCenter();
    Cv.clear();

    ///on le garde pour addDForce///
    _orientation = Cq;
    //_orientation.clear();
    //Cq.clear();
    ////////////////////////////////


    if(_update_pos_relative)
    {
        //_update_pos_relative = false;
        //Vec3d OC = (Vec3d) center.getValue() - Cx;
        //OC = Cq.inverseRotate(OC);
        vars.center = (Vec3d) center.getValue(); // position locale du point
        //printf("\n OC : %f %f %f",OC.x(),OC.y(),OC.z());
        Cv.clear();
    }

    initCalcF();

    //const Real1 s2 = (stiff < 0 ? - stiff*stiff : stiff*stiff );
    sofa::helper::vector<Contact>* contacts = this->contacts.beginEdit();
    contacts->clear();
    f1.resize(p1.size());
    for (unsigned int i=0; i<p1.size(); i++)
    {
        Coord1 p1Xform = Cq.inverseRotate(p1[i] - Cx);

        Deriv1 v1Xform = Cq.inverseRotate(v1[i]);
        Deriv1 f1Xform;
        f1Xform.clear();
        Mat dfdx;
        if (calcF(p1Xform, v1Xform, f1Xform, dfdx) && !_update_pos_relative)
        {
            //printf("\n p1[%d] : %f %f %f p1Xform[%d] : %f %f %f",i, p1[i].x(), p1[i].y(), p1[i].z(),i, p1Xform.x(),p1Xform.y(),p1Xform.z());

            Contact c;
            c.pos = p1[i];
            c.index = i;
            c.m = dfdx;

            Vec3d contactForce =  Cq.rotate(f1Xform);
            f1[i]+=contactForce;
            f2[object2_dof_index.getValue()].getVCenter() -= contactForce;
            c.bras_levier = p1[i] - Cx;
            //f2[object2_dof_index.getValue()].getVOrientation() -= c.bras_levier.cross(contactForce);
            contacts->push_back(c);
            /*
            printf("\n contactForce : %f %f %f, bras_Levier : %f %f %f, f2 = %f %f %f - %f %f %f",
            	contactForce.x(), contactForce.y(), contactForce.z(),
            	c.bras_levier.x(), c.bras_levier.y(), c.bras_levier.z(),
            	f2[object2_dof_index.getValue()].getVCenter().x(), f2[object2_dof_index.getValue()].getVCenter().y(), f2[object2_dof_index.getValue()].getVCenter().z(),
            	f2[object2_dof_index.getValue()].getVOrientation().x(), f2[object2_dof_index.getValue()].getVOrientation().y(), f2[object2_dof_index.getValue()].getVOrientation().z());
            */

        }
    }
    /*
    	printf("\n f2 = %f %f %f - %f %f %f",
    		f2[object2_dof_index.getValue()].getVCenter().x(), f2[object2_dof_index.getValue()].getVCenter().y(), f2[object2_dof_index.getValue()].getVCenter().z(),
    		f2[object2_dof_index.getValue()].getVOrientation().x(), f2[object2_dof_index.getValue()].getVOrientation().y(), f2[object2_dof_index.getValue()].getVOrientation().z());
    */
    /*
    	printf("\n verify addForce2 : ");
    	addForce2(f1, f2, p1, p2, v1, v2);
    	printf("\n f2 = %f %f %f - %f %f %f",
    		f2[object2_dof_index.getValue()].getVCenter().x(), f2[object2_dof_index.getValue()].getVCenter().y(), f2[object2_dof_index.getValue()].getVCenter().z(),
    		f2[object2_dof_index.getValue()].getVOrientation().x(), f2[object2_dof_index.getValue()].getVOrientation().y(), f2[object2_dof_index.getValue()].getVOrientation().z());
    */


    if(_update_pos_relative)
    {
        f2[object2_dof_index.getValue()].getVCenter().clear();
        f2[object2_dof_index.getValue()].getVOrientation().clear();
        _update_pos_relative = false;
    }
    this->contacts.endEdit();
}

template<class DataTypes1, class DataTypes2>
void InteractionEllipsoidForceField<DataTypes1, DataTypes2>::addForce2(VecDeriv1& f1, VecDeriv2& f2,
        const VecCoord1& p1, const VecCoord2& p2,
        const VecDeriv1& v1, const VecDeriv2& /* v2 */)
{

    Quat Cq = p2[object2_dof_index.getValue()].getOrientation();
    Vec3d Cx = (Coord1) p2[object2_dof_index.getValue()].getCenter();

    f1.clear();
    f2.clear();
    f1.resize(p1.size());
    f2.resize(p2.size());
    for (unsigned int i=0; i<p1.size(); i++)
    {
        Coord1 p1Xform = Cq.inverseRotate(p1[i] - Cx);

        Deriv1 v1Xform = Cq.inverseRotate(v1[i]);
        Deriv1 f1Xform;
        f1Xform.clear();
        Mat dfdx;
        if (calcF(p1Xform, v1Xform, f1Xform, dfdx))
        {


            Vec3d contactForce =  Cq.rotate(f1Xform);
            Vec3d bras_levier;
            bras_levier = p1[i] - Cx;
            f1[i]+=contactForce;
            f2[object2_dof_index.getValue()].getVCenter() -= contactForce;
            f2[object2_dof_index.getValue()].getVOrientation() -= bras_levier.cross(contactForce);


        }
    }



}


template<class DataTypes1, class DataTypes2>
void InteractionEllipsoidForceField<DataTypes1, DataTypes2>::addDForce(VecDeriv1& df1, VecDeriv2& df2, const VecDeriv1& dx1, const VecDeriv2& dx2)
{

    df1.resize(dx1.size());
    df2.resize(dx2.size());
    const sofa::helper::vector<Contact>& contacts = this->contacts.getValue();
    //printf("\n");
    for (unsigned int i=0; i<contacts.size(); i++)
    {
        const Contact& c = contacts[i];
        assert((unsigned)c.index<dx1.size());
        Vec3d du;
        du = (Vec3d) dx1[c.index] - (Vec3d) dx2[object2_dof_index.getValue()].getVCenter(); //- c.bras_levier.cross(dx2[object2_dof_index.getValue()].getVOrientation());
        Deriv1 dforce = c.m * _orientation.inverseRotate(du);
        Vec3d DF = _orientation.rotate(dforce);
        df1[c.index] += DF;
        df2[object2_dof_index.getValue()].getVCenter()  -= DF;
        //df2[object2_dof_index.getValue()].getVOrientation()  -= c.bras_levier.cross(DF);
        //printf(" bras_levier[%d] = %f %f %f  - ", i, c.bras_levier.x(), c.bras_levier.y(), c.bras_levier.z());
    }


}

template <class DataTypes1, class DataTypes2>
double InteractionEllipsoidForceField<DataTypes1, DataTypes2>::getPotentialEnergy(const VecCoord1& /* x1 */, const VecCoord2& /* x2 */)
{
    std::cerr<<"InteractionEllipsoidForceField::getPotentialEnergy-not-implemented !!!"<<std::endl;
    return 0;
}

template<class DataTypes1, class DataTypes2>
void InteractionEllipsoidForceField<DataTypes1, DataTypes2>::draw()
{
    if (!getContext()->getShowForceFields()) return;
    if (!bDraw.getValue()) return;

    double cx1=0, cy1=0, cz1=0;
    double cx2=0, cy2=0, cz2=0;
    DataTypes1::get(cx1, cy1, cz1, vars.center );

    //cx1 -=vars.pos6D.getCenter()[0];
    //cy1 -=vars.pos6D.getCenter()[1];
    //cz1 -=vars.pos6D.getCenter()[2];

    cx2=vars.pos6D.getCenter()[0];
    cy2=vars.pos6D.getCenter()[1];
    cz2=vars.pos6D.getCenter()[2];


    double rx=1, ry=1, rz=1;
    DataTypes1::get(rx, ry, rz, vradius.getValue());
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(color.getValue()[0],color.getValue()[1],color.getValue()[2]);


    Quat q=vars.pos6D.getOrientation();
    GLdouble R[4][4];
    Quat q1=q.inverse();
    q1.buildRotationMatrix(R);

    glPushMatrix();

    glTranslated(cx2, cy2, cz2);
    glMultMatrixd( &(R[0][0]) );
    glTranslated(cx1, cy1, cz1);
    glScaled(rx, ry, (stiffness.getValue()>0?rz:-rz));

    glutSolidSphere(1,32,16);
    //glTranslated(-cx2, -cy2, -cz2);

    glPopMatrix();
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    const sofa::helper::vector<Contact>& contacts = this->contacts.getValue();
    for (unsigned int i=0; i<contacts.size(); i++)
    {
        glPointSize(10);
        glColor4f (1,0.5,0.5,1);
        glBegin (GL_POINTS);
        glVertex3d(contacts[i].pos[0],contacts[i].pos[1],contacts[i].pos[2] );
        glEnd();
    }

}


} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
