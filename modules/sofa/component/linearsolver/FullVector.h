/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_COMPONENT_LINEARSOLVER_FULLVECTOR_H
#define SOFA_COMPONENT_LINEARSOLVER_FULLVECTOR_H

#include <sofa/defaulttype/BaseVector.h>
#include <sofa/helper/rmath.h>

namespace sofa
{

namespace component
{

namespace linearsolver
{

template<typename T>
class FullVector : public defaulttype::BaseVector
{
public:
    typedef T Real;
    typedef int Index;
    typedef T* Iterator;
    typedef const T* ConstIterator;

    typedef Real value_type;
    typedef Index size_type;
    typedef Iterator iterator;
    typedef ConstIterator const_iterator;

protected:
    T* data;
    Index cursize;
    Index allocsize;
public:

    FullVector()
        : data(NULL), cursize(0), allocsize(0)
    {
    }

    explicit FullVector(Index n)
        : data(new T[n]), cursize(n), allocsize(n)
    {
    }

    FullVector(T* ptr, Index n)
        : data(ptr), cursize(n), allocsize(-n)
    {
    }

    FullVector(T* ptr, Index n, Index nmax)
        : data(ptr), cursize(n), allocsize(-nmax)
    {
    }

    virtual ~FullVector()
    {
        if (allocsize>0)
            delete[] data;
    }

    T* ptr() { return data; }
    const T* ptr() const { return data; }

    void setptr(T* p) { ptr = p; }

    Index capacity() const { if (allocsize < 0) return -allocsize; else return allocsize; }

    Iterator begin() { return data; }
    Iterator end()   { return data+cursize; }

    ConstIterator begin() const { return data; }
    ConstIterator end()   const { return data+cursize; }

    void fastResize(int dim)
    {
        if (dim == cursize) return;
        if (allocsize >= 0)
        {
            if (dim > allocsize)
            {
                if (allocsize > 0)
                    delete[] data;
                allocsize = dim;
                data = new T[dim];
            }
        }
        else
        {
            if (dim > -allocsize)
            {
                std::cerr << "ERROR: cannot resize preallocated vector to size "<<dim<<std::endl;
                return;
            }
        }
        cursize = dim;
    }

    void resize(int dim)
    {
        fastResize(dim);
        clear();
    }

    void clear()
    {
        if (cursize > 0)
            std::fill( this->begin(), this->end(), T() );
    }

    T& operator[](Index i)
    {
        return data[i];
    }

    const T& operator[](Index i) const
    {
        return data[i];
    }

    double element(int i) const
    {
        return data[i];
    }

    void set(int i, Real_Sofa v)
    {
        data[i] = v;
    }

    void add(int i, Real_Sofa v)
    {
        data[i] += v;
    }

    int size() const
    {
        return cursize;
    }

    FullVector<T> sub(int i, int n)
    {
        return FullVector<T>(data+i,n);
    }

    template<class TV>
    void getsub(int i, int n, TV& v)
    {
        v = FullVector<T>(data+i,n);
    }

    template<class TV>
    void setsub(int i, int n, const TV& v)
    {
        FullVector<T>(data+i,n) = v;
    }

    /// v = a
    void operator=(const FullVector<T>& a)
    {
        fastResize(a.size());
        std::copy(a.begin(), a.end(), begin());
    }

    void operator=(T a)
    {
        fastResize(a.size());
        std::fill(a.begin(), a.end(), a);
    }

    /// v += a
    template<typename Real2>
    void operator+=(const FullVector<Real2>& a)
    {
        for(int i=0; i<cursize; ++i)
            (*this)[i] += (Real)a[i];
    }

    /// v += a*f
    template<typename Real2,typename Real3>
    void peq(const FullVector<Real2>& a, Real3 f)
    {
        for(int i=0; i<cursize; ++i)
            (*this)[i] += (Real)(a[i]*f);
    }

    /// v *= f
    template<typename Real2>
    void operator*=(Real2 f)
    {
        for(int i=0; i<cursize; ++i)
            (*this)[i] *= (Real)f;
    }

    /// \return v.a
    Real dot(const FullVector<Real>& a) const
    {
        Real r = 0;
        for(int i=0; i<cursize; ++i)
            r += (*this)[i]*a[i];
        return r;
    }

    /// \return sqrt(v.v)
    double norm() const
    {
        return helper::rsqrt(dot(*this));
    }

    friend std::ostream& operator << (std::ostream& out, const FullVector<Real>& v )
    {
        for (int i=0,s=v.size(); i<s; ++i)
        {
            if (i) out << ' ';
            out << v[i];
        }
        return out;
    }
};

} // namespace linearsolver

} // namespace component

} // namespace sofa

#endif
