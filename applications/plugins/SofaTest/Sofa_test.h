/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc., 51  *
* Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
*******************************************************************************
*                            SOFA :: Applications                             *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
// Francois Faure, 2013

#ifndef SOFA_STANDARDTEST_Sofa_test_H
#define SOFA_STANDARDTEST_Sofa_test_H

#if (_MSC_VER == 1700) 
#define _VARIADIC_MAX 10 
#endif

#include "initTestPlugin.h"
#include <gtest/gtest.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/simulation/common/Node.h>
#include <time.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

namespace sofa {


/** @brief Base class for Sofa test fixtures.
  */
  struct SOFA_TestPlugin_API BaseSofa_test : public ::testing::Test
{
    /// Initialize Sofa and the random number generator
    BaseSofa_test();

    /// Clear the scene graph
    virtual ~BaseSofa_test();

    /// Clear the scene graph
    void clearSceneGraph();
};


/** @brief Helper functions to compare scalars, vectors, matrices, etc.
  */
template <typename _Real=SReal>
struct SOFA_TestPlugin_API  Sofa_test : public BaseSofa_test
{

    /** @name Scalars
     *  Type and functions to manipulate real numbers.
     */
    ///@{
    typedef _Real Real; ///< Scalar type

    /// the smallest real number
    static Real epsilon(){ return std::numeric_limits<Real>::epsilon(); }

    /// Infinity
    static Real infinity(){ return std::numeric_limits<Real>::infinity(); }

    /// true if the magnitude of r is less than ratio*epsilon
    static bool isSmall(Real r, Real ratio=1. ){
        return fabs(r) < ratio * std::numeric_limits<Real>::epsilon();
    }

    ///@}

    /** @name Vectors
     *  Functions to compare vectors
     */
    ///@{


    /// return the maximum difference between corresponding entries, or the infinity if the vectors have different sizes
    template< int N, typename Real, typename Vector2>
    Real vectorMaxDiff( const sofa::defaulttype::Vec<N,Real>& m1, const Vector2& m2 )
    {
        if( N !=m2.size() ) {
            ADD_FAILURE() << "Comparison between vectors of different sizes";
            return std::numeric_limits<Real>::infinity();
        }
        Real result = 0;
        for( unsigned i=0; i<N; i++ ){
            Real diff = fabs(m1[i]-m2.element(i));
            if( diff>result  ) result=diff;
        }
        return result;
    }


    /// return the maximum difference between corresponding entries, or the infinity if the vectors have different sizes
    template< int N, typename Real>
    Real vectorMaxDiff( const sofa::defaulttype::Vec<N,Real>& m1, const sofa::defaulttype::Vec<N,Real>& m2 )
    {
        Real result = 0;
        for( unsigned i=0; i<N; i++ ){
            Real diff = fabs(m1[i]-m2[i]);
            if( diff>result  ) result=diff;
        }
        return result;
    }

    /// Return the maximum difference between two containers. Issues a failure if sizes are different.
    template<class Container1, class Container2>
    Real vectorMaxDiff( const Container1& c1, const Container2& c2 )
    {
        if( c1.size()!=c2.size() ){
            ADD_FAILURE() << "containers have different sizes";
            return infinity();
        }

        Real maxdiff = 0.;
        for(unsigned i=0; i<c1.size(); i++ ){
//            cout<< c2[i]-c1[i] << " ";
            Real n = norm(c1[i]-c2[i]);
            if( n>maxdiff )
                maxdiff = n;
        }
        return maxdiff;
    }

    ///@}

    /** @name Matrices
     *  Functions to compare matrices
     */
    ///@{

    /// return the maximum difference between corresponding entries, or the infinity if the matrices have different sizes
    template<typename Matrix1, typename Matrix2>
    static Real matrixMaxDiff( const Matrix1& m1, const Matrix2& m2 )
    {
        Real result = 0;
        if(m1.rowSize()!=m2.rowSize() || m2.colSize()!=m1.colSize()){
            ADD_FAILURE() << "Comparison between matrices of different sizes";
            return infinity();
        }
        for(unsigned i=0; i<m1.rowSize(); i++)
            for(unsigned j=0; j<m1.colSize(); j++){
                Real diff = abs(m1.element(i,j)-m2.element(i,j));
                if(diff>result)
                    result = diff;
            }
        return result;
    }

    /// Return the maximum difference between corresponding entries, or the infinity if the matrices have different sizes
    template<int M, int N, typename Real, typename Matrix2>
    static Real matrixMaxDiff( const sofa::defaulttype::Mat<M,N,Real>& m1, const Matrix2& m2 )
    {
        Real result = 0;
        if(M!=m2.rowSize() || m2.colSize()!=N){
            ADD_FAILURE() << "Comparison between matrices of different sizes";
            return std::numeric_limits<Real>::infinity();
        }
        for(unsigned i=0; i<M; i++)
            for(unsigned j=0; j<N; j++){
                Real diff = abs(m1[i][j]-m2.element(i,j));
                if(diff>result)
                    result = diff;
            }
        return result;
    }

    ///@}

protected:
    // helpers
    Real norm(Real a){ return fabs(a); }
    template <typename T> Real norm(T a){ return a.norm(); }


};


/// Resize the Vector and copy it from the Data
template<class Vector, class ReadData>
void copyFromData( Vector& v, const ReadData& d){
    v.resize(d.size());
    for( unsigned i=0; i<v.size(); i++)
        v[i] = d[i];
}

/// Copy the Vector to the Data. They must have the same size.
template<class WriteData, class Vector>
void copyToData( WriteData& d, const Vector& v){
    for( unsigned i=0; i<d.size(); i++)
        d[i] = v[i];
}



}

#endif




