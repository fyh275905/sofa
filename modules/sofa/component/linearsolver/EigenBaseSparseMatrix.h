/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_COMPONENT_LINEARSOLVER_EigenBaseSparseMatrix_H
#define SOFA_COMPONENT_LINEARSOLVER_EigenBaseSparseMatrix_H

#include <sofa/defaulttype/BaseMatrix.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/helper/SortedPermutation.h>
#include <sofa/helper/vector.h>
#include <Eigen/Core>
#include <Eigen/Sparse>
using std::cerr;
using std::endl;

namespace sofa
{

namespace component
{

namespace linearsolver
{
using helper::vector;

//#define EigenBaseSparseMatrix_CHECK
//#define EigenBaseSparseMatrix_VERBOSE


/** Container of an Eigen::SparseMatrix<Real, RowMajor> matrix.

  WARNING: Random write is not possible. For efficiency, the filling must be performed per row, column in increasing order.
Method beginRow(int index) must be called before any entry can be appended to row i.
Then set(i,j,value) must be used in for increasing j. There is no need to explicitly end a row.
When all the entries are written, method endEdit() must be applied to finalize the matrix.
  */
template<class TReal>
class EigenBaseSparseMatrix : public defaulttype::BaseMatrix
{
public:

    typedef TReal Real;
    typedef Eigen::SparseMatrix<Real,Eigen::RowMajor> Matrix;
    typedef Eigen::Matrix<Real,Eigen::Dynamic,1>  VectorEigen;

    Matrix eigenMatrix;    ///< the data



    EigenBaseSparseMatrix(int nbRow=0, int nbCol=0)
    {
        resize(nbRow,nbCol);
    }

    /// Resize the matrix without preserving the data (the matrix is set to zero)
    void resize(int nbRow, int nbCol)
    {
        eigenMatrix.resize(nbRow,nbCol);
    }



    /// number of rows
    unsigned int rowSize(void) const
    {
        return eigenMatrix.rows();
    }

    /// number of columns
    unsigned int colSize(void) const
    {
        return eigenMatrix.cols();
    }

    SReal element(int i, int j) const
    {
        return eigenMatrix.coeff(i,j);
    }

    /// must be called before inserting any element in the given row
    void beginRow( int i )
    {
        eigenMatrix.startVec(i);
    }

    /// This is efficient only if done in storing order: line, row
    void set(int i, int j, double v)
    {
        if( v!=0.0 )
            eigenMatrix.insertBack(i,j) = (Real)v;
        //        cerr<<"EigenBaseSparseMatrix::set, size = "<< eigenMatrix.rows()<<", "<< eigenMatrix.cols()<<", entry: "<< i <<", "<<j<<" = "<< v << endl;
    }

    void add(int /*i*/, int /*j*/, double /*v*/)
    {
        cerr<<"EigenBaseSparseMatrix::add(int i, int j, double v) is not implemented !"<<endl;
    }

    void endEdit()
    {
        eigenMatrix.finalize();
    }

    void clear(int i, int j)
    {
        eigenMatrix.coeffRef(i,j) = (Real)0;
    }

    ///< Set all the entries of a row to 0. Not efficient !
    void clearRow(int /*i*/)
    {
        cerr<<"EigenBaseSparseMatrix::clearRow(int i) is not implemented !"<<endl;
    }

    ///< Set all the entries of a column to 0. Not efficient !
    void clearCol(int /*j*/)
    {
        cerr<<"EigenBaseSparseMatrix::clearCol(int i) is not implemented !"<<endl;
    }

    ///< Set all the entries of a column and a row to 0. Not efficient !
    void clearRowCol(int /*i*/)
    {
        cerr<<"EigenBaseSparseMatrix::clearRowCol(int i) is not implemented !"<<endl;
    }

    /// Set all values to 0, by resizing to the same size. @todo check that it really resets.
    void clear()
    {
        resize(0,0);
        resize(rowSize(),colSize());
    }

    /// Matrix-vector product
    void mult( VectorEigen& result, const VectorEigen& data )
    {
        result = eigenMatrix * data;
    }


    friend std::ostream& operator << (std::ostream& out, const EigenBaseSparseMatrix<TReal>& v )
    {
        int nx = v.colSize();
        int ny = v.rowSize();
        out << "[";
        for (int y=0; y<ny; ++y)
        {
            out << "\n[";
            for (int x=0; x<nx; ++x)
            {
                out << " " << v.element(y,x);
            }
            out << " ]";
        }
        out << " ]";
        return out;
    }

    static const char* Name();


};

template<> inline const char* EigenBaseSparseMatrix<double>::Name() { return "EigenBaseSparseMatrixd"; }
template<> inline const char* EigenBaseSparseMatrix<float>::Name()  { return "EigenBaseSparseMatrixf"; }



} // namespace linearsolver

} // namespace component

} // namespace sofa

#endif
