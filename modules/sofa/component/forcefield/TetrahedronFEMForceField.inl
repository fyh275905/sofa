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
#ifndef SOFA_COMPONENT_FORCEFIELD_TETRAHEDRONFEMFORCEFIELD_INL
#define SOFA_COMPONENT_FORCEFIELD_TETRAHEDRONFEMFORCEFIELD_INL

#include <sofa/core/componentmodel/behavior/ForceField.inl>
#include <sofa/component/forcefield/TetrahedronFEMForceField.h>
#include <sofa/component/topology/GridTopology.h>
#include <sofa/simulation/tree/Simulation.h>
#include <sofa/helper/PolarDecompose.h>
#include <sofa/helper/gl/template.h>
#include <assert.h>
#include <iostream>
#include <set>



using std::set;


namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;



//////////////////////////////////////////////////////////////////////
////////////////////  basic computation methods  /////////////////////
//////////////////////////////////////////////////////////////////////

template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::computeStrainDisplacement( StrainDisplacement &J, Coord a, Coord b, Coord c, Coord d )
{
    // shape functions matrix
    Mat<2, 3, Real> M;

    M[0][0] = b[1];
    M[0][1] = c[1];
    M[0][2] = d[1];
    M[1][0] = b[2];
    M[1][1] = c[2];
    M[1][2] = d[2];
    J[0][0] = J[1][3] = J[2][5]   = - peudo_determinant_for_coef( M );
    M[0][0] = b[0];
    M[0][1] = c[0];
    M[0][2] = d[0];
    J[0][3] = J[1][1] = J[2][4]   = peudo_determinant_for_coef( M );
    M[1][0] = b[1];
    M[1][1] = c[1];
    M[1][2] = d[1];
    J[0][5] = J[1][4] = J[2][2]   = - peudo_determinant_for_coef( M );

    M[0][0] = c[1];
    M[0][1] = d[1];
    M[0][2] = a[1];
    M[1][0] = c[2];
    M[1][1] = d[2];
    M[1][2] = a[2];
    J[3][0] = J[4][3] = J[5][5]   = peudo_determinant_for_coef( M );
    M[0][0] = c[0];
    M[0][1] = d[0];
    M[0][2] = a[0];
    J[3][3] = J[4][1] = J[5][4]   = - peudo_determinant_for_coef( M );
    M[1][0] = c[1];
    M[1][1] = d[1];
    M[1][2] = a[1];
    J[3][5] = J[4][4] = J[5][2]   = peudo_determinant_for_coef( M );

    M[0][0] = d[1];
    M[0][1] = a[1];
    M[0][2] = b[1];
    M[1][0] = d[2];
    M[1][1] = a[2];
    M[1][2] = b[2];
    J[6][0] = J[7][3] = J[8][5]   = - peudo_determinant_for_coef( M );
    M[0][0] = d[0];
    M[0][1] = a[0];
    M[0][2] = b[0];
    J[6][3] = J[7][1] = J[8][4]   = peudo_determinant_for_coef( M );
    M[1][0] = d[1];
    M[1][1] = a[1];
    M[1][2] = b[1];
    J[6][5] = J[7][4] = J[8][2]   = - peudo_determinant_for_coef( M );

    M[0][0] = a[1];
    M[0][1] = b[1];
    M[0][2] = c[1];
    M[1][0] = a[2];
    M[1][1] = b[2];
    M[1][2] = c[2];
    J[9][0] = J[10][3] = J[11][5]   = peudo_determinant_for_coef( M );
    M[0][0] = a[0];
    M[0][1] = b[0];
    M[0][2] = c[0];
    J[9][3] = J[10][1] = J[11][4]   = - peudo_determinant_for_coef( M );
    M[1][0] = a[1];
    M[1][1] = b[1];
    M[1][2] = c[1];
    J[9][5] = J[10][4] = J[11][2]   = peudo_determinant_for_coef( M );


    // 0
    /*
    J[0][1] = J[0][2] = J[0][4] = J[1][0] =  J[1][2] =  J[1][5] =  J[2][0] =  J[2][1] =  J[2][3]  = 0;
    J[3][1] = J[3][2] = J[3][4] = J[4][0] =  J[4][2] =  J[4][5] =  J[5][0] =  J[5][1] =  J[5][3]  = 0;
    J[6][1] = J[6][2] = J[6][4] = J[7][0] =  J[7][2] =  J[7][5] =  J[8][0] =  J[8][1] =  J[8][3]  = 0;
    J[9][1] = J[9][2] = J[9][4] = J[10][0] = J[10][2] = J[10][5] = J[11][0] = J[11][1] = J[11][3] = 0;
    */
    //m_deq( J, 1.2 ); //hack for stability ??
}

template<class DataTypes>
typename TetrahedronFEMForceField<DataTypes>::Real TetrahedronFEMForceField<DataTypes>::peudo_determinant_for_coef ( const Mat<2, 3, Real>&  M )
{
    return  M[0][1]*M[1][2] - M[1][1]*M[0][2] -  M[0][0]*M[1][2] + M[1][0]*M[0][2] + M[0][0]*M[1][1] - M[1][0]*M[0][1];
}

template<class DataTypes>
void TetrahedronFEMForceField<DataTypes>::computeStiffnessMatrix( StiffnessMatrix& S,StiffnessMatrix& SR,const MaterialStiffness &K, const StrainDisplacement &J, const Transformation& Rot )
{
    MatNoInit<6, 12, Real> Jt;
    Jt.transpose( J );

    MatNoInit<12, 12, Real> JKJt;
    JKJt = J*K*Jt;

    MatNoInit<12, 12, Real> RR,RRt;
    RR.clear();
    RRt.clear();
    for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
        {
            RR[i][j]=RR[i+3][j+3]=RR[i+6][j+6]=RR[i+9][j+9]=Rot[i][j];
            RRt[i][j]=RRt[i+3][j+3]=RRt[i+6][j+6]=RRt[i+9][j+9]=Rot[j][i];
        }

    S = RR*JKJt;
    SR = S*RRt;
}

template<class DataTypes>
void TetrahedronFEMForceField<DataTypes>::computeMaterialStiffness(int i, Index&a, Index&b, Index&c, Index&d)
{
    const VecReal& localStiffnessFactor = _localStiffnessFactor;
    const Real youngModulus = (localStiffnessFactor.empty() ? 1.0f : localStiffnessFactor[i*localStiffnessFactor.size()/_indexedElements->size()])*_youngModulus;
    const Real poissonRatio = _poissonRatio;

    _materialsStiffnesses[i][0][0] = _materialsStiffnesses[i][1][1] = _materialsStiffnesses[i][2][2] = 1;
    _materialsStiffnesses[i][0][1] = _materialsStiffnesses[i][0][2] = _materialsStiffnesses[i][1][0]
            = _materialsStiffnesses[i][1][2] = _materialsStiffnesses[i][2][0] =
                    _materialsStiffnesses[i][2][1] = poissonRatio/(1-poissonRatio);
    _materialsStiffnesses[i][0][3] = _materialsStiffnesses[i][0][4] = _materialsStiffnesses[i][0][5] = 0;
    _materialsStiffnesses[i][1][3] = _materialsStiffnesses[i][1][4] = _materialsStiffnesses[i][1][5] = 0;
    _materialsStiffnesses[i][2][3] = _materialsStiffnesses[i][2][4] = _materialsStiffnesses[i][2][5] = 0;
    _materialsStiffnesses[i][3][0] = _materialsStiffnesses[i][3][1] = _materialsStiffnesses[i][3][2] = _materialsStiffnesses[i][3][4] = _materialsStiffnesses[i][3][5] = 0;
    _materialsStiffnesses[i][4][0] = _materialsStiffnesses[i][4][1] = _materialsStiffnesses[i][4][2] = _materialsStiffnesses[i][4][3] = _materialsStiffnesses[i][4][5] = 0;
    _materialsStiffnesses[i][5][0] = _materialsStiffnesses[i][5][1] = _materialsStiffnesses[i][5][2] = _materialsStiffnesses[i][5][3] = _materialsStiffnesses[i][5][4] = 0;
    _materialsStiffnesses[i][3][3] = _materialsStiffnesses[i][4][4] = _materialsStiffnesses[i][5][5] = (1-2*poissonRatio)/(2*(1-poissonRatio));
    _materialsStiffnesses[i] *= (youngModulus*(1-poissonRatio))/((1+poissonRatio)*(1-2*poissonRatio));

    /*Real gamma = (youngModulus*poissonRatio) / ((1+poissonRatio)*(1-2*poissonRatio));
    Real 		mu2 = youngModulus / (1+poissonRatio);
    _materialsStiffnesses[i][0][3] = _materialsStiffnesses[i][0][4] =	_materialsStiffnesses[i][0][5] = 0;
    _materialsStiffnesses[i][1][3] = _materialsStiffnesses[i][1][4] =	_materialsStiffnesses[i][1][5] = 0;
    _materialsStiffnesses[i][2][3] = _materialsStiffnesses[i][2][4] =	_materialsStiffnesses[i][2][5] = 0;
    _materialsStiffnesses[i][3][0] = _materialsStiffnesses[i][3][1] = _materialsStiffnesses[i][3][2] = _materialsStiffnesses[i][3][4] =	_materialsStiffnesses[i][3][5] = 0;
    _materialsStiffnesses[i][4][0] = _materialsStiffnesses[i][4][1] = _materialsStiffnesses[i][4][2] = _materialsStiffnesses[i][4][3] =	_materialsStiffnesses[i][4][5] = 0;
    _materialsStiffnesses[i][5][0] = _materialsStiffnesses[i][5][1] = _materialsStiffnesses[i][5][2] = _materialsStiffnesses[i][5][3] =	_materialsStiffnesses[i][5][4] = 0;
    _materialsStiffnesses[i][0][0] = _materialsStiffnesses[i][1][1] = _materialsStiffnesses[i][2][2] = gamma+mu2;
    _materialsStiffnesses[i][0][1] = _materialsStiffnesses[i][0][2] = _materialsStiffnesses[i][1][0]
    			= _materialsStiffnesses[i][1][2] = _materialsStiffnesses[i][2][0] = _materialsStiffnesses[i][2][1] = gamma;
    _materialsStiffnesses[i][3][3] = _materialsStiffnesses[i][4][4] = _materialsStiffnesses[i][5][5] =	mu2;*/

    // divide by 36 times volumes of the element

    Coord A = _initialPoints[b] - _initialPoints[a];
    Coord B = _initialPoints[c] - _initialPoints[a];
    Coord C = _initialPoints[d] - _initialPoints[a];
    Coord AB = cross(A, B);
    Real volumes6 = fabs( dot( AB, C ) );
    if (volumes6<0)
    {
        serr << "ERROR: Negative volume for tetra "<<i<<" <"<<a<<','<<b<<','<<c<<','<<d<<"> = "<<volumes6/6<<sendl;
    }
    _materialsStiffnesses[i] /= volumes6*6; // 36*Volume in the formula
}

template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::computeForce( Displacement &F, const Displacement &Depl, const MaterialStiffness &K, const StrainDisplacement &J )
{

    // Unit of K = unit of youngModulus / unit of volume = Pa / m^3 = kg m^-4 s^-2
    // Unit of J = m^2
    // Unit of JKJt =  kg s^-2
    // Unit of displacement = m
    // Unit of force = kg m s^-2

#if 0
    F = J*(K*(J.multTranspose(Depl)));
#else
    /* We have these zeros
                                  K[0][3]   K[0][4]   K[0][5]
                                  K[1][3]   K[1][4]   K[1][5]
                                  K[2][3]   K[2][4]   K[2][5]
    K[3][0]   K[3][1]   K[3][2]             K[3][4]   K[3][5]
    K[4][0]   K[4][1]   K[4][2]   K[4][3]             K[4][5]
    K[5][0]   K[5][1]   K[5][2]   K[5][3]   K[5][4]


              J[0][1]   J[0][2]             J[0][4]
    J[1][0]             J[1][2]                       J[1][5]
    J[2][0]   J[2][1]             J[2][3]
              J[3][1]   J[3][2]             J[3][4]
    J[4][0]             J[4][2]                       J[4][5]
    J[5][0]   J[5][1]             J[5][3]
              J[6][1]   J[6][2]             J[6][4]
    J[7][0]             J[7][2]                       J[7][5]
    J[8][0]   J[8][1]             J[8][3]
              J[9][1]   J[9][2]             J[9][4]
    J[10][0]            J[10][2]                      J[10][5]
    J[11][0]  J[11][1]            J[11][3]
    */

    VecNoInit<6,Real> JtD;
    JtD[0] =   J[ 0][0]*Depl[ 0]+/*J[ 1][0]*Depl[ 1]+  J[ 2][0]*Depl[ 2]+*/
            J[ 3][0]*Depl[ 3]+/*J[ 4][0]*Depl[ 4]+  J[ 5][0]*Depl[ 5]+*/
            J[ 6][0]*Depl[ 6]+/*J[ 7][0]*Depl[ 7]+  J[ 8][0]*Depl[ 8]+*/
            J[ 9][0]*Depl[ 9] /*J[10][0]*Depl[10]+  J[11][0]*Depl[11]*/;
    JtD[1] = /*J[ 0][1]*Depl[ 0]+*/J[ 1][1]*Depl[ 1]+/*J[ 2][1]*Depl[ 2]+*/
            /*J[ 3][1]*Depl[ 3]+*/J[ 4][1]*Depl[ 4]+/*J[ 5][1]*Depl[ 5]+*/
            /*J[ 6][1]*Depl[ 6]+*/J[ 7][1]*Depl[ 7]+/*J[ 8][1]*Depl[ 8]+*/
            /*J[ 9][1]*Depl[ 9]+*/J[10][1]*Depl[10] /*J[11][1]*Depl[11]*/;
    JtD[2] = /*J[ 0][2]*Depl[ 0]+  J[ 1][2]*Depl[ 1]+*/J[ 2][2]*Depl[ 2]+
            /*J[ 3][2]*Depl[ 3]+  J[ 4][2]*Depl[ 4]+*/J[ 5][2]*Depl[ 5]+
            /*J[ 6][2]*Depl[ 6]+  J[ 7][2]*Depl[ 7]+*/J[ 8][2]*Depl[ 8]+
            /*J[ 9][2]*Depl[ 9]+  J[10][2]*Depl[10]+*/J[11][2]*Depl[11]  ;
    JtD[3] =   J[ 0][3]*Depl[ 0]+  J[ 1][3]*Depl[ 1]+/*J[ 2][3]*Depl[ 2]+*/
            J[ 3][3]*Depl[ 3]+  J[ 4][3]*Depl[ 4]+/*J[ 5][3]*Depl[ 5]+*/
            J[ 6][3]*Depl[ 6]+  J[ 7][3]*Depl[ 7]+/*J[ 8][3]*Depl[ 8]+*/
            J[ 9][3]*Depl[ 9]+  J[10][3]*Depl[10] /*J[11][3]*Depl[11]*/;
    JtD[4] = /*J[ 0][4]*Depl[ 0]+*/J[ 1][4]*Depl[ 1]+  J[ 2][4]*Depl[ 2]+
            /*J[ 3][4]*Depl[ 3]+*/J[ 4][4]*Depl[ 4]+  J[ 5][4]*Depl[ 5]+
            /*J[ 6][4]*Depl[ 6]+*/J[ 7][4]*Depl[ 7]+  J[ 8][4]*Depl[ 8]+
            /*J[ 9][4]*Depl[ 9]+*/J[10][4]*Depl[10]+  J[11][4]*Depl[11]  ;
    JtD[5] =   J[ 0][5]*Depl[ 0]+/*J[ 1][5]*Depl[ 1]*/ J[ 2][5]*Depl[ 2]+
            J[ 3][5]*Depl[ 3]+/*J[ 4][5]*Depl[ 4]*/ J[ 5][5]*Depl[ 5]+
            J[ 6][5]*Depl[ 6]+/*J[ 7][5]*Depl[ 7]*/ J[ 8][5]*Depl[ 8]+
            J[ 9][5]*Depl[ 9]+/*J[10][5]*Depl[10]*/ J[11][5]*Depl[11];
//         serr<<"TetrahedronFEMForceField<DataTypes>::computeForce, D = "<<Depl<<sendl;
//         serr<<"TetrahedronFEMForceField<DataTypes>::computeForce, JtD = "<<JtD<<sendl;

    VecNoInit<6,Real> KJtD;
    KJtD[0] =   K[0][0]*JtD[0]+  K[0][1]*JtD[1]+  K[0][2]*JtD[2]
            /*K[0][3]*JtD[3]+  K[0][4]*JtD[4]+  K[0][5]*JtD[5]*/;
    KJtD[1] =   K[1][0]*JtD[0]+  K[1][1]*JtD[1]+  K[1][2]*JtD[2]
            /*K[1][3]*JtD[3]+  K[1][4]*JtD[4]+  K[1][5]*JtD[5]*/;
    KJtD[2] =   K[2][0]*JtD[0]+  K[2][1]*JtD[1]+  K[2][2]*JtD[2]
            /*K[2][3]*JtD[3]+  K[2][4]*JtD[4]+  K[2][5]*JtD[5]*/;
    KJtD[3] = /*K[3][0]*JtD[0]+  K[3][1]*JtD[1]+  K[3][2]*JtD[2]+*/
        K[3][3]*JtD[3] /*K[3][4]*JtD[4]+  K[3][5]*JtD[5]*/;
    KJtD[4] = /*K[4][0]*JtD[0]+  K[4][1]*JtD[1]+  K[4][2]*JtD[2]+*/
        /*K[4][3]*JtD[3]+*/K[4][4]*JtD[4] /*K[4][5]*JtD[5]*/;
    KJtD[5] = /*K[5][0]*JtD[0]+  K[5][1]*JtD[1]+  K[5][2]*JtD[2]+*/
        /*K[5][3]*JtD[3]+  K[5][4]*JtD[4]+*/K[5][5]*JtD[5]  ;

    F[ 0] =   J[ 0][0]*KJtD[0]+/*J[ 0][1]*KJtD[1]+  J[ 0][2]*KJtD[2]+*/
            J[ 0][3]*KJtD[3]+/*J[ 0][4]*KJtD[4]+*/J[ 0][5]*KJtD[5]  ;
    F[ 1] = /*J[ 1][0]*KJtD[0]+*/J[ 1][1]*KJtD[1]+/*J[ 1][2]*KJtD[2]+*/
            J[ 1][3]*KJtD[3]+  J[ 1][4]*KJtD[4] /*J[ 1][5]*KJtD[5]*/;
    F[ 2] = /*J[ 2][0]*KJtD[0]+  J[ 2][1]*KJtD[1]+*/J[ 2][2]*KJtD[2]+
            /*J[ 2][3]*KJtD[3]+*/J[ 2][4]*KJtD[4]+  J[ 2][5]*KJtD[5]  ;
    F[ 3] =   J[ 3][0]*KJtD[0]+/*J[ 3][1]*KJtD[1]+  J[ 3][2]*KJtD[2]+*/
            J[ 3][3]*KJtD[3]+/*J[ 3][4]*KJtD[4]+*/J[ 3][5]*KJtD[5]  ;
    F[ 4] = /*J[ 4][0]*KJtD[0]+*/J[ 4][1]*KJtD[1]+/*J[ 4][2]*KJtD[2]+*/
            J[ 4][3]*KJtD[3]+  J[ 4][4]*KJtD[4] /*J[ 4][5]*KJtD[5]*/;
    F[ 5] = /*J[ 5][0]*KJtD[0]+  J[ 5][1]*KJtD[1]+*/J[ 5][2]*KJtD[2]+
            /*J[ 5][3]*KJtD[3]+*/J[ 5][4]*KJtD[4]+  J[ 5][5]*KJtD[5]  ;
    F[ 6] =   J[ 6][0]*KJtD[0]+/*J[ 6][1]*KJtD[1]+  J[ 6][2]*KJtD[2]+*/
            J[ 6][3]*KJtD[3]+/*J[ 6][4]*KJtD[4]+*/J[ 6][5]*KJtD[5]  ;
    F[ 7] = /*J[ 7][0]*KJtD[0]+*/J[ 7][1]*KJtD[1]+/*J[ 7][2]*KJtD[2]+*/
            J[ 7][3]*KJtD[3]+  J[ 7][4]*KJtD[4] /*J[ 7][5]*KJtD[5]*/;
    F[ 8] = /*J[ 8][0]*KJtD[0]+  J[ 8][1]*KJtD[1]+*/J[ 8][2]*KJtD[2]+
            /*J[ 8][3]*KJtD[3]+*/J[ 8][4]*KJtD[4]+  J[ 8][5]*KJtD[5]  ;
    F[ 9] =   J[ 9][0]*KJtD[0]+/*J[ 9][1]*KJtD[1]+  J[ 9][2]*KJtD[2]+*/
            J[ 9][3]*KJtD[3]+/*J[ 9][4]*KJtD[4]+*/J[ 9][5]*KJtD[5]  ;
    F[10] = /*J[10][0]*KJtD[0]+*/J[10][1]*KJtD[1]+/*J[10][2]*KJtD[2]+*/
            J[10][3]*KJtD[3]+  J[10][4]*KJtD[4] /*J[10][5]*KJtD[5]*/;
    F[11] = /*J[11][0]*KJtD[0]+  J[11][1]*KJtD[1]+*/J[11][2]*KJtD[2]+
            /*J[11][3]*KJtD[3]+*/J[11][4]*KJtD[4]+  J[11][5]*KJtD[5]  ;
#endif
}

template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::computeForce( Displacement &F, const Displacement &Depl, const MaterialStiffness &K, const StrainDisplacement &J, double fact )
{

    // Unit of K = unit of youngModulus / unit of volume = Pa / m^3 = kg m^-4 s^-2
    // Unit of J = m^2
    // Unit of JKJt =  kg s^-2
    // Unit of displacement = m
    // Unit of force = kg m s^-2

#if 0
    F = J*(K*(J.multTranspose(Depl)));
    F *= fact;
#else
    /* We have these zeros
                                  K[0][3]   K[0][4]   K[0][5]
                                  K[1][3]   K[1][4]   K[1][5]
                                  K[2][3]   K[2][4]   K[2][5]
    K[3][0]   K[3][1]   K[3][2]             K[3][4]   K[3][5]
    K[4][0]   K[4][1]   K[4][2]   K[4][3]             K[4][5]
    K[5][0]   K[5][1]   K[5][2]   K[5][3]   K[5][4]


              J[0][1]   J[0][2]             J[0][4]
    J[1][0]             J[1][2]                       J[1][5]
    J[2][0]   J[2][1]             J[2][3]
              J[3][1]   J[3][2]             J[3][4]
    J[4][0]             J[4][2]                       J[4][5]
    J[5][0]   J[5][1]             J[5][3]
              J[6][1]   J[6][2]             J[6][4]
    J[7][0]             J[7][2]                       J[7][5]
    J[8][0]   J[8][1]             J[8][3]
              J[9][1]   J[9][2]             J[9][4]
    J[10][0]            J[10][2]                      J[10][5]
    J[11][0]  J[11][1]            J[11][3]
    */

    VecNoInit<6,Real> JtD;
    JtD[0] =   J[ 0][0]*Depl[ 0]+/*J[ 1][0]*Depl[ 1]+  J[ 2][0]*Depl[ 2]+*/
            J[ 3][0]*Depl[ 3]+/*J[ 4][0]*Depl[ 4]+  J[ 5][0]*Depl[ 5]+*/
            J[ 6][0]*Depl[ 6]+/*J[ 7][0]*Depl[ 7]+  J[ 8][0]*Depl[ 8]+*/
            J[ 9][0]*Depl[ 9] /*J[10][0]*Depl[10]+  J[11][0]*Depl[11]*/;
    JtD[1] = /*J[ 0][1]*Depl[ 0]+*/J[ 1][1]*Depl[ 1]+/*J[ 2][1]*Depl[ 2]+*/
            /*J[ 3][1]*Depl[ 3]+*/J[ 4][1]*Depl[ 4]+/*J[ 5][1]*Depl[ 5]+*/
            /*J[ 6][1]*Depl[ 6]+*/J[ 7][1]*Depl[ 7]+/*J[ 8][1]*Depl[ 8]+*/
            /*J[ 9][1]*Depl[ 9]+*/J[10][1]*Depl[10] /*J[11][1]*Depl[11]*/;
    JtD[2] = /*J[ 0][2]*Depl[ 0]+  J[ 1][2]*Depl[ 1]+*/J[ 2][2]*Depl[ 2]+
            /*J[ 3][2]*Depl[ 3]+  J[ 4][2]*Depl[ 4]+*/J[ 5][2]*Depl[ 5]+
            /*J[ 6][2]*Depl[ 6]+  J[ 7][2]*Depl[ 7]+*/J[ 8][2]*Depl[ 8]+
            /*J[ 9][2]*Depl[ 9]+  J[10][2]*Depl[10]+*/J[11][2]*Depl[11]  ;
    JtD[3] =   J[ 0][3]*Depl[ 0]+  J[ 1][3]*Depl[ 1]+/*J[ 2][3]*Depl[ 2]+*/
            J[ 3][3]*Depl[ 3]+  J[ 4][3]*Depl[ 4]+/*J[ 5][3]*Depl[ 5]+*/
            J[ 6][3]*Depl[ 6]+  J[ 7][3]*Depl[ 7]+/*J[ 8][3]*Depl[ 8]+*/
            J[ 9][3]*Depl[ 9]+  J[10][3]*Depl[10] /*J[11][3]*Depl[11]*/;
    JtD[4] = /*J[ 0][4]*Depl[ 0]+*/J[ 1][4]*Depl[ 1]+  J[ 2][4]*Depl[ 2]+
            /*J[ 3][4]*Depl[ 3]+*/J[ 4][4]*Depl[ 4]+  J[ 5][4]*Depl[ 5]+
            /*J[ 6][4]*Depl[ 6]+*/J[ 7][4]*Depl[ 7]+  J[ 8][4]*Depl[ 8]+
            /*J[ 9][4]*Depl[ 9]+*/J[10][4]*Depl[10]+  J[11][4]*Depl[11]  ;
    JtD[5] =   J[ 0][5]*Depl[ 0]+/*J[ 1][5]*Depl[ 1]*/ J[ 2][5]*Depl[ 2]+
            J[ 3][5]*Depl[ 3]+/*J[ 4][5]*Depl[ 4]*/ J[ 5][5]*Depl[ 5]+
            J[ 6][5]*Depl[ 6]+/*J[ 7][5]*Depl[ 7]*/ J[ 8][5]*Depl[ 8]+
            J[ 9][5]*Depl[ 9]+/*J[10][5]*Depl[10]*/ J[11][5]*Depl[11];
//         serr<<"TetrahedronFEMForceField<DataTypes>::computeForce, D = "<<Depl<<sendl;
//         serr<<"TetrahedronFEMForceField<DataTypes>::computeForce, JtD = "<<JtD<<sendl;

    VecNoInit<6,Real> KJtD;
    KJtD[0] =   K[0][0]*JtD[0]+  K[0][1]*JtD[1]+  K[0][2]*JtD[2]
            /*K[0][3]*JtD[3]+  K[0][4]*JtD[4]+  K[0][5]*JtD[5]*/;
    KJtD[1] =   K[1][0]*JtD[0]+  K[1][1]*JtD[1]+  K[1][2]*JtD[2]
            /*K[1][3]*JtD[3]+  K[1][4]*JtD[4]+  K[1][5]*JtD[5]*/;
    KJtD[2] =   K[2][0]*JtD[0]+  K[2][1]*JtD[1]+  K[2][2]*JtD[2]
            /*K[2][3]*JtD[3]+  K[2][4]*JtD[4]+  K[2][5]*JtD[5]*/;
    KJtD[3] = /*K[3][0]*JtD[0]+  K[3][1]*JtD[1]+  K[3][2]*JtD[2]+*/
        K[3][3]*JtD[3] /*K[3][4]*JtD[4]+  K[3][5]*JtD[5]*/;
    KJtD[4] = /*K[4][0]*JtD[0]+  K[4][1]*JtD[1]+  K[4][2]*JtD[2]+*/
        /*K[4][3]*JtD[3]+*/K[4][4]*JtD[4] /*K[4][5]*JtD[5]*/;
    KJtD[5] = /*K[5][0]*JtD[0]+  K[5][1]*JtD[1]+  K[5][2]*JtD[2]+*/
        /*K[5][3]*JtD[3]+  K[5][4]*JtD[4]+*/K[5][5]*JtD[5]  ;

    KJtD *= fact;

    F[ 0] =   J[ 0][0]*KJtD[0]+/*J[ 0][1]*KJtD[1]+  J[ 0][2]*KJtD[2]+*/
            J[ 0][3]*KJtD[3]+/*J[ 0][4]*KJtD[4]+*/J[ 0][5]*KJtD[5]  ;
    F[ 1] = /*J[ 1][0]*KJtD[0]+*/J[ 1][1]*KJtD[1]+/*J[ 1][2]*KJtD[2]+*/
            J[ 1][3]*KJtD[3]+  J[ 1][4]*KJtD[4] /*J[ 1][5]*KJtD[5]*/;
    F[ 2] = /*J[ 2][0]*KJtD[0]+  J[ 2][1]*KJtD[1]+*/J[ 2][2]*KJtD[2]+
            /*J[ 2][3]*KJtD[3]+*/J[ 2][4]*KJtD[4]+  J[ 2][5]*KJtD[5]  ;
    F[ 3] =   J[ 3][0]*KJtD[0]+/*J[ 3][1]*KJtD[1]+  J[ 3][2]*KJtD[2]+*/
            J[ 3][3]*KJtD[3]+/*J[ 3][4]*KJtD[4]+*/J[ 3][5]*KJtD[5]  ;
    F[ 4] = /*J[ 4][0]*KJtD[0]+*/J[ 4][1]*KJtD[1]+/*J[ 4][2]*KJtD[2]+*/
            J[ 4][3]*KJtD[3]+  J[ 4][4]*KJtD[4] /*J[ 4][5]*KJtD[5]*/;
    F[ 5] = /*J[ 5][0]*KJtD[0]+  J[ 5][1]*KJtD[1]+*/J[ 5][2]*KJtD[2]+
            /*J[ 5][3]*KJtD[3]+*/J[ 5][4]*KJtD[4]+  J[ 5][5]*KJtD[5]  ;
    F[ 6] =   J[ 6][0]*KJtD[0]+/*J[ 6][1]*KJtD[1]+  J[ 6][2]*KJtD[2]+*/
            J[ 6][3]*KJtD[3]+/*J[ 6][4]*KJtD[4]+*/J[ 6][5]*KJtD[5]  ;
    F[ 7] = /*J[ 7][0]*KJtD[0]+*/J[ 7][1]*KJtD[1]+/*J[ 7][2]*KJtD[2]+*/
            J[ 7][3]*KJtD[3]+  J[ 7][4]*KJtD[4] /*J[ 7][5]*KJtD[5]*/;
    F[ 8] = /*J[ 8][0]*KJtD[0]+  J[ 8][1]*KJtD[1]+*/J[ 8][2]*KJtD[2]+
            /*J[ 8][3]*KJtD[3]+*/J[ 8][4]*KJtD[4]+  J[ 8][5]*KJtD[5]  ;
    F[ 9] =   J[ 9][0]*KJtD[0]+/*J[ 9][1]*KJtD[1]+  J[ 9][2]*KJtD[2]+*/
            J[ 9][3]*KJtD[3]+/*J[ 9][4]*KJtD[4]+*/J[ 9][5]*KJtD[5]  ;
    F[10] = /*J[10][0]*KJtD[0]+*/J[10][1]*KJtD[1]+/*J[10][2]*KJtD[2]+*/
            J[10][3]*KJtD[3]+  J[10][4]*KJtD[4] /*J[10][5]*KJtD[5]*/;
    F[11] = /*J[11][0]*KJtD[0]+  J[11][1]*KJtD[1]+*/J[11][2]*KJtD[2]+
            /*J[11][3]*KJtD[3]+*/J[11][4]*KJtD[4]+  J[11][5]*KJtD[5]  ;
#endif
}

//////////////////////////////////////////////////////////////////////
////////////////////  small displacements method  ////////////////////
//////////////////////////////////////////////////////////////////////

template<class DataTypes>
void TetrahedronFEMForceField<DataTypes>::initSmall(int i, Index&a, Index&b, Index&c, Index&d)
{
    computeStrainDisplacement( _strainDisplacements[i], _initialPoints[a], _initialPoints[b], _initialPoints[c], _initialPoints[d] );
}

template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::accumulateForceSmall( Vector& f, const Vector & p, typename VecElement::const_iterator elementIt, Index elementIndex )
{
    //serr<<"TetrahedronFEMForceField<DataTypes>::accumulateForceSmall"<<sendl;
    Element index = *elementIt;
    Index a = index[0];
    Index b = index[1];
    Index c = index[2];
    Index d = index[3];

    // displacements
    Displacement D;
    D[0] = 0;
    D[1] = 0;
    D[2] = 0;
    D[3] =  _initialPoints[b][0] - _initialPoints[a][0] - p[b][0]+p[a][0];
    D[4] =  _initialPoints[b][1] - _initialPoints[a][1] - p[b][1]+p[a][1];
    D[5] =  _initialPoints[b][2] - _initialPoints[a][2] - p[b][2]+p[a][2];
    D[6] =  _initialPoints[c][0] - _initialPoints[a][0] - p[c][0]+p[a][0];
    D[7] =  _initialPoints[c][1] - _initialPoints[a][1] - p[c][1]+p[a][1];
    D[8] =  _initialPoints[c][2] - _initialPoints[a][2] - p[c][2]+p[a][2];
    D[9] =  _initialPoints[d][0] - _initialPoints[a][0] - p[d][0]+p[a][0];
    D[10] = _initialPoints[d][1] - _initialPoints[a][1] - p[d][1]+p[a][1];
    D[11] = _initialPoints[d][2] - _initialPoints[a][2] - p[d][2]+p[a][2];
    /*        serr<<"TetrahedronFEMForceField<DataTypes>::accumulateForceSmall, displacement"<<D<<sendl;
            serr<<"TetrahedronFEMForceField<DataTypes>::accumulateForceSmall, straindisplacement"<<_strainDisplacements[elementIndex]<<sendl;
            serr<<"TetrahedronFEMForceField<DataTypes>::accumulateForceSmall, material"<<_materialsStiffnesses[elementIndex]<<sendl;*/

    // compute force on element
    Displacement F;

    if(!_assembling)
    {
        computeForce( F, D, _materialsStiffnesses[elementIndex], _strainDisplacements[elementIndex] );
        //serr<<"TetrahedronFEMForceField<DataTypes>::accumulateForceSmall, force"<<F<<sendl;
    }
    else
    {
        Transformation Rot;
        Rot[0][0]=Rot[1][1]=Rot[2][2]=1;
        Rot[0][1]=Rot[0][2]=0;
        Rot[1][0]=Rot[1][2]=0;
        Rot[2][0]=Rot[2][1]=0;


        StiffnessMatrix JKJt,tmp;
        computeStiffnessMatrix(JKJt,tmp,_materialsStiffnesses[elementIndex], _strainDisplacements[elementIndex],Rot);

        //erase the stiffness matrix at each time step
        if(elementIndex==0)
        {
            for(unsigned int i=0; i<_stiffnesses.size(); ++i)
            {
                _stiffnesses[i].resize(0);
            }
        }

        for(int i=0; i<12; ++i)
        {
            int row = index[i/3]*3+i%3;

            for(int j=0; j<12; ++j)
            {
                if(JKJt[i][j]!=0)
                {

                    int col = index[j/3]*3+j%3;
                    //serr<<row<<" "<<col<<sendl;

                    //typename CompressedValue::iterator result = _stiffnesses[row].find(col);


                    // search if the vertex is already take into account by another element
                    typename CompressedValue::iterator result = _stiffnesses[row].end();
                    for(typename CompressedValue::iterator it=_stiffnesses[row].begin(); it!=_stiffnesses[row].end()&&result==_stiffnesses[row].end(); ++it)
                    {
                        if( (*it).first == col )
                            result = it;
                    }

                    if( result==_stiffnesses[row].end() )
                        _stiffnesses[row].push_back( Col_Value(col,JKJt[i][j] )  );
                    else
                        (*result).second += JKJt[i][j];
                }
            }
        }

        /*for(unsigned int i=0;i<_stiffnesses.size();++i)
        	for(typename CompressedValue::iterator it=_stiffnesses[i].begin();it!=_stiffnesses[i].end();++it)
        		serr<<i<<" "<<(*it).first<<"   "<<(*it).second<<"   "<<JKJt[i][(*it).first]<<sendl;*/

        F = JKJt * D;
    }

    f[a] += Deriv( F[0], F[1], F[2] );
    f[b] += Deriv( F[3], F[4], F[5] );
    f[c] += Deriv( F[6], F[7], F[8] );
    f[d] += Deriv( F[9], F[10], F[11] );
}

template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::applyStiffnessSmall( Vector& f, const Vector& x, int i, Index a, Index b, Index c, Index d, double fact )
{
    Displacement X;

    X[0] = x[a][0];
    X[1] = x[a][1];
    X[2] = x[a][2];

    X[3] = x[b][0];
    X[4] = x[b][1];
    X[5] = x[b][2];

    X[6] = x[c][0];
    X[7] = x[c][1];
    X[8] = x[c][2];

    X[9] = x[d][0];
    X[10] = x[d][1];
    X[11] = x[d][2];

    Displacement F;
    computeForce( F, X, _materialsStiffnesses[i], _strainDisplacements[i], fact );

    f[a] += Deriv( -F[0], -F[1],  -F[2] );
    f[b] += Deriv( -F[3], -F[4],  -F[5] );
    f[c] += Deriv( -F[6], -F[7],  -F[8] );
    f[d] += Deriv( -F[9], -F[10], -F[11] );
}

//////////////////////////////////////////////////////////////////////
////////////////////  large displacements method  ////////////////////
//////////////////////////////////////////////////////////////////////

template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::computeRotationLarge( Transformation &r, const Vector &p, const Index &a, const Index &b, const Index &c)
{
    // first vector on first edge
    // second vector in the plane of the two first edges
    // third vector orthogonal to first and second

    Coord edgex = p[b]-p[a];
    edgex.normalize();

    Coord edgey = p[c]-p[a];
    edgey.normalize();

    Coord edgez = cross( edgex, edgey );
    edgez.normalize();

    edgey = cross( edgez, edgex );
    edgey.normalize();

    r[0][0] = edgex[0];
    r[0][1] = edgex[1];
    r[0][2] = edgex[2];
    r[1][0] = edgey[0];
    r[1][1] = edgey[1];
    r[1][2] = edgey[2];
    r[2][0] = edgez[0];
    r[2][1] = edgez[1];
    r[2][2] = edgez[2];
}

//HACK get rotation for fast contact handling with simplified compliance
template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::getRotation(Transformation& R, unsigned int nodeIdx)
{
    if (!_rotationIdx.empty())
    {
        Transformation R0t;
        R0t.transpose(_initialRotations[_rotationIdx[nodeIdx]]);
        R = _rotations[_rotationIdx[nodeIdx]] * R0t;
        //R = _rotations[_rotationIdx[nodeIdx]];
    }
    else
    {
        R[0][0] = 1.0 ; R[1][1] = 1.0 ; R[2][2] = 1.0 ;
        R[0][1] = 0.0 ; R[0][2] = 0.0 ;
        R[1][0] = 0.0 ; R[1][2] = 0.0 ;
        R[2][0] = 0.0 ; R[2][1] = 0.0 ;
    }
}

template<class DataTypes>
void TetrahedronFEMForceField<DataTypes>::initLarge(int i, Index&a, Index&b, Index&c, Index&d)
{
    // Rotation matrix (initial Tetrahedre/world)
    // first vector on first edge
    // second vector in the plane of the two first edges
    // third vector orthogonal to first and second
    Transformation R_0_1;
    computeRotationLarge( R_0_1, _initialPoints, a, b, c);
    _initialRotations[i].transpose(R_0_1);

    //save the element index as the node index
    _rotationIdx[a] = i;
    _rotationIdx[b] = i;
    _rotationIdx[c] = i;
    _rotationIdx[d] = i;

    _rotatedInitialElements[i][0] = R_0_1*_initialPoints[a];
    _rotatedInitialElements[i][1] = R_0_1*_initialPoints[b];
    _rotatedInitialElements[i][2] = R_0_1*_initialPoints[c];
    _rotatedInitialElements[i][3] = R_0_1*_initialPoints[d];

//	serr<<"a,b,c : "<<a<<" "<<b<<" "<<c<<sendl;
//	serr<<"_initialPoints : "<<_initialPoints<<sendl;
//	serr<<"R_0_1 large : "<<R_0_1<<sendl;

    _rotatedInitialElements[i][1] -= _rotatedInitialElements[i][0];
    _rotatedInitialElements[i][2] -= _rotatedInitialElements[i][0];
    _rotatedInitialElements[i][3] -= _rotatedInitialElements[i][0];
    _rotatedInitialElements[i][0] = Coord(0,0,0);

//	serr<<"_rotatedInitialElements : "<<_rotatedInitialElements<<sendl;

    computeStrainDisplacement( _strainDisplacements[i],_rotatedInitialElements[i][0], _rotatedInitialElements[i][1],_rotatedInitialElements[i][2],_rotatedInitialElements[i][3] );
}

template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::accumulateForceLarge( Vector& f, const Vector & p, typename VecElement::const_iterator elementIt, Index elementIndex )
{
    Element index = *elementIt;

    // Rotation matrix (deformed and displaced Tetrahedron/world)
    Transformation R_0_2;
    computeRotationLarge( R_0_2, p, index[0],index[1],index[2]);

    _rotations[elementIndex].transpose(R_0_2);
    //serr<<"R_0_2 large : "<<R_0_2<<sendl;

    // positions of the deformed and displaced Tetrahedron in its frame
    helper::fixed_array<Coord,4> deforme;
    for(int i=0; i<4; ++i)
        deforme[i] = R_0_2*p[index[i]];

    deforme[1][0] -= deforme[0][0];
    deforme[2][0] -= deforme[0][0];
    deforme[2][1] -= deforme[0][1];
    deforme[3] -= deforme[0];

    // displacement
    Displacement D;
    D[0] = 0;
    D[1] = 0;
    D[2] = 0;
    D[3] = _rotatedInitialElements[elementIndex][1][0] - deforme[1][0];
    D[4] = 0;
    D[5] = 0;
    D[6] = _rotatedInitialElements[elementIndex][2][0] - deforme[2][0];
    D[7] = _rotatedInitialElements[elementIndex][2][1] - deforme[2][1];
    D[8] = 0;
    D[9] = _rotatedInitialElements[elementIndex][3][0] - deforme[3][0];
    D[10] = _rotatedInitialElements[elementIndex][3][1] - deforme[3][1];
    D[11] =_rotatedInitialElements[elementIndex][3][2] - deforme[3][2];

    //serr<<"D : "<<D<<sendl;

    Displacement F;
    if(_updateStiffnessMatrix)
    {
        //serr<<"TetrahedronFEMForceField<DataTypes>::accumulateForceLarge, update stiffness matrix"<<sendl;
        _strainDisplacements[elementIndex][0][0]   = ( - deforme[2][1]*deforme[3][2] );
        _strainDisplacements[elementIndex][1][1] = ( deforme[2][0]*deforme[3][2] - deforme[1][0]*deforme[3][2] );
        _strainDisplacements[elementIndex][2][2]   = ( deforme[2][1]*deforme[3][0] - deforme[2][0]*deforme[3][1] + deforme[1][0]*deforme[3][1] - deforme[1][0]*deforme[2][1] );

        _strainDisplacements[elementIndex][3][0]   = ( deforme[2][1]*deforme[3][2] );
        _strainDisplacements[elementIndex][4][1]  = ( - deforme[2][0]*deforme[3][2] );
        _strainDisplacements[elementIndex][5][2]   = ( - deforme[2][1]*deforme[3][0] + deforme[2][0]*deforme[3][1] );

        _strainDisplacements[elementIndex][7][1]  = ( deforme[1][0]*deforme[3][2] );
        _strainDisplacements[elementIndex][8][2]   = ( - deforme[1][0]*deforme[3][1] );

        _strainDisplacements[elementIndex][11][2] = ( deforme[1][0]*deforme[2][1] );
    }

    if(!_assembling)
    {
        // compute force on element
        computeForce( F, D, _materialsStiffnesses[elementIndex], _strainDisplacements[elementIndex]);
        for(int i=0; i<12; i+=3)
            f[index[i/3]] += _rotations[elementIndex] * Deriv( F[i], F[i+1],  F[i+2] );

        //serr<<"p large : "<<p<<sendl;
        //serr<<"F large : "<<f<<sendl;
//		for(int i=0;i<12;i+=3)
//		{
//			Vec tmp;
//			v_eq_Ab( tmp, _rotations[elementIndex], Vec( F[i], F[i+1],  F[i+2] ) );
//			serr<<tmp<<"\t";
//		}
//		serr<<sendl;
    }
    else
    {
        _strainDisplacements[elementIndex][6][0] = 0;
        _strainDisplacements[elementIndex][9][0] = 0;
        _strainDisplacements[elementIndex][10][1] = 0;

        StiffnessMatrix RJKJt, RJKJtRt;
        computeStiffnessMatrix(RJKJt,RJKJtRt,_materialsStiffnesses[elementIndex], _strainDisplacements[elementIndex],_rotations[elementIndex]);


        //erase the stiffness matrix at each time step
        if(elementIndex==0)
        {
            for(unsigned int i=0; i<_stiffnesses.size(); ++i)
            {
                _stiffnesses[i].resize(0);
            }
        }

        for(int i=0; i<12; ++i)
        {
            int row = index[i/3]*3+i%3;

            for(int j=0; j<12; ++j)
            {
                int col = index[j/3]*3+j%3;

                // search if the vertex is already take into account by another element
                typename CompressedValue::iterator result = _stiffnesses[row].end();
                for(typename CompressedValue::iterator it=_stiffnesses[row].begin(); it!=_stiffnesses[row].end()&&result==_stiffnesses[row].end(); ++it)
                {
                    if( (*it).first == col )
                    {
                        result = it;
                    }
                }

                if( result==_stiffnesses[row].end() )
                {
                    _stiffnesses[row].push_back( Col_Value(col,RJKJtRt[i][j] )  );
                }
                else
                {
                    (*result).second += RJKJtRt[i][j];
                }
            }
        }

        F = RJKJt*D;

        for(int i=0; i<12; i+=3)
            f[index[i/3]] += Deriv( F[i], F[i+1],  F[i+2] );
    }
}

template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::applyStiffnessLarge( Vector& f, const Vector& x, int i, Index a, Index b, Index c, Index d, double fact )
{
    Transformation R_0_2;
    R_0_2.transpose(_rotations[i]);

    Displacement X;
    Coord x_2;

    x_2 = R_0_2*x[a];
    X[0] = x_2[0];
    X[1] = x_2[1];
    X[2] = x_2[2];

    x_2 = R_0_2*x[b];
    X[3] = x_2[0];
    X[4] = x_2[1];
    X[5] = x_2[2];

    x_2 = R_0_2*x[c];
    X[6] = x_2[0];
    X[7] = x_2[1];
    X[8] = x_2[2];

    x_2 = R_0_2*x[d];
    X[9] = x_2[0];
    X[10] = x_2[1];
    X[11] = x_2[2];

    Displacement F;

    //serr<<"X : "<<X<<sendl;

    computeForce( F, X, _materialsStiffnesses[i], _strainDisplacements[i], fact );

    //serr<<"F : "<<F<<sendl;

    f[a] += _rotations[i] * Deriv( -F[0], -F[1],  -F[2] );
    f[b] += _rotations[i] * Deriv( -F[3], -F[4],  -F[5] );
    f[c] += _rotations[i] * Deriv( -F[6], -F[7],  -F[8] );
    f[d] += _rotations[i] * Deriv( -F[9], -F[10], -F[11] );
}

//////////////////////////////////////////////////////////////////////
////////////////////  polar decomposition method  ////////////////////
//////////////////////////////////////////////////////////////////////

template<class DataTypes>
void TetrahedronFEMForceField<DataTypes>::initPolar(int i, Index& a, Index&b, Index&c, Index&d)
{
    Transformation A;
    A[0] = _initialPoints[b]-_initialPoints[a];
    A[1] = _initialPoints[c]-_initialPoints[a];
    A[2] = _initialPoints[d]-_initialPoints[a];
    _initialTransformation[i] = A;

    Transformation R_0_1;
    MatNoInit<3,3,Real> S;
    polar_decomp(A, R_0_1, S);

    _initialRotations[i].transpose( R_0_1);

    //save the element index as the node index
    _rotationIdx[a] = i;
    _rotationIdx[b] = i;
    _rotationIdx[c] = i;
    _rotationIdx[d] = i;

    _rotatedInitialElements[i][0] = R_0_1*_initialPoints[a];
    _rotatedInitialElements[i][1] = R_0_1*_initialPoints[b];
    _rotatedInitialElements[i][2] = R_0_1*_initialPoints[c];
    _rotatedInitialElements[i][3] = R_0_1*_initialPoints[d];

    computeStrainDisplacement( _strainDisplacements[i],_rotatedInitialElements[i][0], _rotatedInitialElements[i][1],_rotatedInitialElements[i][2],_rotatedInitialElements[i][3] );
}

template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::accumulateForcePolar( Vector& f, const Vector & p, typename VecElement::const_iterator elementIt, Index elementIndex )
{
    Element index = *elementIt;

    Transformation A;
    A[0] = p[index[1]]-p[index[0]];
    A[1] = p[index[2]]-p[index[0]];
    A[2] = p[index[3]]-p[index[0]];

    Transformation R_0_2;
    MatNoInit<3,3,Real> S;
    polar_decomp(A, R_0_2, S);

    _rotations[elementIndex].transpose( R_0_2 );

    // positions of the deformed and displaced Tetrahedre in its frame
    helper::fixed_array<Coord, 4>  deforme;
    for(int i=0; i<4; ++i)
        deforme[i] = R_0_2 * p[index[i]];

    // displacement
    Displacement D;
    D[0] = _rotatedInitialElements[elementIndex][0][0] - deforme[0][0];
    D[1] = _rotatedInitialElements[elementIndex][0][1] - deforme[0][1];
    D[2] = _rotatedInitialElements[elementIndex][0][2] - deforme[0][2];
    D[3] = _rotatedInitialElements[elementIndex][1][0] - deforme[1][0];
    D[4] = _rotatedInitialElements[elementIndex][1][1] - deforme[1][1];
    D[5] = _rotatedInitialElements[elementIndex][1][2] - deforme[1][2];
    D[6] = _rotatedInitialElements[elementIndex][2][0] - deforme[2][0];
    D[7] = _rotatedInitialElements[elementIndex][2][1] - deforme[2][1];
    D[8] = _rotatedInitialElements[elementIndex][2][2] - deforme[2][2];
    D[9] = _rotatedInitialElements[elementIndex][3][0] - deforme[3][0];
    D[10] = _rotatedInitialElements[elementIndex][3][1] - deforme[3][1];
    D[11] = _rotatedInitialElements[elementIndex][3][2] - deforme[3][2];
    //serr<<"D : "<<D<<sendl;

    Displacement F;
    if(_updateStiffnessMatrix)
    {
        // shape functions matrix
        computeStrainDisplacement( _strainDisplacements[elementIndex], deforme[0],deforme[1],deforme[2],deforme[3]  );
    }

    if(!_assembling)
    {
        computeForce( F, D, _materialsStiffnesses[elementIndex], _strainDisplacements[elementIndex] );
        for(int i=0; i<12; i+=3)
            f[index[i/3]] += _rotations[elementIndex] * Deriv( F[i], F[i+1],  F[i+2] );
    }
    else
    {
        serr << "TODO(TetrahedronFEMForceField): support for assembling system matrix when using polar method."<<sendl;
    }
}

template<class DataTypes>
inline void TetrahedronFEMForceField<DataTypes>::applyStiffnessPolar( Vector& f, const Vector& x, int i, Index a, Index b, Index c, Index d, double fact )
{
    Transformation R_0_2;
    R_0_2.transpose( _rotations[i] );

    Displacement X;
    //Coord x_2;

    //x_2 = R_0_2*x[a];
    X[0] = R_0_2[0][0] * x[a][0] + R_0_2[0][1] * x[a][1] + R_0_2[0][2] * x[a][2];//x_2[0];
    X[1] = R_0_2[1][0] * x[a][0] + R_0_2[1][1] * x[a][1] + R_0_2[1][2] * x[a][2];//x_2[1];
    X[2] = R_0_2[2][0] * x[a][0] + R_0_2[2][1] * x[a][1] + R_0_2[2][2] * x[a][2];//x_2[2];

    //x_2 = R_0_2*x[b];
    X[3] = R_0_2[0][0] * x[b][0] + R_0_2[0][1] * x[b][1] + R_0_2[0][2] * x[b][2];//x_2[0];
    X[4] = R_0_2[1][0] * x[b][0] + R_0_2[1][1] * x[b][1] + R_0_2[1][2] * x[b][2];//x_2[1];
    X[5] = R_0_2[2][0] * x[b][0] + R_0_2[2][1] * x[b][1] + R_0_2[2][2] * x[b][2];//x_2[2];

    //x_2 = R_0_2*x[c];
    X[6] = R_0_2[0][0] * x[c][0] + R_0_2[0][1] * x[c][1] + R_0_2[0][2] * x[c][2];//x_2[0];
    X[7] = R_0_2[1][0] * x[c][0] + R_0_2[1][1] * x[c][1] + R_0_2[1][2] * x[c][2];//x_2[1];
    X[8] = R_0_2[2][0] * x[c][0] + R_0_2[2][1] * x[c][1] + R_0_2[2][2] * x[c][2];//x_2[2];

    //x_2 = R_0_2*x[d];
    X[9] = R_0_2[0][0] * x[d][0] + R_0_2[0][1] * x[d][1] + R_0_2[0][2] * x[d][2];//x_2[0];
    X[10] = R_0_2[1][0] * x[d][0] + R_0_2[1][1] * x[d][1] + R_0_2[1][2] * x[d][2];//x_2[1];
    X[11] = R_0_2[2][0] * x[d][0] + R_0_2[2][1] * x[d][1] + R_0_2[2][2] * x[d][2];//x_2[2];

    Displacement F;

    //serr<<"X : "<<X<<sendl;

    computeForce( F, X, _materialsStiffnesses[i], _strainDisplacements[i], fact );

    //serr<<"F : "<<F<<sendl;

    f[a][0] -= _rotations[i][0][0] *  F[0] +  _rotations[i][0][1] * F[1] + _rotations[i][0][2] * F[2];
    f[a][1] -= _rotations[i][1][0] *  F[0] +  _rotations[i][1][1] * F[1] + _rotations[i][1][2] * F[2];
    f[a][2] -= _rotations[i][2][0] *  F[0] +  _rotations[i][2][1] * F[1] + _rotations[i][2][2] * F[2];

    f[b][0] -= _rotations[i][0][0] *  F[3] +  _rotations[i][0][1] * F[4] + _rotations[i][0][2] * F[5];
    f[b][1] -=  _rotations[i][1][0] *  F[3] +  _rotations[i][1][1] * F[4] + _rotations[i][1][2] * F[5];
    f[b][2] -= _rotations[i][2][0] *  F[3] +  _rotations[i][2][1] * F[4] + _rotations[i][2][2] * F[5];

    f[c][0] -= _rotations[i][0][0] *  F[6] +  _rotations[i][0][1] * F[7] + _rotations[i][0][2] * F[8];
    f[c][1] -= _rotations[i][1][0] *  F[6] +  _rotations[i][1][1] * F[7] + _rotations[i][1][2] * F[8];
    f[c][2] -= _rotations[i][2][0] *  F[6] +  _rotations[i][2][1] * F[7] + _rotations[i][2][2] * F[8];

    f[d][0] -= _rotations[i][0][0] *  F[9] +  _rotations[i][0][1] * F[10] + _rotations[i][0][2] * F[11];
    f[d][1]	-= _rotations[i][1][0] *  F[9] +  _rotations[i][1][1] * F[10] + _rotations[i][1][2] * F[11];
    f[d][2]	-= _rotations[i][2][0] *  F[9] +  _rotations[i][2][1] * F[10] + _rotations[i][2][2] * F[11];

}

//////////////////////////////////////////////////////////////////////
////////////////  generic main computations methods  /////////////////
//////////////////////////////////////////////////////////////////////

template<class DataTypes>
void TetrahedronFEMForceField<DataTypes>::parse(core::objectmodel::BaseObjectDescription* arg)
{
    this->core::componentmodel::behavior::ForceField<DataTypes>::parse(arg);
    this->setComputeGlobalMatrix(std::string(arg->getAttribute("computeGlobalMatrix","false"))=="true");
}

template <class DataTypes>
void TetrahedronFEMForceField<DataTypes>::init()
{
    f_initialPoints.beginEdit();
    f_poissonRatio.beginEdit();
    f_youngModulus.beginEdit();
    f_localStiffnessFactor.beginEdit();
    f_updateStiffnessMatrix.beginEdit();
    f_assembling.beginEdit();

    this->core::componentmodel::behavior::ForceField<DataTypes>::init();
    _mesh = this->getContext()->getMeshTopology();
#ifdef SOFA_NEW_HEXA
    if (_mesh==NULL || (_mesh->getTetras().empty() && _mesh->getNbHexas()<=0))
#else
    if (_mesh==NULL || (_mesh->getTetras().empty() && _mesh->getNbCubes()<=0))
#endif
    {
        serr << "ERROR(TetrahedronFEMForceField): object must have a tetrahedric BaseMeshTopology."<<sendl;
        return;
    }
    if (!_mesh->getTetras().empty())
    {
        _indexedElements = & (_mesh->getTetras());
    }
    else
    {
#ifdef SOFA_DEV
        _trimgrid = dynamic_cast<topology::FittedRegularGridTopology*>(_mesh);
#endif // SOFA_DEV
        core::componentmodel::topology::BaseMeshTopology::SeqTetras* tetras = new core::componentmodel::topology::BaseMeshTopology::SeqTetras;
#ifdef SOFA_NEW_HEXA
        int nbcubes = _mesh->getNbHexas();
#else
        int nbcubes = _mesh->getNbCubes();
#endif
        // These values are only correct if the mesh is a grid topology
        int nx = 2;
        int ny = 1;
        int nz = 1;
        {
            topology::GridTopology* grid = dynamic_cast<topology::GridTopology*>(_mesh);
            if (grid != NULL)
            {
                nx = grid->getNx()-1;
                ny = grid->getNy()-1;
                nz = grid->getNz()-1;
            }
        }

        // Tesselation of each cube into 6 tetrahedra
        tetras->reserve(nbcubes*6);
        for (int i=0; i<nbcubes; i++)
        {
            // if (flags && !flags->isCubeActive(i)) continue;
#ifdef SOFA_NEW_HEXA
            core::componentmodel::topology::BaseMeshTopology::Hexa c = _mesh->getHexa(i);
#define swap(a,b) { int t = a; a = b; b = t; }
            if (!((i%nx)&1))
            {
                // swap all points on the X edges
                swap(c[0],c[1]);
                swap(c[3],c[2]);
                swap(c[4],c[5]);
                swap(c[7],c[6]);
            }
            if (((i/nx)%ny)&1)
            {
                // swap all points on the Y edges
                swap(c[0],c[3]);
                swap(c[1],c[2]);
                swap(c[4],c[7]);
                swap(c[5],c[6]);
            }
            if ((i/(nx*ny))&1)
            {
                // swap all points on the Z edges
                swap(c[0],c[4]);
                swap(c[1],c[5]);
                swap(c[2],c[6]);
                swap(c[3],c[7]);
            }
#undef swap
            typedef core::componentmodel::topology::BaseMeshTopology::Tetra Tetra;
            tetras->push_back(Tetra(c[0],c[5],c[1],c[6]));
            tetras->push_back(Tetra(c[0],c[1],c[3],c[6]));
            tetras->push_back(Tetra(c[1],c[3],c[6],c[2]));
            tetras->push_back(Tetra(c[6],c[3],c[0],c[7]));
            tetras->push_back(Tetra(c[6],c[7],c[0],c[5]));
            tetras->push_back(Tetra(c[7],c[5],c[4],c[0]));
#else
            core::componentmodel::topology::BaseMeshTopology::Cube c = _mesh->getCube(i);
            int sym = 0;
            if (!((i%nx)&1)) sym+=1;
            if (((i/nx)%ny)&1) sym+=2;
            if ((i/(nx*ny))&1) sym+=4;
            typedef core::componentmodel::topology::BaseMeshTopology::Tetra Tetra;
            tetras->push_back(Tetra(c[0^sym],c[5^sym],c[1^sym],c[7^sym]));
            tetras->push_back(Tetra(c[0^sym],c[1^sym],c[2^sym],c[7^sym]));
            tetras->push_back(Tetra(c[1^sym],c[2^sym],c[7^sym],c[3^sym]));
            tetras->push_back(Tetra(c[7^sym],c[2^sym],c[0^sym],c[6^sym]));
            tetras->push_back(Tetra(c[7^sym],c[6^sym],c[0^sym],c[5^sym]));
            tetras->push_back(Tetra(c[6^sym],c[5^sym],c[4^sym],c[0^sym]));
#endif
        }

        /*
        // Tesselation of each cube into 5 tetrahedra
        tetras->reserve(nbcubes*5);
        for (int i=0;i<nbcubes;i++)
        {
        	MeshTopology::Cube c = _mesh->getCube(i);
        	int sym = 0;
        	if ((i%nx)&1) sym+=1;
        	if (((i/nx)%ny)&1) sym+=2;
        	if ((i/(nx*ny))&1) sym+=4;
        	tetras->push_back(make_array(c[1^sym],c[0^sym],c[3^sym],c[5^sym]));
        	tetras->push_back(make_array(c[2^sym],c[3^sym],c[0^sym],c[6^sym]));
        	tetras->push_back(make_array(c[4^sym],c[5^sym],c[6^sym],c[0^sym]));
        	tetras->push_back(make_array(c[7^sym],c[6^sym],c[5^sym],c[3^sym]));
        	tetras->push_back(make_array(c[0^sym],c[3^sym],c[5^sym],c[6^sym]));
        }
        */
        _indexedElements = tetras;
    }
    /*if (_mesh->hasPos())
    { // use positions from topology
    VecCoord& p = *f_initialPoints.beginEdit();
        p.resize(_mesh->getNbPoints());
        for (unsigned int i=0;i<p.size();i++)
        {
            p[i] = Coord((Real)_mesh->getPX(i),(Real)_mesh->getPY(i),(Real)_mesh->getPZ(i));
        }
    }
    else
    {
    if (f_initialPoints.getValue().size() == 0)
    {
          VecCoord& p = *this->mstate->getX0();
          (*f_initialPoints.beginEdit()) = p;
        }
    }*/

    reinit(); // compute per-element stiffness matrices and other precomputed values

//     sout << "TetrahedronFEMForceField: init OK, "<<_indexedElements->size()<<" tetra."<<sendl;
}


template <class DataTypes>
void TetrahedronFEMForceField<DataTypes>::reinit()
{
    if (!this->mstate) return;
    setMethod(f_method.getValue() );
    VecCoord& p = *this->mstate->getX0();
    (*f_initialPoints.beginEdit()) = p;
    _strainDisplacements.resize( _indexedElements->size() );
    _materialsStiffnesses.resize(_indexedElements->size() );
    if(_assembling)
    {
        _stiffnesses.resize( _initialPoints.size()*3 );
    }

    unsigned int i;
    typename VecElement::const_iterator it;
    switch(method)
    {
    case SMALL :
    {
        for(it = _indexedElements->begin(), i = 0 ; it != _indexedElements->end() ; ++it, ++i)
        {
            Index a = (*it)[0];
            Index b = (*it)[1];
            Index c = (*it)[2];
            Index d = (*it)[3];
            this->computeMaterialStiffness(i,a,b,c,d);
            this->initSmall(i,a,b,c,d);
        }
        break;
    }
    case LARGE :
    {
        _rotations.resize( _indexedElements->size() );
        _initialRotations.resize( _indexedElements->size() );
        _rotationIdx.resize(_indexedElements->size() *4);
        _rotatedInitialElements.resize(_indexedElements->size());
        for(it = _indexedElements->begin(), i = 0 ; it != _indexedElements->end() ; ++it, ++i)
        {
            Index a = (*it)[0];
            Index b = (*it)[1];
            Index c = (*it)[2];
            Index d = (*it)[3];
            computeMaterialStiffness(i,a,b,c,d);
            initLarge(i,a,b,c,d);
        }
        break;
    }
    case POLAR :
    {
        _rotations.resize( _indexedElements->size() );
        _initialRotations.resize( _indexedElements->size() );
        _rotationIdx.resize(_indexedElements->size() *4);
        _rotatedInitialElements.resize(_indexedElements->size());
        _initialTransformation.resize(_indexedElements->size());
        unsigned int i=0;
        typename VecElement::const_iterator it;
        for(it = _indexedElements->begin(), i = 0 ; it != _indexedElements->end() ; ++it, ++i)
        {
            Index a = (*it)[0];
            Index b = (*it)[1];
            Index c = (*it)[2];
            Index d = (*it)[3];
            computeMaterialStiffness(i,a,b,c,d);
            initPolar(i,a,b,c,d);
        }
        break;
    }
    }
}


template<class DataTypes>
void TetrahedronFEMForceField<DataTypes>::addForce (VecDeriv& f, const VecCoord& p, const VecDeriv& /*v*/)
{
    f.resize(p.size());

    if (needUpdateTopology)
    {
        reinit();
        needUpdateTopology = false;
    }

    unsigned int i;
    typename VecElement::const_iterator it;
    switch(method)
    {
    case SMALL :
    {
        for(it=_indexedElements->begin(), i = 0 ; it!=_indexedElements->end(); ++it,++i)
        {
#ifdef SOFA_DEV
            if (_trimgrid && !_trimgrid->isCubeActive(i/6)) continue;
#endif // SOFA_DEV
            accumulateForceSmall( f, p, it, i );
        }
        break;
    }
    case LARGE :
    {
        for(it=_indexedElements->begin(), i = 0 ; it!=_indexedElements->end(); ++it,++i)
        {

#ifdef SOFA_DEV
            if (_trimgrid && !_trimgrid->isCubeActive(i/6)) continue;
#endif // SOFA_DEV
            accumulateForceLarge( f, p, it, i );
        }
        break;
    }
    case POLAR :
    {
        for(it=_indexedElements->begin(), i = 0 ; it!=_indexedElements->end(); ++it,++i)
        {
#ifdef SOFA_DEV
            if (_trimgrid && !_trimgrid->isCubeActive(i/6)) continue;
#endif // SOFA_DEV
            accumulateForcePolar( f, p, it, i );
        }
        break;
    }
    }
}

template<class DataTypes>
void TetrahedronFEMForceField<DataTypes>::addDForce (VecDeriv& v, const VecDeriv& x, double kFactor, double /*bFactor*/)
{
    v.resize(x.size());
    unsigned int i;
    typename VecElement::const_iterator it;

    switch(method)
    {
    case SMALL :
    {
        for(it = _indexedElements->begin(), i = 0 ; it != _indexedElements->end() ; ++it, ++i)
        {
#ifdef SOFA_DEV
            if (_trimgrid && !_trimgrid->isCubeActive(i/6)) continue;
#endif // SOFA_DEV
            Index a = (*it)[0];
            Index b = (*it)[1];
            Index c = (*it)[2];
            Index d = (*it)[3];

            applyStiffnessSmall( v,x, i, a,b,c,d, kFactor );
        }
        break;
    }
    case LARGE :
    {
        for(it = _indexedElements->begin(), i = 0 ; it != _indexedElements->end() ; ++it, ++i)
        {
#ifdef SOFA_DEV
            if (_trimgrid && !_trimgrid->isCubeActive(i/6)) continue;
#endif // SOFA_DEV
            Index a = (*it)[0];
            Index b = (*it)[1];
            Index c = (*it)[2];
            Index d = (*it)[3];

            applyStiffnessLarge( v,x, i, a,b,c,d, kFactor );
        }

        break;
    }
    case POLAR :
    {
        for(it = _indexedElements->begin(), i = 0 ; it != _indexedElements->end() ; ++it, ++i)
        {
#ifdef SOFA_DEV
            if (_trimgrid && !_trimgrid->isCubeActive(i/6)) continue;
#endif // SOFA_DEV
            Index a = (*it)[0];
            Index b = (*it)[1];
            Index c = (*it)[2];
            Index d = (*it)[3];

            applyStiffnessPolar( v,x, i, a,b,c,d, kFactor );
        }
        break;
    }
    }
}

template <class DataTypes>
double TetrahedronFEMForceField<DataTypes>::getPotentialEnergy(const VecCoord&)
{
    serr<<"TetrahedronFEMForceField::getPotentialEnergy-not-implemented !!!"<<sendl;
    return 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<class DataTypes>
void TetrahedronFEMForceField<DataTypes>::draw()
{
    if (!getContext()->getShowForceFields()) return;
    if (!this->mstate) return;

    const VecCoord& x = *this->mstate->getX();

    if (getContext()->getShowWireFrame())
        simulation::tree::getSimulation()->DrawUtility.setPolygonMode(0,true);


    std::vector< Vector3 > points[4];
    typename VecElement::const_iterator it;
    int i;
    for(it = _indexedElements->begin(), i = 0 ; it != _indexedElements->end() ; ++it, ++i)
    {
#ifdef SOFA_DEV
        if (_trimgrid && !_trimgrid->isCubeActive(i/6)) continue;
#endif // SOFA_DEV
        Index a = (*it)[0];
        Index b = (*it)[1];
        Index c = (*it)[2];
        Index d = (*it)[3];
        Coord center = (x[a]+x[b]+x[c]+x[d])*0.125;
        Coord pa = (x[a]+center)*(Real)0.666667;
        Coord pb = (x[b]+center)*(Real)0.666667;
        Coord pc = (x[c]+center)*(Real)0.666667;
        Coord pd = (x[d]+center)*(Real)0.666667;

// 		glColor4f(0,0,1,1);
        points[0].push_back(pa);
        points[0].push_back(pb);
        points[0].push_back(pc);

// 		glColor4f(0,0.5,1,1);
        points[1].push_back(pb);
        points[1].push_back(pc);
        points[1].push_back(pd);

// 		glColor4f(0,1,1,1);
        points[2].push_back(pc);
        points[2].push_back(pd);
        points[2].push_back(pa);

// 		glColor4f(0.5,1,1,1);
        points[3].push_back(pd);
        points[3].push_back(pa);
        points[3].push_back(pb);
    }

    simulation::tree::getSimulation()->DrawUtility.setLightingEnabled(false);
    simulation::tree::getSimulation()->DrawUtility.drawTriangles(points[0], Vec<4,float>(0.0,0.0,1.0,1.0));
    simulation::tree::getSimulation()->DrawUtility.drawTriangles(points[1], Vec<4,float>(0.0,0.5,1.0,1.0));
    simulation::tree::getSimulation()->DrawUtility.drawTriangles(points[2], Vec<4,float>(0.0,1.0,1.0,1.0));
    simulation::tree::getSimulation()->DrawUtility.drawTriangles(points[3], Vec<4,float>(0.5,1.0,1.0,1.0));
    simulation::tree::getSimulation()->DrawUtility.setLightingEnabled(true);

    if (getContext()->getShowWireFrame())
        simulation::tree::getSimulation()->DrawUtility.setPolygonMode(0,false);

    ////////////// AFFICHAGE DES ROTATIONS ////////////////////////
    if (getContext()->getShowNormals())
    {

        std::vector< Vector3 > points[3];

        for(unsigned ii = 0; ii<  x.size() ; ii++)
        {
            Coord a = x[ii];
            Transformation R;
            getRotation(R, ii);
            //R.transpose();
            Deriv v;
            // x
// 		glColor4f(1,0,0,1);
            v.x() =1.0; v.y()=0.0; v.z()=0.0;
            Coord b = a + R*v;
            points[0].push_back(a);
            points[0].push_back(b);
            // y
// 		glColor4f(0,1,0,1);
            v.x() =0.0; v.y()=1.0; v.z()=0.0;
            b = a + R*v;
            points[1].push_back(a);
            points[1].push_back(b);
            // z
// 		glColor4f(0,0,1,1);
            v.x() =0.0; v.y()=0.0; v.z()=1.0;
            b = a + R*v;
            points[2].push_back(a);
            points[2].push_back(b);
        }

        simulation::tree::getSimulation()->DrawUtility.setLightingEnabled(false);
        simulation::tree::getSimulation()->DrawUtility.drawLines(points[0], 5, Vec<4,float>(1,0,0,1));
        simulation::tree::getSimulation()->DrawUtility.drawLines(points[1], 5, Vec<4,float>(0,1,0,1));
        simulation::tree::getSimulation()->DrawUtility.drawLines(points[2], 5, Vec<4,float>(0,0,1,1));
        simulation::tree::getSimulation()->DrawUtility.setLightingEnabled(true);

    }
}


template<class DataTypes>
void TetrahedronFEMForceField<DataTypes>::addKToMatrix(sofa::defaulttype::BaseMatrix *mat, SReal k, unsigned int &offset)
{
    // Build Matrix Block for this ForceField
    int i,j,n1, n2, row, column, ROW, COLUMN , IT;

    Transformation Rot;
    StiffnessMatrix JKJt,tmp;

    typename VecElement::const_iterator it;

    Index noeud1, noeud2;

    Rot[0][0]=Rot[1][1]=Rot[2][2]=1;
    Rot[0][1]=Rot[0][2]=0;
    Rot[1][0]=Rot[1][2]=0;
    Rot[2][0]=Rot[2][1]=0;

    for(it = _indexedElements->begin(), IT=0 ; it != _indexedElements->end() ; ++it,++IT)
    {
        if (method == SMALL)
            computeStiffnessMatrix(JKJt,tmp,_materialsStiffnesses[IT], _strainDisplacements[IT],Rot);
        else
            computeStiffnessMatrix(JKJt,tmp,_materialsStiffnesses[IT], _strainDisplacements[IT],_rotations[IT]);

        // find index of node 1
        for (n1=0; n1<4; n1++)
        {
            noeud1 = (*it)[n1];

            for(i=0; i<3; i++)
            {
                ROW = offset+3*noeud1+i;
                row = 3*n1+i;
                // find index of node 2
                for (n2=0; n2<4; n2++)
                {
                    noeud2 = (*it)[n2];

                    for (j=0; j<3; j++)
                    {
                        COLUMN = offset+3*noeud2+j;
                        column = 3*n2+j;
                        mat->add(ROW, COLUMN, - tmp[row][column]*k);
                    }
                }
            }
        }
    }
}

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
