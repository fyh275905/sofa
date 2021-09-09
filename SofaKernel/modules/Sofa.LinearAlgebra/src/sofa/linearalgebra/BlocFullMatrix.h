/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <sofa/linearalgebra/config.h>

#include <sofa/linearalgebra/BaseMatrix.h>

namespace sofa::linearalgebra
{

/// Simple bloc full matrix container (used for InvMatrixType)
template< std::size_t N, typename T>
class BlocFullMatrix : public defaulttype::BaseMatrix
{
public:

    enum { BSIZE = N };
    typedef T Real;

    class TransposedBloc{

    public:
        const type::Mat<BSIZE,BSIZE,Real>& m;

        TransposedBloc(const sofa::type::Mat<BSIZE, BSIZE, Real>& m_a) : m(m_a){
}

        type::Vec<BSIZE,Real> operator*(const type::Vec<BSIZE,Real>& v)
        {
            return m.multTranspose(v);
        }

        type::Mat<BSIZE,BSIZE,Real> operator-() const
        {
            return -m.transposed();
        }
    };

    class Bloc : public type::Mat<BSIZE,BSIZE,Real>
    {
    public:
        Index Nrows() const;
        Index Ncols() const;
        void resize(Index, Index);
        const T& element(Index i, Index j) const;
        void set(Index i, Index j, const T& v);
        void add(Index i, Index j, const T& v);
        void operator=(const type::Mat<BSIZE,BSIZE,Real>& v)
        {
            type::Mat<BSIZE,BSIZE,Real>::operator=(v);
        }
        type::Mat<BSIZE,BSIZE,Real> operator-() const
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator-();
        }
        type::Mat<BSIZE,BSIZE,Real> operator-(const type::Mat<BSIZE,BSIZE,Real>& m) const
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator-(m);
        }
        type::Vec<BSIZE,Real> operator*(const type::Vec<BSIZE,Real>& v)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(v);
        }
        type::Mat<BSIZE,BSIZE,Real> operator*(const type::Mat<BSIZE,BSIZE,Real>& m)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(m);
        }
        type::Mat<BSIZE,BSIZE,Real> operator*(const Bloc& m)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(m);
        }
        type::Mat<BSIZE,BSIZE,Real> operator*(const TransposedBloc& mt)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(mt.m.transposed());
        }
        TransposedBloc t() const;
        Bloc i() const;
    };
    typedef Bloc SubMatrixType;
    typedef FullMatrix<T> InvMatrixType;
    // return the dimension of submatrices when requesting a given size
    static Index getSubMatrixDim(Index);

protected:
    Bloc* data;
    Index nTRow,nTCol;
    Index nBRow,nBCol;
    Index allocsize;

public:

    BlocFullMatrix();

    BlocFullMatrix(Index nbRow, Index nbCol);

    ~BlocFullMatrix() override;

    Bloc* ptr() { return data; }
    const Bloc* ptr() const { return data; }

    const Bloc& bloc(Index bi, Index bj) const;

    Bloc& bloc(Index bi, Index bj);

    void resize(Index nbRow, Index nbCol) override;

    Index rowSize(void) const override;

    Index colSize(void) const override;

    SReal element(Index i, Index j) const override;

    const Bloc& asub(Index bi, Index bj, Index, Index) const;

    const Bloc& sub(Index i, Index j, Index, Index) const;

    Bloc& asub(Index bi, Index bj, Index, Index);

    Bloc& sub(Index i, Index j, Index, Index);

    template<class B>
    void getSubMatrix(Index i, Index j, Index nrow, Index ncol, B& m);

    template<class B>
    void getAlignedSubMatrix(Index bi, Index bj, Index nrow, Index ncol, B& m);

    template<class B>
    void setSubMatrix(Index i, Index j, Index nrow, Index ncol, const B& m);

    template<class B>
    void setAlignedSubMatrix(Index bi, Index bj, Index nrow, Index ncol, const B& m);

    void set(Index i, Index j, double v) override;

    void add(Index i, Index j, double v) override;

    void clear(Index i, Index j) override;

    void clearRow(Index i) override;

    void clearCol(Index j) override;

    void clearRowCol(Index i) override;

    void clear() override;

    template<class Real2>
    FullVector<Real2> operator*(const FullVector<Real2>& v) const
    {
        FullVector<Real2> res(rowSize());
        for (Index bi=0; bi<nBRow; ++bi)
        {
            Index bj = 0;
            for (Index i=0; i<BSIZE; ++i)
            {
                Real r = 0;
                for (Index j=0; j<BSIZE; ++j)
                {
                    r += bloc(bi,bj)[i][j] * v[(bi + bj - 1)*BSIZE + j];
                }
                res[bi*BSIZE + i] = r;
            }
            for (++bj; bj<nBCol; ++bj)
            {
                for (Index i=0; i<BSIZE; ++i)
                {
                    Real r = 0;
                    for (Index j=0; j<BSIZE; ++j)
                    {
                        r += bloc(bi,bj)[i][j] * v[(bi + bj - 1)*BSIZE + j];
                    }
                    res[bi*BSIZE + i] += r;
                }
            }
        }
        return res;
    }


    static const char* Name();
};

} // namespace sofa::linearalgebra
