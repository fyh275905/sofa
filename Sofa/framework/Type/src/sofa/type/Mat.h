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

#include <sofa/type/config.h>
#include <sofa/type/fwd.h>

#include <sofa/type/fixed_array.h>
#include <sofa/type/Vec.h>

#include <iostream>

namespace // anonymous
{
    template<typename real>
    constexpr real rabs(const real r)
    {
        if constexpr (std::is_signed<real>())
            return std::abs(r);
        else
            return r;
    }

    template<typename real>
    constexpr real equalsZero(const real r, const real epsilon = std::numeric_limits<real>::epsilon())
    {
        return rabs(r) <= epsilon;
    }

} // anonymous namespace

namespace sofa::type
{

template <sofa::Size L, sofa::Size C, class real>
class Mat : public fixed_array<VecNoInit<C,real>, L>
{
public:

    enum { N = L*C };

    typedef typename fixed_array<real, N>::size_type Size;

    typedef real Real;
    typedef Vec<C,real> Line;
    typedef VecNoInit<C,real> LineNoInit;
    typedef Vec<L,real> Col;

    static const Size nbLines = L;
    static const Size nbCols  = C;

    constexpr Mat() noexcept
    {
        clear();
    }

    explicit constexpr Mat(NoInit) noexcept
    {
    }

    template<typename... ArgsT,
        typename = std::enable_if_t< (std::is_convertible_v<ArgsT, Line> && ...) >,
        typename = std::enable_if_t< (sizeof...(ArgsT) == L && sizeof...(ArgsT) > 1) >
    >
    constexpr Mat(ArgsT&&... r) noexcept
        : sofa::type::fixed_array<LineNoInit, L>(std::forward<ArgsT>(r)...)
    {}

    /// Constructor from an element
    explicit constexpr Mat(const real& v) noexcept
    {
        for( Size i=0; i<L; i++ )
            for( Size j=0; j<C; j++ )
                this->elems[i][j] = v;
    }

    /// Constructor from another matrix
    template<typename real2>
    constexpr Mat(const Mat<L,C,real2>& m) noexcept
    {
        std::copy(m.begin(), m.begin()+L, this->begin());
    }

    /// Constructor from another matrix with different size (with null default entries and ignoring outside entries)
    template<Size L2, Size C2, typename real2>
    explicit constexpr Mat(const Mat<L2,C2,real2>& m) noexcept
    {
        constexpr Size minL = std::min( L, L2 );
        constexpr Size minC = std::min( C, C2 );

        for( Size l=0 ; l<minL ; ++l )
        {
            for( Size c=0 ; c< minC; ++c )
                this->elems[l][c] = static_cast<real>(m[l][c]);
            for( Size c= minC; c<C ; ++c )
                this->elems[l][c] = 0;
        }

        for( Size l= minL; l<L ; ++l )
            for( Size c=0 ; c<C ; ++c )
                this->elems[l][c] = 0;
    }

    /// Constructor from an array of elements (stored per line).
    template<typename real2>
    explicit constexpr Mat(const real2* p) noexcept
    {
        std::copy(p, p+N, this->begin()->begin());
    }

    /// number of lines
    constexpr Size getNbLines() const
    {
        return L;
    }

    /// number of colums
    constexpr Size getNbCols() const
    {
        return C;
    }


    /// Assignment from an array of elements (stored per line).
    constexpr void operator=(const real* p) noexcept
    {
        std::copy(p, p+N, this->begin()->begin());
    }

    /// Assignment from another matrix
    template<typename real2> 
    constexpr void operator=(const Mat<L,C,real2>& m) noexcept
    {
        std::copy(m.begin(), m.begin()+L, this->begin());
    }

    /// Assignment from a matrix of different size.
    template<Size L2, Size C2> 
    constexpr void operator=(const Mat<L2,C2,real>& m) noexcept
    {
        std::copy(m.begin(), m.begin()+(L>L2?L2:L), this->begin());
    }

    template<Size L2, Size C2> 
    constexpr void getsub(Size L0, Size C0, Mat<L2,C2,real>& m) const noexcept
    {
        for (Size i=0; i<L2; i++)
            for (Size j=0; j<C2; j++)
                m[i][j] = this->elems[i+L0][j+C0];
    }

    template<Size L2, Size C2> 
    constexpr void setsub(Size L0, Size C0, const Mat<L2,C2,real>& m) noexcept
    {
        for (Size i=0; i<L2; i++)
            for (Size j=0; j<C2; j++)
                this->elems[i+L0][j+C0] = m[i][j];
    }

    template<Size L2> 
    constexpr void setsub(Size L0, Size C0, const Vec<L2,real>& v) noexcept
    {
        assert( C0<C );
        assert( L0+L2-1<L );
        for (Size i=0; i<L2; i++)
            this->elems[i+L0][C0] = v[i];
    }


    /// Sets each element to 0.
    constexpr void clear() noexcept
    {
        for (Size i=0; i<L; i++)
            this->elems[i].clear();
    }

    /// Sets each element to r.
    constexpr void fill(real r) noexcept
    {
        for (Size i=0; i<L; i++)
            this->elems[i].fill(r);
    }

    /// Read-only access to line i.
    constexpr const Line& line(Size i) const noexcept
    {
        return this->elems[i];
    }

    /// Copy of column j.
    constexpr Col col(Size j) const noexcept
    {
        Col c;
        for (Size i=0; i<L; i++)
            c[i]=this->elems[i][j];
        return c;
    }

    /// Write acess to line i.
    constexpr LineNoInit& operator[](Size i) noexcept
    {
        return this->elems[i];
    }

    /// Read-only access to line i.
    constexpr const LineNoInit& operator[](Size i) const noexcept
    {
        return this->elems[i];
    }

    /// Write acess to line i.
    constexpr LineNoInit& operator()(Size i) noexcept
    {
        return this->elems[i];
    }

    /// Read-only access to line i.
    constexpr const LineNoInit& operator()(Size i) const noexcept
    {
        return this->elems[i];
    }

    /// Write access to element (i,j).
    constexpr real& operator()(Size i, Size j) noexcept
    {
        return this->elems[i][j];
    }

    /// Read-only access to element (i,j).
    constexpr const real& operator()(Size i, Size j) const noexcept
    {
        return this->elems[i][j];
    }

    /// Cast into a standard C array of lines (read-only).
    constexpr const Line* lptr() const noexcept
    {
        return this->elems;
    }

    /// Cast into a standard C array of lines.
    constexpr Line* lptr() noexcept
    {
        return this->elems;
    }

    /// Cast into a standard C array of elements (stored per line) (read-only).
    constexpr const real* ptr() const noexcept
    {
        return this->elems[0].ptr();
    }

    /// Cast into a standard C array of elements (stored per line).
    constexpr real* ptr() noexcept
    {
        return this->elems[0].ptr();
    }

    /// Special access to first line.
    constexpr Line& x()  noexcept { static_assert(L >= 1, ""); return this->elems[0]; }
    /// Special access to second line.
    constexpr Line& y()  noexcept { static_assert(L >= 2, ""); return this->elems[1]; }
    /// Special access to third line.
    constexpr Line& z()  noexcept { static_assert(L >= 3, ""); return this->elems[2]; }
    /// Special access to fourth line.
    constexpr Line& w()  noexcept { static_assert(L >= 4, ""); return this->elems[3]; }

    /// Special access to first line (read-only).
    constexpr const Line& x() const noexcept { static_assert(L >= 1, ""); return this->elems[0]; }
    /// Special access to second line (read-only).
    constexpr const Line& y() const noexcept { static_assert(L >= 2, ""); return this->elems[1]; }
    /// Special access to thrid line (read-only).
    constexpr const Line& z() const noexcept { static_assert(L >= 3, ""); return this->elems[2]; }
    /// Special access to fourth line (read-only).
    constexpr const Line& w() const noexcept { static_assert(L >= 4, ""); return this->elems[3]; }

    /// Set matrix to identity.
    constexpr void identity() noexcept
    {
        static_assert(L == C, "");
        clear();
        for (Size i=0; i<L; i++)
            this->elems[i][i]=1;
    }

    /// Returns the identity matrix
    static Mat<L,L,real> Identity() noexcept
    {
        static_assert(L == C, "");
        Mat<L,L,real> id;
        for (Size i=0; i<L; i++)
            id[i][i]=1;
        return id;
    }

    /// precomputed identity matrix of size (L,L)
    static Mat<L, L, real> s_identity;

    template<Size S>
    static bool canSelfTranspose(const Mat<S, S, real>& lhs, const Mat<S, S, real>& rhs) noexcept
    {
        return &lhs == &rhs;
    }

    template<Size I, Size J>
    static bool canSelfTranspose(const Mat<I, J, real>& /*lhs*/, const Mat<J, I, real>& /*rhs*/) noexcept
    {
        return false;
    }

    /// Set matrix as the transpose of m.
    constexpr void transpose(const Mat<C,L,real> &m) noexcept
    {
        if (canSelfTranspose(*this, m))
        {
            for (Size i=0; i<L; i++)
            {
                for (Size j=i+1; j<C; j++)
                {
                    std::swap(this->elems[i][j], this->elems[j][i]);
                }
            }
        }
        else
        {
            for (Size i=0; i<L; i++)
                for (Size j=0; j<C; j++)
                    this->elems[i][j]=m[j][i];
        }
    }

    /// Return the transpose of m.
    constexpr Mat<C,L,real> transposed() const noexcept
    {
        Mat<C,L,real> m(NOINIT);
        for (Size i=0; i<L; i++)
            for (Size j=0; j<C; j++)
                m[j][i]=this->elems[i][j];
        return m;
    }

    /// Transpose the square matrix.
    constexpr void transpose() noexcept
    {
        static_assert(L == C, "Cannot self-transpose a non-square matrix. Use transposed() instead");
        for (Size i=0; i<L; i++)
        {
            for (Size j=i+1; j<C; j++)
            {
                std::swap(this->elems[i][j], this->elems[j][i]);
            }
        }
    }

    /// @name Tests operators
    /// @{

    constexpr bool operator==(const Mat<L,C,real>& b) const noexcept
    {
        for (Size i=0; i<L; i++)
            if (this->elems[i] != b[i]) return false;
        return true;
    }

    constexpr bool operator!=(const Mat<L,C,real>& b) const noexcept
    {
        for (Size i=0; i<L; i++)
            if (this->elems[i]!=b[i]) return true;
        return false;
    }


    constexpr bool isSymmetric() const
    {
        for (Size i=0; i<L; i++)
            for (Size j=i+1; j<C; j++)
                if( rabs( this->elems[i][j] - this->elems[j][i] ) > EQUALITY_THRESHOLD ) return false;
        return true;
    }

    constexpr bool isDiagonal() const noexcept
    {
        for (Size i=0; i<L; i++)
        {
            for (Size j=0; j<i-1; j++)
                if( rabs( this->elems[i][j] ) > EQUALITY_THRESHOLD ) return false;
            for (Size j=i+1; j<C; j++)
                if( rabs( this->elems[i][j] ) > EQUALITY_THRESHOLD ) return false;
        }
        return true;
    }


    /// @}

    // LINEAR ALGEBRA

    /// Matrix multiplication operator.
    template <Size P>
    constexpr Mat<L,P,real> operator*(const Mat<C,P,real>& m) const noexcept
    {
        Mat<L,P,real> r(NOINIT);
        for(Size i=0; i<L; i++)
            for(Size j=0; j<P; j++)
            {
                r[i][j]=(*this)[i][0] * m[0][j];
                for(Size k=1; k<C; k++)
                    r[i][j] += (*this)[i][k] * m[k][j];
            }
        return r;
    }

    /// Matrix addition operator.
    constexpr Mat<L,C,real> operator+(const Mat<L,C,real>& m) const noexcept
    {
        Mat<L,C,real> r(NOINIT);
        for(Size i = 0; i < L; i++)
            r[i] = (*this)[i] + m[i];
        return r;
    }

    /// Matrix subtraction operator.
    constexpr Mat<L,C,real> operator-(const Mat<L,C,real>& m) const noexcept
    {
        Mat<L,C,real> r(NOINIT);
        for(Size i = 0; i < L; i++)
            r[i] = (*this)[i] - m[i];
        return r;
    }

    /// Matrix negation operator.
    constexpr Mat<L,C,real> operator-() const noexcept
    {
        Mat<L,C,real> r(NOINIT);
        for(Size i = 0; i < L; i++)
            r[i] = -(*this)[i];
        return r;
    }

    /// Multiplication operator Matrix * Line.
    constexpr Col operator*(const Line& v) const noexcept
    {
        Col r(NOINIT);
        for(Size i=0; i<L; i++)
        {
            r[i]=(*this)[i][0] * v[0];
            for(Size j=1; j<C; j++)
                r[i] += (*this)[i][j] * v[j];
        }
        return r;
    }


    /// Multiplication with a diagonal Matrix CxC represented as a vector of size C
    constexpr Mat<L,C,real> multDiagonal(const Line& d) const noexcept
    {
        Mat<L,C,real> r(NOINIT);
        for(Size i=0; i<L; i++)
            for(Size j=0; j<C; j++)
                r[i][j]=(*this)[i][j] * d[j];
        return r;
    }

    /// Multiplication of the transposed Matrix * Column
    constexpr Line multTranspose(const Col& v) const noexcept
    {
        Line r(NOINIT);
        for(Size i=0; i<C; i++)
        {
            r[i]=(*this)[0][i] * v[0];
            for(Size j=1; j<L; j++)
                r[i] += (*this)[j][i] * v[j];
        }
        return r;
    }


    /// Transposed Matrix multiplication operator.
    template <Size P>
    constexpr Mat<C,P,real> multTranspose(const Mat<L,P,real>& m) const noexcept
    {
        Mat<C,P,real> r(NOINIT);
        for(Size i=0; i<C; i++)
            for(Size j=0; j<P; j++)
            {
                r[i][j]=(*this)[0][i] * m[0][j];
                for(Size k=1; k<L; k++)
                    r[i][j] += (*this)[k][i] * m[k][j];
            }
        return r;
    }

    /// Multiplication with the transposed of the given matrix operator \returns this * mt
    template <Size P>
    constexpr Mat<L,P,real> multTransposed(const Mat<P,C,real>& m) const noexcept
    {
        Mat<L,P,real> r(NOINIT);
        for(Size i=0; i<L; i++)
            for(Size j=0; j<P; j++)
            {
                r[i][j]=(*this)[i][0] * m[j][0];
                for(Size k=1; k<C; k++)
                    r[i][j] += (*this)[i][k] * m[j][k];
            }
        return r;
    }

    /// Addition with the transposed of the given matrix operator \returns this + mt
    constexpr Mat<L,C,real> plusTransposed(const Mat<C,L,real>& m) const noexcept
    {
        Mat<L,C,real> r(NOINIT);
        for(Size i=0; i<L; i++)
            for(Size j=0; j<C; j++)
                r[i][j] = (*this)[i][j] + m[j][i];
        return r;
    }

    /// Substraction with the transposed of the given matrix operator \returns this - mt
    constexpr Mat<L,C,real>minusTransposed(const Mat<C,L,real>& m) const noexcept
    {
        Mat<L,C,real> r(NOINIT);
        for(Size i=0; i<L; i++)
            for(Size j=0; j<C; j++)
                r[i][j] = (*this)[i][j] - m[j][i];
        return r;
    }


    /// Scalar multiplication operator.
    constexpr Mat<L,C,real> operator*(real f) const noexcept
    {
        Mat<L,C,real> r(NOINIT);
        for(Size i=0; i<L; i++)
            for(Size j=0; j<C; j++)
                r[i][j] = (*this)[i][j] * f;
        return r;
    }

    /// Scalar matrix multiplication operator.
    friend constexpr Mat<L,C,real> operator*(real r, const Mat<L,C,real>& m) noexcept
    {
        return m*r;
    }

    /// Scalar division operator.
    constexpr Mat<L,C,real> operator/(real f) const
    {
        Mat<L,C,real> r(NOINIT);
        for(Size i=0; i<L; i++)
            for(Size j=0; j<C; j++)
                r[i][j] = (*this)[i][j] / f;
        return r;
    }

    /// Scalar multiplication assignment operator.
    constexpr void operator *=(real r) noexcept
    {
        for(Size i=0; i<L; i++)
            this->elems[i]*=r;
    }

    /// Scalar division assignment operator.
    constexpr void operator /=(real r)
    {
        for(Size i=0; i<L; i++)
            this->elems[i]/=r;
    }

    /// Addition assignment operator.
    constexpr void operator +=(const Mat<L,C,real>& m) noexcept
    {
        for(Size i=0; i<L; i++)
            this->elems[i]+=m[i];
    }

    /// Addition of the transposed of m
    constexpr void addTransposed(const Mat<C,L,real>& m) noexcept
    {
        for(Size i=0; i<L; i++)
            for(Size j=0; j<C; j++)
                (*this)[i][j] += m[j][i];
    }

    /// Substraction of the transposed of m
    constexpr void subTransposed(const Mat<C,L,real>& m) noexcept
    {
        for(Size i=0; i<L; i++)
            for(Size j=0; j<C; j++)
                (*this)[i][j] -= m[j][i];
    }

    /// Substraction assignment operator.
    constexpr void operator -=(const Mat<L,C,real>& m) noexcept
    {
        for(Size i=0; i<L; i++)
            this->elems[i]-=m[i];
    }


    /// invert this
    constexpr Mat<L,C,real> inverted() const
    {
        static_assert(L == C, "Cannot invert a non-square matrix");
        Mat<L,C,real> m = *this;

        const bool canInvert = invertMatrix(m, *this);
        assert(canInvert);
        SOFA_UNUSED(canInvert);

        return m;
    }

    /// Invert square matrix m
    [[nodiscard]] constexpr bool invert(const Mat<L,C,real>& m)
    {
        static_assert(L == C, "Cannot invert a non-square matrix");
        if (&m == this)
        {
            Mat<L,C,real> mat = m;
            bool res = invertMatrix(*this, mat);
            return res;
        }
        return invertMatrix(*this, m);
    }

    static Mat<L,C,real> transformTranslation(const Vec<C-1,real>& t) noexcept
    {
        Mat<L,C,real> m;
        m.identity();
        for (Size i=0; i<C-1; ++i)
            m.elems[i][C-1] = t[i];
        return m;
    }

    static Mat<L,C,real> transformScale(real s) noexcept
    {
        Mat<L,C,real> m;
        m.identity();
        for (Size i=0; i<C-1; ++i)
            m.elems[i][i] = s;
        return m;
    }

    static Mat<L,C,real> transformScale(const Vec<C-1,real>& s) noexcept
    {
        Mat<L,C,real> m;
        m.identity();
        for (Size i=0; i<C-1; ++i)
            m.elems[i][i] = s[i];
        return m;
    }

    template<class Quat>
    static Mat<L,C,real> transformRotation(const Quat& q) noexcept
    {
        static_assert(L == C && (L ==4 || L == 3), "transformRotation can only be called with 3x3 or 4x4 matrices.");

        Mat<L,C,real> m;
        m.identity();

        if constexpr(L == 4 && C == 4)
        {
            q.toHomogeneousMatrix(m);
            return m;
        }
        else // if constexpr(L == 3 && C == 3)
        {
            q.toMatrix(m);
            return m;
        }
    }

    /// @return True if and only if the Matrix is a transformation matrix
    constexpr bool isTransform() const
    {
        for (Size j=0;j<C-1;++j)
            if (fabs((*this)(L-1,j)) > EQUALITY_THRESHOLD)
                return false;
        if (fabs((*this)(L-1,C-1) - 1.) > EQUALITY_THRESHOLD)
            return false;
        return true;
    }

    /// Multiplication operator Matrix * Vector considering the matrix as a transformation.
    constexpr Vec<C-1,real> transform(const Vec<C-1,real>& v) const noexcept
    {
        Vec<C-1,real> r(NOINIT);
        for(Size i=0; i<C-1; i++)
        {
            r[i]=(*this)[i][0] * v[0];
            for(Size j=1; j<C-1; j++)
                r[i] += (*this)[i][j] * v[j];
            r[i] += (*this)[i][C-1];
        }
        return r;
    }

    /// Invert transformation matrix m
    constexpr bool transformInvert(const Mat<L,C,real>& m)
    {
        return transformInvertMatrix(*this, m);
    }

    /// for square matrices
    /// @warning in-place simple symmetrization
    /// this = ( this + this.transposed() ) / 2.0
    constexpr void symmetrize() noexcept
    {
        static_assert( C == L, "" );
        for(Size l=0; l<L; l++)
            for(Size c=l+1; c<C; c++)
                this->elems[l][c] = this->elems[c][l] = ( this->elems[l][c] + this->elems[c][l] ) * 0.5f;
    }

};


template <sofa::Size L, sofa::Size C, typename real> 
Mat<L, L, real> Mat<L, C, real>::s_identity = Mat<L, L, real>::Identity();

/// Same as Mat except the values are not initialized by default
template <sofa::Size L, sofa::Size C, typename real>
class MatNoInit : public Mat<L,C,real>
{
public:
    constexpr MatNoInit() noexcept
        : Mat<L,C,real>(NOINIT) 
    {
    }

    /// Assignment from an array of elements (stored per line).
    constexpr void operator=(const real* p) noexcept
    {
        this->Mat<L,C,real>::operator=(p);
    }

    /// Assignment from another matrix
    template<sofa::Size L2, sofa::Size C2, typename real2> 
    constexpr void operator=(const Mat<L2,C2,real2>& m) noexcept
    {
        this->Mat<L,C,real>::operator=(m);
    }
};

/// Determinant of a 3x3 matrix.
template<class real>
constexpr real determinant(const Mat<3,3,real>& m) noexcept
{
    return m(0,0)*m(1,1)*m(2,2)
            + m(1,0)*m(2,1)*m(0,2)
            + m(2,0)*m(0,1)*m(1,2)
            - m(0,0)*m(2,1)*m(1,2)
            - m(1,0)*m(0,1)*m(2,2)
            - m(2,0)*m(1,1)*m(0,2);
}

/// Determinant of a 2x2 matrix.
template<class real>
constexpr real determinant(const Mat<2,2,real>& m) noexcept
{
    return m(0,0)*m(1,1)
            - m(1,0)*m(0,1);
}

/// Generalized-determinant of a 2x3 matrix.
/// Mirko Radi, "About a Determinant of Rectangular 2×n Matrix and its Geometric Interpretation"
template<class real>
constexpr real determinant(const Mat<2,3,real>& m) noexcept
{
    return m(0,0)*m(1,1) - m(0,1)*m(1,0) - ( m(0,0)*m(1,2) - m(0,2)*m(1,0) ) + m(0,1)*m(1,2) - m(0,2)*m(1,1);
}

/// Generalized-determinant of a 3x2 matrix.
/// Mirko Radi, "About a Determinant of Rectangular 2×n Matrix and its Geometric Interpretation"
template<class real>
constexpr real determinant(const Mat<3,2,real>& m) noexcept
{
    return m(0,0)*m(1,1) - m(1,0)*m(0,1) - ( m(0,0)*m(2,1) - m(2,0)*m(0,1) ) + m(1,0)*m(2,1) - m(2,0)*m(1,1);
}

// one-norm of a 3 x 3 matrix
template<class real>
constexpr real oneNorm(const Mat<3,3,real>& A)
{
    real norm = 0.0;
    for (sofa::Size i=0; i<3; i++)
    {
        real columnAbsSum = rabs(A(0,i)) + rabs(A(1,i)) + rabs(A(2,i));
        if (columnAbsSum > norm)
            norm = columnAbsSum;
    }
    return norm;
}

// inf-norm of a 3 x 3 matrix
template<class real>
constexpr real infNorm(const Mat<3,3,real>& A)
{
    real norm = 0.0;
    for (sofa::Size i=0; i<3; i++)
    {
        real rowSum = rabs(A(i,0)) + rabs(A(i,1)) + rabs(A(i,2));
        if (rowSum > norm)
            norm = rowSum;
    }
    return norm;
}

/// trace of a square matrix
template<sofa::Size N, class real>
constexpr real trace(const Mat<N,N,real>& m) noexcept
{
    real t = m[0][0];
    for(sofa::Size i=1 ; i<N ; ++i ) 
        t += m[i][i];
    return t;
}

/// diagonal of a square matrix
template<sofa::Size N, class real>
constexpr Vec<N,real> diagonal(const Mat<N,N,real>& m)
{
    Vec<N,real> v(NOINIT);
    for(sofa::Size i=0 ; i<N ; ++i ) 
        v[i] = m[i][i];
    return v;
}

/// Matrix inversion (general case).
template<sofa::Size S, class real>
[[nodiscard]] constexpr bool invertMatrix(Mat<S,S,real>& dest, const Mat<S,S,real>& from)
{
    sofa::Size i{0}, j{0}, k{0};
    Vec<S, sofa::Size> r, c, row, col;

    Mat<S,S,real> m1 = from;
    Mat<S,S,real> m2;
    m2.identity();

    for ( k = 0; k < S; k++ )
    {
        // Choosing the pivot
        real pivot = 0;
        for (i = 0; i < S; i++)
        {
            if (row[i])
                continue;
            for (j = 0; j < S; j++)
            {
                if (col[j])
                    continue;
                real t = m1[i][j]; if (t<0) t=-t;
                if ( t > pivot)
                {
                    pivot = t;
                    r[k] = i;
                    c[k] = j;
                }
            }
        }

        if (equalsZero(pivot))
        {
            return false;
        }

        row[r[k]] = col[c[k]] = 1;
        pivot = m1[r[k]][c[k]];

        // Normalization
        m1[r[k]] /= pivot; m1[r[k]][c[k]] = 1;
        m2[r[k]] /= pivot;

        // Reduction
        for (i = 0; i < S; i++)
        {
            if (i != r[k])
            {
                real f = m1[i][c[k]];
                m1[i] -= m1[r[k]]*f; m1[i][c[k]] = 0;
                m2[i] -= m2[r[k]]*f;
            }
        }
    }

    for (i = 0; i < S; i++)
        for (j = 0; j < S; j++)
            if (c[j] == i)
                row[i] = r[j];

    for ( i = 0; i < S; i++ )
        dest[i] = m2[row[i]];

    return true;
}

/// Matrix inversion (special case 3x3).
template<class real>
[[nodiscard]] constexpr bool invertMatrix(Mat<3,3,real>& dest, const Mat<3,3,real>& from)
{
    const real det=determinant(from);

    if (equalsZero(det))
    {
        return false;
    }

    dest(0,0)= (from(1,1)*from(2,2) - from(2,1)*from(1,2))/det;
    dest(1,0)= (from(1,2)*from(2,0) - from(2,2)*from(1,0))/det;
    dest(2,0)= (from(1,0)*from(2,1) - from(2,0)*from(1,1))/det;
    dest(0,1)= (from(2,1)*from(0,2) - from(0,1)*from(2,2))/det;
    dest(1,1)= (from(2,2)*from(0,0) - from(0,2)*from(2,0))/det;
    dest(2,1)= (from(2,0)*from(0,1) - from(0,0)*from(2,1))/det;
    dest(0,2)= (from(0,1)*from(1,2) - from(1,1)*from(0,2))/det;
    dest(1,2)= (from(0,2)*from(1,0) - from(1,2)*from(0,0))/det;
    dest(2,2)= (from(0,0)*from(1,1) - from(1,0)*from(0,1))/det;

    return true;
}

/// Matrix inversion (special case 2x2).
template<class real>
[[nodiscard]] constexpr bool invertMatrix(Mat<2,2,real>& dest, const Mat<2,2,real>& from)
{
    const real det=determinant(from);

    if (equalsZero(det))
    {
        return false;
    }

    dest(0,0)=  from(1,1)/det;
    dest(0,1)= -from(0,1)/det;
    dest(1,0)= -from(1,0)/det;
    dest(1,1)=  from(0,0)/det;

    return true;
}

/// Inverse Matrix considering the matrix as a transformation.
template<sofa::Size S, class real>
[[nodiscard]] constexpr bool transformInvertMatrix(Mat<S,S,real>& dest, const Mat<S,S,real>& from)
{
    Mat<S-1,S-1,real> R, R_inv;
    from.getsub(0,0,R);
    bool b = invertMatrix(R_inv, R);

    Mat<S-1,1,real> t, t_inv;
    from.getsub(0,S-1,t);
    t_inv = -1.*R_inv*t;

    dest.setsub(0,0,R_inv);
    dest.setsub(0,S-1,t_inv);
    for (sofa::Size i=0; i<S-1; ++i)
        dest(S-1,i)=0.0;
    dest(S-1,S-1)=1.0;

    return b;
}

template <sofa::Size L, sofa::Size C, typename real>
std::ostream& operator<<(std::ostream& o, const Mat<L,C,real>& m)
{
    o << '[' << m[0];
    for (sofa::Size i=1; i<L; i++)
        o << ',' << m[i];
    o << ']';
    return o;
}

template <sofa::Size L, sofa::Size C, typename real>
std::istream& operator>>(std::istream& in, Mat<L,C,real>& m)
{
    sofa::Size c;
    c = in.peek();
    while (c==' ' || c=='\n' || c=='[')
    {
        in.get();
        if( c=='[' ) break;
        c = in.peek();
    }
    in >> m[0];
    for (sofa::Size i=1; i<L; i++)
    {
        c = in.peek();
        while (c==' ' || c==',')
        {
            in.get();
            c = in.peek();
        }
        in >> m[i];
    }
    if(in.eof()) return in;
    c = in.peek();
    while (c==' ' || c=='\n' || c==']')
    {
        in.get();
        if( c==']' ) break;
        if(in.eof()) break;
        c = in.peek();
    }
    return in;
}

/// printing in other software formats

template <sofa::Size L, sofa::Size C, typename real>
void printMatlab(std::ostream& o, const Mat<L,C,real>& m)
{
    o<<"[";
    for(sofa::Size l=0; l<L; ++l)
    {
        for(sofa::Size c=0; c<C; ++c)
        {
            o<<m[l][c];
            if( c!=C-1 ) o<<",\t";
        }
        if( l!=L-1 ) o<<";"<<std::endl;
    }
    o<<"]"<<std::endl;
}


template <sofa::Size L, sofa::Size C, typename real>
void printMaple(std::ostream& o, const Mat<L,C,real>& m)
{
    o<<"matrix("<<L<<","<<C<<", [";
    for(sofa::Size l=0; l<L; ++l)
    {
        for(sofa::Size c=0; c<C; ++c)
        {
            o<<m[l][c];
            o<<",\t";
        }
        if( l!=L-1 ) o<<std::endl;
    }
    o<<"])"<<std::endl;
}



/// Create a matrix as \f$ u v^T \f$
template <sofa::Size L, sofa::Size C, typename T>
constexpr Mat<L,C,T> dyad( const Vec<L,T>& u, const Vec<C,T>& v ) noexcept
{
    Mat<L,C,T> res(NOINIT);
    for(sofa::Size i=0; i<L; i++ )
        for(sofa::Size j=0; j<C; j++ )
            res[i][j] = u[i]*v[j];
    return res;
}

/// Compute the scalar product of two matrix (sum of product of all terms)
template <sofa::Size L, sofa::Size C, typename real>
constexpr real scalarProduct(const Mat<L,C,real>& left,const Mat<L,C,real>& right) noexcept
{
    real product(0.);
    for(sofa::Size i=0; i<L; i++)
        for(sofa::Size j=0; j<C; j++)
            product += left(i,j) * right(i,j);
    return product;
}


/// skew-symmetric mapping
/// crossProductMatrix(v) * x = v.cross(x)
template<class Real>
constexpr Mat<3, 3, Real> crossProductMatrix(const Vec<3, Real>& v) noexcept
{
    type::Mat<3, 3, Real> res(NOINIT);
    res[0][0]=0;
    res[0][1]=-v[2];
    res[0][2]=v[1];
    res[1][0]=v[2];
    res[1][1]=0;
    res[1][2]=-v[0];
    res[2][0]=-v[1];
    res[2][1]=v[0];
    res[2][2]=0;
    return res;
}


/// return a * b^T
template<sofa::Size L,class Real>
constexpr Mat<L,L,Real> tensorProduct(const Vec<L,Real>& a, const Vec<L,Real>& b ) noexcept
{
    typedef MatNoInit<L,L,Real> Mat;
    Mat m;

    for( typename Mat::Size i=0 ; i<L ; ++i )
        for( typename Mat::Size j=0 ; j<L ; ++j )
            m[i][j] = a[i]*b[j];

    return m;
}

} // namespace sofa::type
