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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_HELPER_VECTOR_H
#define SOFA_HELPER_VECTOR_H

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdlib.h>
#include <typeinfo>

#include <sofa/helper/helper.h>

namespace sofa
{

namespace helper
{

void SOFA_HELPER_API vector_access_failure(const void* vec, unsigned size, unsigned i, const std::type_info& type);

//======================================================================
/**	Same as std::vector, + range checking on operator[ ]

 Range checking can be turned of using compile option -DNDEBUG
\author Francois Faure, 1999
*/
//======================================================================
template<
class T,
      class Alloc = std::allocator<T>
      >
class vector: public std::vector<T,Alloc>
{
public:

    /// size_type
    typedef typename std::vector<T,Alloc>::size_type size_type;
    /// reference to a value (read-write)
    typedef typename std::vector<T,Alloc>::reference reference;
    /// const reference to a value (read only)
    typedef typename std::vector<T,Alloc>::const_reference const_reference;

    /// Basic onstructor
    vector() : std::vector<T,Alloc>() {}
    /// Constructor
    vector(size_type n, const T& value): std::vector<T,Alloc>(n,value) {}
    /// Constructor
    vector(int n, const T& value): std::vector<T,Alloc>(n,value) {}
    /// Constructor
    vector(long n, const T& value): std::vector<T,Alloc>(n,value) {}
    /// Constructor
    explicit vector(size_type n): std::vector<T,Alloc>(n) {}
    /// Constructor
    vector(const std::vector<T, Alloc>& x): std::vector<T,Alloc>(x) {}
    /// Constructor
    vector<T, Alloc>& operator=(const std::vector<T, Alloc>& x)
    {
        std::vector<T,Alloc>::operator = (x);
        return (*this);
    }

#ifdef __STL_MEMBER_TEMPLATES
    /// Constructor
    template <class InputIterator>
    vector(InputIterator first, InputIterator last): std::vector<T,Alloc>(first,last) {}
#else /* __STL_MEMBER_TEMPLATES */
    /// Constructor
    vector(typename vector<T,Alloc>::const_iterator first, typename vector<T,Alloc>::const_iterator last): std::vector<T,Alloc>(first,last) {}
#endif /* __STL_MEMBER_TEMPLATES */


/// Read/write random access
    reference operator[](size_type n)
    {
#ifndef NDEBUG
        if (n>=this->size())
            vector_access_failure(this, this->size(), n, typeid(T));
        //assert( n<this->size() );
#endif
        return *(this->begin() + n);
    }

/// Read-only random access
    const_reference operator[](size_type n) const
    {
#ifndef NDEBUG
        if (n>=this->size())
            vector_access_failure(this, this->size(), n, typeid(T));
        //assert( n<this->size() );
#endif
        return *(this->begin() + n);
    }

    std::ostream& write(std::ostream& os) const
    {
        if( this->size()>0 )
        {
            for( unsigned int i=0; i<this->size()-1; ++i ) os<<(*this)[i]<<" ";
            os<<(*this)[this->size()-1];
        }
        return os;
    }

    std::istream& read(std::istream& in)
    {
        T t;
        this->clear();
        while(in>>t)
        {
            this->push_back(t);
        }
        if( in.rdstate() & std::ios_base::eofbit ) { in.clear(); }
        return in;
    }

/// Output stream
    inline friend std::ostream& operator<< ( std::ostream& os, const vector<T,Alloc>& vec )
    {
        return vec.write(os);
    }

/// Input stream
    inline friend std::istream& operator>> ( std::istream& in, vector<T,Alloc>& vec )
    {
        return vec.read(in);
    }

    /// Sets every element to 'value'
    void fill( const T& value )
    {
        std::fill( this->begin(), this->end(), value );
    }

};

/// Input stream
/// Specialization for reading vectors of int and unsigned int using "A-B" notation for all integers between A and B, optionnally specifying a step using "A-B-step" notation.
template<>
inline std::istream& vector<int, std::allocator<int> >::read( std::istream& in )
{
    int t;
    this->clear();
    std::string s;
    while(in>>s)
    {
        std::string::size_type hyphen = s.find_first_of('-',1);
        if (hyphen == std::string::npos)
        {
            t = atoi(s.c_str());
            this->push_back(t);
        }
        else
        {
            int t1,t2,tinc;
            std::string s1(s,0,hyphen);
            t1 = atoi(s1.c_str());
            std::string::size_type hyphen2 = s.find_first_of('-',hyphen+2);
            if (hyphen2 == std::string::npos)
            {
                std::string s2(s,hyphen+1);
                t2 = atoi(s2.c_str());
                tinc = (t1<t2) ? 1 : -1;
            }
            else
            {
                std::string s2(s,hyphen+1,hyphen2);
                std::string s3(s,hyphen2+1);
                t2 = atoi(s2.c_str());
                tinc = atoi(s3.c_str());
                if (tinc == 0)
                {
                    std::cerr << "ERROR parsing \""<<s<<"\": increment is 0\n";
                    tinc = (t1<t2) ? 1 : -1;
                }
                if ((t2-t1)*tinc < 0)
                {
                    // increment not of the same sign as t2-t1 : swap t1<->t2
                    t = t1;
                    t1 = t2;
                    t2 = t;
                }
            }
            if (tinc < 0)
                for (t=t1; t>=t2; t+=tinc)
                    this->push_back(t);
            else
                for (t=t1; t<=t2; t+=tinc)
                    this->push_back(t);
        }
    }
    if( in.rdstate() & std::ios_base::eofbit ) { in.clear(); }
    return in;
}

/// Output stream
/// Specialization for writing vectors of unsigned char
template<>
inline std::ostream& vector<unsigned char, std::allocator<unsigned char> >::write(std::ostream& os) const
{
    if( this->size()>0 )
    {
        for( unsigned int i=0; i<this->size()-1; ++i ) os<<(int)(*this)[i]<<" ";
        os<<(int)(*this)[this->size()-1];
    }
    return os;
}

/// Inpu stream
/// Specialization for writing vectors of unsigned char
template<>
inline std::istream&  vector<unsigned char, std::allocator<unsigned char> >::read(std::istream& in)
{
    int t;
    this->clear();
    while(in>>t)
    {
        this->push_back((unsigned char)t);
    }
    if( in.rdstate() & std::ios_base::eofbit ) { in.clear(); }
    return in;
}

/// Input stream
/// Specialization for reading vectors of int and unsigned int using "A-B" notation for all integers between A and B
template<>
inline std::istream& vector<unsigned int, std::allocator<unsigned int> >::read( std::istream& in )
{
    unsigned int t;
    this->clear();
    std::string s;
    while(in>>s)
    {
        std::string::size_type hyphen = s.find_first_of('-',1);
        if (hyphen == std::string::npos)
        {
            t = atoi(s.c_str());
            this->push_back(t);
        }
        else
        {
            unsigned int t1,t2;
            int tinc;
            std::string s1(s,0,hyphen);
            t1 = (unsigned int)atoi(s1.c_str());
            std::string::size_type hyphen2 = s.find_first_of('-',hyphen+2);
            if (hyphen2 == std::string::npos)
            {
                std::string s2(s,hyphen+1);
                t2 = (unsigned int)atoi(s2.c_str());
                tinc = (t1<t2) ? 1 : -1;
            }
            else
            {
                std::string s2(s,hyphen+1,hyphen2);
                std::string s3(s,hyphen2+1);
                t2 = (unsigned int)atoi(s2.c_str());
                tinc = atoi(s3.c_str());
                if (tinc == 0)
                {
                    std::cerr << "ERROR parsing \""<<s<<"\": increment is 0\n";
                    tinc = (t1<t2) ? 1 : -1;
                }
                if (((int)(t2-t1))*tinc < 0)
                {
                    // increment not of the same sign as t2-t1 : swap t1<->t2
                    t = t1;
                    t1 = t2;
                    t2 = t;
                }
            }
            if (tinc < 0)
                for (t=t1; t>=t2; t=(unsigned int)((int)t+tinc))
                    this->push_back(t);
            else
                for (t=t1; t<=t2; t=(unsigned int)((int)t+tinc))
                    this->push_back(t);
        }
    }
    if( in.rdstate() & std::ios_base::eofbit ) { in.clear(); }
    return in;
}


// ======================  operations on standard vectors

// -----------------------------------------------------------
//
/*! @name vector class-related methods

*/
//
// -----------------------------------------------------------
//@{
/** Remove the first occurence of a given value.

The remaining values are shifted.
*/
template<class T1, class T2>
void remove( T1& v, const T2& elem )
{
    typename T1::iterator e = std::find( v.begin(), v.end(), elem );
    if( e != v.end() )
    {
        typename T1::iterator next = e;
        next++;
        for( ; next != v.end(); ++e, ++next )
            *e = *next;
    }
    v.pop_back();
}

/** Remove the first occurence of a given value.

The last value is moved to where the value was found, and the other values are not shifted.
*/
template<class T1, class T2>
void removeValue( T1& v, const T2& elem )
{
    typename T1::iterator e = std::find( v.begin(), v.end(), elem );
    if( e != v.end() )
    {
        *e = v.back();
        v.pop_back();
    }
}

/// Remove value at given index, replace it by the value at the last index, other values are not changed
template<class T, class TT>
void removeIndex( std::vector<T,TT>& v, size_t index )
{
#ifndef NDEBUG
    //assert( 0<= static_cast<int>(index) && index <v.size() );
    if (index>=v.size())
        vector_access_failure(&v, v.size(), index, typeid(T));
#endif
    v[index] = v.back();
    v.pop_back();
}

//@}

} // namespace helper

} // namespace sofa

/*
/// Output stream
template<class T, class Alloc>
  std::ostream& operator<< ( std::ostream& os, const std::vector<T,Alloc>& vec )
{
  if( vec.size()>0 ){
    for( unsigned int i=0; i<vec.size()-1; ++i ) os<<vec[i]<<" ";
    os<<vec[vec.size()-1];
  }
  return os;
}

/// Input stream
template<class T, class Alloc>
    std::istream& operator>> ( std::istream& in, std::vector<T,Alloc>& vec )
{
  T t;
  vec.clear();
  while(in>>t){
    vec.push_back(t);
  }
  if( in.rdstate() & std::ios_base::eofbit ) { in.clear(); }
  return in;
}

/// Input a pair
template<class T, class U>
    std::istream& operator>> ( std::istream& in, std::pair<T,U>& pair )
{
  in>>pair.first>>pair.second;
  return in;
}

/// Output a pair
template<class T, class U>
    std::ostream& operator<< ( std::ostream& out, const std::pair<T,U>& pair )
{
  out<<pair.first<<" "<<pair.second;
  return out;
}
*/

#endif


