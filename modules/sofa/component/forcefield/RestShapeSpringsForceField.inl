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
#ifndef SOFA_COMPONENT_INTERACTIONFORCEFIELD_ConstantForceField_INL
#define SOFA_COMPONENT_INTERACTIONFORCEFIELD_ConstantForceField_INL

#include <sofa/core/componentmodel/behavior/ForceField.inl>
#include "RestShapeSpringsForceField.h"
#include <sofa/helper/system/config.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/helper/gl/template.h>
#include <assert.h>
#include <iostream>



namespace sofa
{

namespace component
{

namespace forcefield
{

template<class DataTypes>
RestShapeSpringsForceField<DataTypes>::RestShapeSpringsForceField()
    : points(initData(&points, "points", "points where the forces are applied"))
    , stiffness(initData(&stiffness, "stiffness", "stiffness values between the actual position and the rest shape position"))
{}


template<class DataTypes>
void RestShapeSpringsForceField<DataTypes>::init()
{

    core::componentmodel::behavior::ForceField<DataTypes>::init();

    if (points.getValue().size()==0)
    {
        VecIndex indices; // = points.getValue();
        std::cout<<"in RestShapeSpringsForceField no point is defined, default case: points = all points "<<std::endl;

        for (unsigned int i=0; i<(unsigned)this->mstate->getSize(); i++)
        {
            //int &toto = i;
            indices.push_back(i);
        }
        points.setValue(indices);

    }
    if(stiffness.getValue()s.size() == 0)
    {
        VecReal stiffs;
        stiffs.push_back(100.0);
        std::cout<<"in RestShapeSpringsForceField no stiffness is defined, assuming equal stiffness on each node, k = 100.0 "<<std::endl;
        stiffness.setValue(stiffs);
    }

}

template<class DataTypes>
void RestShapeSpringsForceField<DataTypes>::addForce(VecDeriv& f, const VecCoord& p, const VecDeriv& )
{
    const VecCoord& p_0 = *this->mstate->getX0();

    f.resize(p.size());

    const VecIndex& indices = points.getValue();
    const VecReal& k = stiffness.getValue();

    if ( k.size()!= indices.size() )
    {
        sout << "WARNING : stiffness is not defined on each point, first stiffness is used" << sendl;

        for (unsigned int i=0; i<indices.size(); i++)
        {
            const unsigned int index = indices[i];

            Deriv dx = p[index] - p_0[index];
            f[index] -=  dx * k[0] ;

            //	Deriv dx = p[i] - p_0[i];
            //	f[ indices[i] ] -=  dx * k[0] ;
        }
    }
    else
    {
        for (unsigned int i=0; i<indices.size(); i++)
        {
            const unsigned int index = indices[i];

            Deriv dx = p[index] - p_0[index];
            f[index] -=  dx * k[index] ;

            //	Deriv dx = p[i] - p_0[i];
            //	f[ indices[i] ] -=  dx * k[i] ;
        }
    }
}


template<class DataTypes>
void RestShapeSpringsForceField<DataTypes>::addDForce(VecDeriv& df, const VecDeriv &dx, double kFactor, double )
{
    const VecIndex& indices = points.getValue();
    const VecReal& k = stiffness.getValue();

    if (k.size()!= indices.size() )
    {
        sout << "WARNING : stiffness is not defined on each point, first stiffness is used" << sendl;

        for (unsigned int i=0; i<indices.size(); i++)
        {
            df[indices[i]] -=  dx[indices[i]] * k[0] * kFactor;
        }
    }
    else
    {
        for (unsigned int i=0; i<indices.size(); i++)
        {
            //	df[ indices[i] ] -=  dx[indices[i]] * k[i] * kFactor ;
            df[indices[i]] -=  dx[indices[i]] * k[indices[i]] * kFactor ;
        }
    }
}


template<class DataTypes>
void RestShapeSpringsForceField<DataTypes>::addKToMatrix(sofa::defaulttype::BaseMatrix * mat, double kFact, unsigned int &offset)
{
    const VecIndex& indices = points.getValue();
    const VecReal& k = stiffness.getValue();
    const int N = Coord::static_size;

    unsigned int curIndex = 0;

    if (k.size()!= indices.size() )
    {
        for (unsigned int index = 0; index < indices.size(); index++)
        {
            curIndex = indices[index];

            for(int i = 0; i < N; i++)
            {

                //	for (unsigned int j = 0; j < N; j++)
                //	{
                //		mat->add(offset + N * curIndex + i, offset + N * curIndex + j, kFact * k[0]);
                //	}

                mat->add(offset + N * curIndex + i, offset + N * curIndex + i, kFact * k[0]);
            }
        }
    }
    else
    {
        for (unsigned int index = 0; index < indices.size(); index++)
        {
            curIndex = indices[index];

            for(int i = 0; i < N; i++)
            {

                //	for (unsigned int j = 0; j < N; j++)
                //	{
                //		mat->add(offset + N * curIndex + i, offset + N * curIndex + j, kFact * k[curIndex]);
                //	}

                mat->add(offset + N * curIndex + i, offset + N * curIndex + i, kFact * k[curIndex]);
            }
        }
    }
}


//template <class DataTypes>
//double RestShapeSpringsForceField<DataTypes>::getPotentialEnergy(const VecCoord& x)
//{
//	const VecIndex& indices = points.getValue();
//	const VecDeriv& f = forces.getValue();
//	double e=0;
//	unsigned int i = 0;
//	for (; i<f.size(); i++)
//	{
//		e -= f[i]*x[indices[i]];
//	}
//	for (; i<indices.size(); i++)
//	{
//		e -= f[f.size()-1]*x[indices[i]];
//	}
//	return e;
//}


//template <class DataTypes>
//void RestShapeSpringsForceField<DataTypes>::setForce( unsigned i, const Deriv& force )
//{
//	VecIndex& indices = *points.beginEdit();
//	VecDeriv& f = *forces.beginEdit();
//	indices.push_back(i);
//	f.push_back( force );
//	points.endEdit();
//	forces.endEdit();
//}


template<class DataTypes>
void RestShapeSpringsForceField<DataTypes>::draw()
{
    /*
    if (!getContext()->getShowForceFields())
    	return;  /// \todo put this in the parent class
    const VecIndex& indices = points.getValue();
    const VecDeriv& f = forces.getValue();
    const VecCoord& x = *this->mstate->getX();
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(0,1,0);
    for (unsigned int i=0; i<indices.size(); i++)
    {
    	Real xx,xy,xz,fx,fy,fz;
    	DataTypes::get(xx,xy,xz,x[indices[i]]);
    	DataTypes::get(fx,fy,fz,f[(i<f.size()) ? i : f.size()-1]);
    	glVertex3f( (GLfloat)xx, (GLfloat)xy, (GLfloat)xz );
    	glVertex3f( (GLfloat)(xx+fx), (GLfloat)(xy+fy), (GLfloat)(xz+fz) );
    }
    glEnd();
    */
}


template <class DataTypes>
bool RestShapeSpringsForceField<DataTypes>::addBBox(double*, double* )
{
    return false;
}


} // namespace forcefield

} // namespace component

} // namespace sofa

#endif



