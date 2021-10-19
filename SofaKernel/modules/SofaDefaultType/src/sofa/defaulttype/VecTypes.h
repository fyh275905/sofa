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
#ifndef SOFA_DEFAULTTYPE_VECTYPES_H
#define SOFA_DEFAULTTYPE_VECTYPES_H

#include <sofa/defaulttype/fwd.h>
#include <sofa/type/Vec.h>
#include <sofa/type/vector.h>
#include <sofa/helper/random.h>
#include <sofa/defaulttype/MapMapSparseMatrix.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <sofa/helper/logging/Messaging.h>
namespace sofa
{

namespace defaulttype
{


template<class TCoord, class TDeriv, class TReal>
class StdVectorTypes
{
public:
    typedef TCoord Coord;
    typedef TDeriv Deriv;
    typedef TReal Real;
    typedef type::vector<Coord> VecCoord;
    typedef type::vector<Deriv> VecDeriv;
    typedef type::vector<Real> VecReal;

    enum { spatial_dimensions = Coord::spatial_dimensions };
    enum { coord_total_size = Coord::total_size };
    enum { deriv_total_size = Deriv::total_size };

    typedef typename TCoord::Size Size;

    typedef Coord CPos;
    static const CPos& getCPos(const Coord& c) { return c; }
    static void setCPos(Coord& c, const CPos& v) { c = v; }
    typedef Deriv DPos;
    static const DPos& getDPos(const Deriv& d) { return d; }
    static void setDPos(Deriv& d, const DPos& v) { d = v; }

    typedef MapMapSparseMatrix<Deriv> MatrixDeriv;


protected:

    /// @internal size dependant specializations
    /// @{

    template<Size N, class T>
    struct Impl
    {
        static void set( Coord& c, T x, T y, T z )
        {
            if constexpr (N > 2)
            {
                c[0] = Real(x);
                c[1] = Real(y);
                c[2] = Real(z);
            }
            if constexpr (N == 2)
            {
                SOFA_UNUSED(z);
                c[0] = Real(x);
                c[1] = Real(y);
            }
            if constexpr (N == 1)
            {
                SOFA_UNUSED(y);
                SOFA_UNUSED(z);
                c[0] = Real(x);
            }
        }

        static void get( T& x, T& y, T& z, const Coord& c )
        {
            if constexpr(N > 2)
            {
                x = T(c[0]);
                y = T(c[1]);
                z = T(c[2]);
            }
            if constexpr (N == 2)
            {
                SOFA_UNUSED(z);
                x = T(c[0]);
                y = T(c[1]);
                z = T(0);
            }
            if constexpr (N == 1)
            {
                SOFA_UNUSED(y);
                SOFA_UNUSED(z);
                x = T(c[0]);
                y = T(0);
                z = T(0);
            }
        }

        static void add( Coord& c, T x, T y, T z )
        {
            if constexpr (N > 2)
            {
                c[0] += Real(x);
                c[1] += Real(y);
                c[2] += Real(z);
            }
            if constexpr (N == 2)
            {
                SOFA_UNUSED(z);
                c[0] += Real(x);
                c[1] += Real(y);
            }
            if constexpr (N == 1)
            {
                SOFA_UNUSED(y);
                SOFA_UNUSED(z);
                c[0] += Real(x);
            }
        }
    };

    ///@}



public:

    template<typename T>
    static void set(Coord& c, T x, T y, T z)
    {
        Impl<spatial_dimensions,T>::set(c,x,y,z);
    }

    template<typename T>
    static void get(T& x, T& y, T& z, const Coord& c)
    {
        Impl<spatial_dimensions,T>::get(x,y,z,c);
    }

    /// Return a Deriv with random value. Each entry with magnitude smaller than the given value.
    static Deriv randomDeriv( Real minMagnitude, Real maxMagnitude )
    {
        Deriv result;
        set( result, Real(helper::drand(minMagnitude,maxMagnitude)), Real(helper::drand(minMagnitude,maxMagnitude)), Real(helper::drand(minMagnitude,maxMagnitude)) );
        return result;
    }

    static Deriv coordDifference(const Coord& c1, const Coord& c2)
    {
        return (Deriv)(c1-c2);
    }

    template<typename T>
    static void add(Coord& c, T x, T y, T z)
    {
        Impl<spatial_dimensions,T>::add(c,x,y,z);
    }

    static const char* Name();

    static Coord interpolate(const type::vector< Coord > &ancestors, const type::vector< Real > &coefs)
    {
        assert(ancestors.size() == coefs.size());

        Coord c;

        for (std::size_t i = 0; i < ancestors.size(); i++)
        {
            c += ancestors[i] * coefs[i];
        }

        return c;
    }
};


/// 3D DOFs, double precision
typedef StdVectorTypes<type::Vec3d,type::Vec3d,double> Vec3dTypes;
template<> inline const char* Vec3dTypes::Name() { return "Vec3d"; }

/// 2D DOFs, double precision
typedef StdVectorTypes<type::Vec2d, type::Vec2d,double> Vec2dTypes;
template<> inline const char* Vec2dTypes::Name() { return "Vec2d"; }

/// 1D DOFs, double precision
typedef StdVectorTypes<type::Vec1d, type::Vec1d,double> Vec1dTypes;
template<> inline const char* Vec1dTypes::Name() { return "Vec1d"; }

/// 6D DOFs, double precision
typedef StdVectorTypes<type::Vec6d, type::Vec6d,double> Vec6dTypes;
template<> inline const char* Vec6dTypes::Name() { return "Vec6d"; }

/// 3f DOFs, single precision
typedef StdVectorTypes<type::Vec3f, type::Vec3f,float> Vec3fTypes;
template<> inline const char* Vec3fTypes::Name() { return "Vec3f"; }

/// 2f DOFs, single precision
typedef StdVectorTypes<type::Vec2f, type::Vec2f,float> Vec2fTypes;
template<> inline const char* Vec2fTypes::Name() { return "Vec2f"; }

/// 1f DOFs, single precision
typedef StdVectorTypes<type::Vec1f, type::Vec1f,float> Vec1fTypes;
template<> inline const char* Vec1fTypes::Name() { return "Vec1f"; }

/// 6f DOFs, single precision
typedef StdVectorTypes<type::Vec6f, type::Vec6f,float> Vec6fTypes;
template<> inline const char* Vec6fTypes::Name() { return "Vec6f"; }

/// 6D DOFs, double precision (default)
typedef StdVectorTypes<type::Vec6, type::Vec6, type::Vec6::value_type> Vec6Types;
/// 3D DOFs, double precision (default)
typedef StdVectorTypes<type::Vec3, type::Vec3, type::Vec3::value_type> Vec3Types;
/// 2D DOFs, double precision (default)
typedef StdVectorTypes<type::Vec2, type::Vec2, type::Vec2::value_type> Vec2Types;
/// 1D DOFs, double precision (default)
typedef StdVectorTypes<type::Vec1, type::Vec1, type::Vec1::value_type> Vec1Types;


} // namespace defaulttype

} // namespace sofa

#endif
