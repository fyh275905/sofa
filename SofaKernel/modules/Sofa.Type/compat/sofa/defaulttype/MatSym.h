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

#include <sofa/type/MatSym.h>

// The following SOFA_DEPRECATED_HEADER is commented to avoid a massive number of warnings.
// This flag will be enabled once all the code base in Sofa is ported to Sofa.Type.
// (PR #1790)
// SOFA_DEPRECATED_HEADER(v21.12, "sofa/type/MatSym.h")

namespace sofa::defaulttype
{
    template <int D, class real = float>
    using MatSym = sofa::type::MatSym<D, real>;

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

    template<class real>
    inline real determinant(const MatSym<3, real>& m)
    {
        return type::determinant(m);
    }

    template<class real>
    inline real determinant(const MatSym<2, real>& m)
    {
        return type::determinant(m);
    }

    template<class real>
    inline real trace(const MatSym<3, real>& m)
    {
        return type::trace(m);
    }

    template<class real>
    inline real trace(const MatSym<2, real>& m)
    {
        return type::trace(m);
    }

    template<int S, class real>
    bool invertMatrix(MatSym<S, real>& dest, const MatSym<S, real>& from)
    {
        auto res = type::invertMatrix(dest, from);
        if (!res)
        {
            msg_error("MatSym") << "invertMatrix (general case) finds too small determinant for matrix = " << from;
        }
        return res;
    }

    /// Matrix inversion (special case 3x3).
    template<class real>
    bool invertMatrix(MatSym<3, real>& dest, const MatSym<3, real>& from)
    {
        auto res = type::invertMatrix(dest, from);
        if (!res)
        {
            msg_error("MatSym") << "invertMatrix (special case 3x3) finds too small determinant for matrix = " << from;
        }
        return res;
    }

    /// Matrix inversion (special case 2x2).
    template<class real>
    bool invertMatrix(MatSym<2, real>& dest, const MatSym<2, real>& from)
    {
        auto res = type::invertMatrix(dest, from);
        if (!res)
        {
            msg_error("MatSym") << "invertMatrix (special case 2x2) finds too small determinant for matrix = " << from;
        }
        return res;
    }

    /// Compute the scalar product of two matrix (sum of product of all terms)
    template <int D, typename real>
    inline real scalarProduct(const MatSym<D, real>& left, const MatSym<D, real>& right)
    {
        return type::scalarProduct(left, right);
    }

    template <int D, typename real>
    inline real scalarProduct(const MatSym<D, real>& left, const type::Mat<D, D, real>& right)
    {
        return type::scalarProduct(left, right);
    }


} // namespace sofa::defaulttype
