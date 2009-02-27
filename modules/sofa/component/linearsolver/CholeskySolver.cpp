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
// Author: Hadrien Courtecuisse
//
// Copyright: See COPYING file that comes with this distribution
#include <sofa/component/linearsolver/CholeskySolver.h>
#include <sofa/component/linearsolver/NewMatMatrix.h>
#include <sofa/component/linearsolver/FullMatrix.h>
#include <sofa/component/linearsolver/SparseMatrix.h>
#include <sofa/core/ObjectFactory.h>
#include <iostream>
#include "sofa/helper/system/thread/CTime.h"
#include <sofa/core/objectmodel/BaseContext.h>
#include <sofa/core/componentmodel/behavior/LinearSolver.h>
#include <math.h>
#include <sofa/helper/system/thread/CTime.h>


namespace sofa
{

namespace component
{

namespace linearsolver
{

using namespace sofa::defaulttype;
using namespace sofa::core::componentmodel::behavior;
using namespace sofa::simulation;
using namespace sofa::core::objectmodel;
using sofa::helper::system::thread::CTime;
using sofa::helper::system::thread::ctime_t;
using std::cerr;
using std::endl;

template<class TMatrix, class TVector>
CholeskySolver<TMatrix,TVector>::CholeskySolver()
    : f_verbose( initData(&f_verbose,false,"verbose","Dump system state at each iteration") )
    , f_graph( initData(&f_graph,"graph","Graph of residuals at each iteration") )
{
    f_graph.setWidget("graph");
    f_graph.setReadOnly(true);
}


//Factorisation : A = LL^t
//A x = b <=> LL^t x = b
//        <=> L u = b , L^t x = u
template<class TMatrix, class TVector>
void CholeskySolver<TMatrix,TVector>::solve (Matrix& /*M*/, Vector& z, Vector& r)
{
    //Compute L
    int n = L.colSize();

    //Solve L u = b
    for (int j=0; j<n; j++)
    {
        double temp = 0.0;
        double d = 1.0 / L.element(j,j);
        for (int i=0; i<j; i++)
        {
            temp += z[i] * L.element(i,j);
        }
        z[j] = (r[j] - temp) * d ;
    }

    //Solve L^t x = u
    for (int j=n-1; j>=0; j--)
    {
        double temp = 0.0;
        double d = 1.0 / L.element(j,j);
        for (int i=j+1; i<n; i++)
        {
            temp += z[i] * L.element(j,i);
        }
        z[j] = (z[j] - temp) * d;
    }
}

template<class TMatrix, class TVector>
void CholeskySolver<TMatrix,TVector>::invert(Matrix& M)
{
    int n = M.colSize();
    double ss,d;

    L.resize(n,n);
    d = 1.0 / sqrt(M.element(0,0));
    for (int i=0; i<n; i++)
    {
        L.set(0,i,M.element(i,0) * d);
    }

    for (int j=1; j<n; j++)
    {
        ss=0;
        for (int k=0; k<j; k++) ss+=L.element(k,j)*L.element(k,j);

        d = 1.0 / sqrt(M.element(j,j)-ss);
        L.set(j,j,(M.element(j,j)-ss) * d);


        for (int i=j+1; i<n; i++)
        {
            ss=0;
            for (int k=0; k<j; k++) ss+=L.element(k,i)*L.element(k,j);
            L.set(j,i,(M.element(i,j)-ss) * d);
        }
    }
}

//1. void Cholesky(int n, float A[N][N], float L[N][N], float Lt[N][N])
//2. {
//3.     L[0][0]=sqrt(A[0][0]);
//4.     float somme;
//5.
//6.     for(int i=1;i<n;i++)
//7.     {
//8.         for(int j=0;j<i;j++)
//9.         {
//10.             somme=0;
//11.             for(int k=0;k<j;j++)
//12.             {
//13.                 somme=somme+L[i][k]*L[j][k];
//14.             }
//15.             L[i][j]=(A[i][j]-somme)/L[j][j];
//16.         }
//17.
//18.         somme=0;
//19.         for(int k=0;k<i;k++)
//20.         {
//21.             somme=somme+L[i][k]*L[i][k];
//22.         }
//23.         L[i][i]=sqrt(A[i][i]-somme);
//24.     }
//25. }
/*
template<class TMatrix, class TVector>
void CholeskySolver<TMatrix,TVector>::invert(Matrix& M) {
	int n = M.colSize();
	L.resize(n,n);
	double somme;

	L.set(0,0,sqrt(M.element(0,0)));

	for (int i=1; i<n; i++) {
		for (int j=0; j<i; j++) {
			somme = 0.0;

			for (int k=0; k<j; k++) {
				somme = somme + L.element(i,k)*L.element(j,k);
			}
			L.set(i,j,(M.element(i,j)-somme) / L.element(j,j));
		}

		somme = 0.0;
		for (int k=0; k<i; k++) {
			somme = somme + L.element(i,k)*L.element(i,k);
		}
		L.set(i,i,sqrt(M.element(i,i)-somme));
	}
}
*/

SOFA_DECL_CLASS(CholeskySolver)

int CholeskySolverClass = core::RegisterObject("Linear system solver using the conjugate gradient iterative algorithm")
//.add< CholeskySolver<GraphScatteredMatrix,GraphScatteredVector> >(true)
        .add< CholeskySolver< SparseMatrix<double>, FullVector<double> > >(true)
//.add< CholeskySolver<NewMatBandMatrix,NewMatVector> >(true)
//.add< CholeskySolver<NewMatMatrix,NewMatVector> >()
        .add< CholeskySolver<NewMatSymmetricMatrix,NewMatVector> >()
//.add< CholeskySolver<NewMatSymmetricBandMatrix,NewMatVector> >()
        .add< CholeskySolver< FullMatrix<double>, FullVector<double> > >()
        .addAlias("CholeskySolverAlias")
        ;

} // namespace linearsolver

} // namespace component

} // namespace sofa

