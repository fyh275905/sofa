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

#include <sofa/type/Mat.h>

// Lots of code does not include Vec because they include Mat
// So it makes sense to do that for them implicitly
// And it will generate a warning as well
#include <sofa/defaulttype/Vec.h>
#include <sofa/helper/logging/Messaging.h>

// The following SOFA_DEPRECATED_HEADER is commented to avoid a massive number of warnings.
// This flag will be enabled once all the code base in Sofa is ported to Sofa.Type.
// (PR #1790)
//SOFA_DEPRECATED_HEADER(v21.12, "sofa/type/Mat.h")

namespace sofa::defaulttype
{
using sofa::type::Mat;
using sofa::type::MatNoInit;

using sofa::type::Mat1x1f;
using sofa::type::Mat1x1d;

using sofa::type::Mat2x2f;
using sofa::type::Mat2x2d;

using sofa::type::Mat3x3f;
using sofa::type::Mat3x3d;

using sofa::type::Mat3x4f;
using sofa::type::Mat3x4d;

using sofa::type::Mat4x4f;
using sofa::type::Mat4x4d;

using sofa::type::Mat2x2;
using sofa::type::Mat3x3;
using sofa::type::Mat4x4;

using sofa::type::Matrix2;
using sofa::type::Matrix3;
using sofa::type::Matrix4;

template<class real>
inline real determinant(const Mat<3, 3, real>& m)
{
    return type::determinant(m);
}

template<class real>
inline real determinant(const Mat<2, 2, real>& m)
{
    return type::determinant(m);
}

template<class real>
inline real determinant(const Mat<2, 3, real>& m)
{
    return type::determinant(m);
}

template<class real>
inline real determinant(const Mat<3, 2, real>& m)
{
    return type::determinant(m);
}

template<class real>
inline real oneNorm(const Mat<3, 3, real>& A)
{
    return type::oneNorm(A);
}

template<class real>
inline real infNorm(const Mat<3, 3, real>& A)
{
    return type::infNorm(A);
}

template<sofa::Size N, class real>
inline real trace(const Mat<N, N, real>& m)
{
    return type::trace(m);
}

template<sofa::Size N, class real>
inline Vec<N, real> diagonal(const Mat<N, N, real>& m)
{
    return type::diagonal(m);
}

// keep the old behavior (i.e msg_error (!) + no check if the boolean is used)
/// Matrix inversion (general case).
template<sofa::Size S, class real>
bool invertMatrix(Mat<S, S, real>& dest, const Mat<S, S, real>& from)
{
    auto res = type::invertMatrix(dest, from);
    if(!res)
    {
        msg_error("Mat") << "invertMatrix (general case) finds too small determinant for matrix = " << from;
    }
    return res;
}

template<class real>
bool invertMatrix(Mat<3, 3, real>& dest, const Mat<3, 3, real>& from)
{
    auto res = type::invertMatrix(dest, from);
    if (!res)
    {
        msg_error("Mat") << "invertMatrix (special case 3x3) finds too small determinant for matrix = " << from;
    }
    return res;
}

template<class real>
bool invertMatrix(Mat<2, 2, real>& dest, const Mat<2, 2, real>& from)
{
    auto res = type::invertMatrix(dest, from);
    if (!res)
    {
        msg_error("Mat") << "invertMatrix (general case) finds too small determinant for matrix = " << from;
    }
    return res;
}

template<sofa::Size S, class real>
bool transformInvertMatrix(Mat<S, S, real>& dest, const Mat<S, S, real>& from)
{
    return type::transformInvertMatrix(dest, from);
}

template <sofa::Size L, sofa::Size C, typename real>
void printMatlab(std::ostream& o, const Mat<L, C, real>& m)
{
    type::printMatlab(o, m);
}

template <sofa::Size L, sofa::Size C, typename real>
void printMaple(std::ostream& o, const Mat<L, C, real>& m)
{
    type::printMaple(o, m);
}

template <sofa::Size L, sofa::Size C, typename T>
inline Mat<L, C, T> dyad(const Vec<L, T>& u, const Vec<C, T>& v)
{
    return type::dyad(u,v);
}

template <sofa::Size L, sofa::Size C, typename real>
inline real scalarProduct(const Mat<L, C, real>& left, const Mat<L, C, real>& right)
{
    return type::scalarProduct(left, right);
}

template<class Real>
inline Mat<3, 3, Real> crossProductMatrix(const Vec<3, Real>& v)
{
    return type::crossProductMatrix(v);
}


/// return a * b^T
template<sofa::Size L, class Real>
static Mat<L, L, Real> tensorProduct(const Vec<L, Real> a, const Vec<L, Real> b)
{
    return type::tensorProduct(a, b);
}

using Mat1x1f = sofa::type::Mat1x1f;
using Mat1x1d = sofa::type::Mat1x1d;

using Mat2x2f = sofa::type::Mat2x2f;
using Mat2x2d = sofa::type::Mat2x2d;

using Mat3x3f = sofa::type::Mat3x3f;
using Mat3x3d = sofa::type::Mat3x3d;

using Mat3x4f = sofa::type::Mat3x4f;
using Mat3x4d = sofa::type::Mat3x4d;

using Mat4x4f = sofa::type::Mat4x4f;
using Mat4x4d = sofa::type::Mat4x4d;

using Mat2x2 = sofa::type::Mat2x2;
using Mat3x3 = sofa::type::Mat3x3;
using Mat4x4 = sofa::type::Mat4x4;

using Matrix2 = sofa::type::Matrix2;
using Matrix3 = sofa::type::Matrix3;
using Matrix4 = sofa::type::Matrix4;

} // namespace sofa::defaulttype
