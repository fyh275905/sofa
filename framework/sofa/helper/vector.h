/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
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
#include <sofa/helper/MemoryManager.h>
#include <sofa/defaulttype/DataTypeInfo.h>

#define ALL_DEVICE_VALID 0xFFFFFFFF

namespace sofa
{

namespace helper
{

template <class T, class MemoryManager = CPUMemoryManager<T> >
class vector
{
public:
    typedef T      value_type;
    typedef size_t size_type;
    typedef T&     reference;
    typedef const T& const_reference;
    typedef T*     iterator;
    typedef const T* const_iterator;

protected:
    size_type     vectorSize;     ///< Current size of the vector
    size_type     allocSize;      ///< Allocated size
    mutable void* devicePointer[MemoryManager::MAX_DEVICES];  ///< Pointer to the data on the GPU side
    T*            hostPointer;    ///< Pointer to the data on the CPU side
    GLuint        bufferObject;   ///< Optionnal associated OpenGL buffer ID
    mutable int   deviceIsValid;  ///< True if the data on the GPU is currently valid
    mutable bool  hostIsValid;    ///< True if the data on the CPU is currently valid
    mutable bool  bufferIsRegistered;  ///< True if the OpenGL buffer is registered with CUDA

public:

    vector()
        : vectorSize ( 0 ), allocSize ( 0 ), hostPointer ( NULL ), bufferObject(0), deviceIsValid ( ALL_DEVICE_VALID ), hostIsValid ( true ), bufferIsRegistered(false)
    {
        for (int d=0; d<MemoryManager::numDevices(); d++) devicePointer[d] = NULL;
    }
    vector ( size_type n )
        : vectorSize ( 0 ), allocSize ( 0 ), hostPointer ( NULL ), bufferObject(0), deviceIsValid ( ALL_DEVICE_VALID ), hostIsValid ( true ), bufferIsRegistered(false)
    {
        for (int d=0; d<MemoryManager::numDevices(); d++) devicePointer[d] = NULL;
        resize ( n );
    }
    vector ( const vector<T,MemoryManager >& v )
        : vectorSize ( 0 ), allocSize ( 0 ), hostPointer ( NULL ), bufferObject(0), deviceIsValid ( ALL_DEVICE_VALID ), hostIsValid ( true ), bufferIsRegistered(false)
    {
        for (int d=0; d<MemoryManager::numDevices(); d++) devicePointer[d] = NULL;
        *this = v;
    }

    bool isHostValid() const
    {
        return hostIsValid;
    }
    bool isDeviceValid(unsigned gpu) const
    {
        int mask = 1<<gpu;
        return (deviceIsValid & mask) != 0;
    }

    void clear()
    {
        vectorSize = 0;
        deviceIsValid = ALL_DEVICE_VALID;
        hostIsValid = true;
    }

    void operator= ( const vector<T,MemoryManager >& v )
    {
        if (&v == this)
        {
            //COMM : std::cerr << "ERROR: self-assignment of CudaVector< " << core::objectmodel::Base::decodeTypeName(typeid(T)) << ">"<<std::endl;
            return;
        }
        size_type newSize = v.size();
        clear();

        fastResize ( newSize );
        deviceIsValid = v.deviceIsValid;
        hostIsValid = v.hostIsValid;

        if ( vectorSize > 0 && deviceIsValid )
        {
            if (MemoryManager::SUPPORT_GL_BUFFER)
            {
                if (bufferObject) mapBuffer();//COMM : necessaire????
                if (v.bufferObject) v.mapBuffer();
            }

            for (int d=0; d<MemoryManager::numDevices(); d++)
            {
                if (devicePointer[d]!=NULL && v.devicePointer[d]!=NULL && v.isDeviceValid(d))
                {
                    MemoryManager::memcpyDeviceToDevice (d, devicePointer[d], v.devicePointer[d], vectorSize*sizeof ( T ) );
                }
            }
        }
        if ( vectorSize!=0 && hostIsValid )
            std::copy ( v.hostPointer, v.hostPointer+vectorSize, hostPointer );
    }

    ~vector()
    {
        if ( hostPointer!=NULL ) MemoryManager::hostFree ( hostPointer );

        if (MemoryManager::SUPPORT_GL_BUFFER && bufferObject )
        {
            unregisterBuffer();
            MemoryManager::bufferFree(bufferObject);
            devicePointer[MemoryManager::getBufferDevice()] = NULL; // already free
        }
        else
        {
            for (int d=0; d<MemoryManager::numDevices(); d++)
            {
                if ( devicePointer[d]!=NULL ) MemoryManager::deviceFree(d, devicePointer[d] );
            }
        }
    }

    size_type size() const
    {
        return vectorSize;
    }

    size_type capacity() const
    {
        return allocSize;
    }

    bool empty() const
    {
        return vectorSize==0;
    }

    void reserve (size_type s,size_type WARP_SIZE=MemoryManager::BSIZE)
    {
        if ( s <= allocSize ) return;
        allocSize = ( s>2*allocSize ) ?s:2*allocSize;
        // always allocate multiples of BSIZE values
        allocSize = ( allocSize+WARP_SIZE-1 ) & (size_type)(-(long)WARP_SIZE);

        if (MemoryManager::SUPPORT_GL_BUFFER && bufferObject)
        {
            //COMM : if (mycudaVerboseLevel>=LOG_INFO) std::cout << "CudaVector<"<<sofa::core::objectmodel::Base::className((T*)NULL)<<"> : GL reserve("<<s<<")"<<std::endl;
            hostRead(); // make sure the host copy is valid
            unregisterBuffer();
            //COMM fct opengl
            glBindBuffer( GL_ARRAY_BUFFER, bufferObject);
            glBufferData( GL_ARRAY_BUFFER, allocSize*sizeof ( T ), 0, GL_DYNAMIC_DRAW);
            glBindBuffer( GL_ARRAY_BUFFER, 0);
            if ( vectorSize > 0 && deviceIsValid ) deviceIsValid = 0;
        }
        else
        {
            for (int d=0; d<MemoryManager::numDevices(); d++)
            {
                void* prevDevicePointer = devicePointer[d];
                //COMM : if (mycudaVerboseLevel>=LOG_INFO) std::cout << "CudaVector<"<<sofa::core::objectmodel::Base::className((T*)NULL)<<"> : reserve("<<s<<")"<<std::endl;
                MemoryManager::deviceAlloc(d, &devicePointer[d], allocSize*sizeof ( T ) );
                if ( vectorSize > 0 && isDeviceValid(d)) MemoryManager::memcpyDeviceToDevice (d, devicePointer[d], prevDevicePointer, vectorSize*sizeof ( T ) );
                if ( prevDevicePointer != NULL ) MemoryManager::deviceFree (d, prevDevicePointer );
            }
        }

        T* prevHostPointer = hostPointer;
        void* newHostPointer = NULL;
        MemoryManager::hostAlloc( &newHostPointer, allocSize*sizeof ( T ) );
        hostPointer = (T*)newHostPointer;
        if ( vectorSize!=0 && hostIsValid ) std::copy ( prevHostPointer, prevHostPointer+vectorSize, hostPointer );
        if ( prevHostPointer != NULL ) MemoryManager::hostFree( prevHostPointer );
    }

    /// resize the vector without calling constructors or destructors, and without synchronizing the device and host copy
    void fastResize ( size_type s,size_type WARP_SIZE=MemoryManager::BSIZE)
    {
        if ( s == vectorSize ) return;
        reserve ( s,WARP_SIZE);
        vectorSize = s;
        if ( !vectorSize )
        {
            // special case when the vector is now empty -> host and device are valid
            deviceIsValid = ALL_DEVICE_VALID;
            hostIsValid = true;
        }
    }
    /// resize the vector discarding any old values, without calling constructors or destructors, and without synchronizing the device and host copy
    void recreate( size_type s,size_type WARP_SIZE=MemoryManager::BSIZE)
    {
        clear();
        fastResize(s,WARP_SIZE);
    }

    void memsetDevice(int v = 0)
    {
        for (int d=0; d<MemoryManager::numDevices(); d++) MemoryManager::memsetDevice(d, devicePointer[d], v, vectorSize*sizeof(T));
        hostIsValid = false;
        deviceIsValid = ALL_DEVICE_VALID;
    }

    void memsetHost(int v = 0)
    {
        MemoryManager::memsetHost(hostPointer,v,vectorSize*sizeof(T));
        hostIsValid = true;
        deviceIsValid = 0;
    }

    void resize ( size_type s,size_type WARP_SIZE=MemoryManager::BSIZE)
    {
        if ( s == vectorSize ) return;
        reserve ( s,WARP_SIZE);
        if ( s > vectorSize )
        {
            if (sofa::defaulttype::DataTypeInfo<T>::ZeroConstructor )   // can use memset instead of constructors
            {
                if (hostIsValid) MemoryManager::memsetHost(hostPointer+vectorSize,0,(s-vectorSize)*sizeof(T));

                for (int d=0; d<MemoryManager::numDevices(); d++)
                {
                    if (isDeviceValid(d)) MemoryManager::memsetDevice(d,(void *) (((T*)devicePointer[d])+vectorSize), 0, (s-vectorSize)*sizeof(T));
                }
            }
            else
            {
                copyToHost();
                memset(hostPointer+vectorSize,0,(s-vectorSize)*sizeof(T));
                // Call the constructor for the new elements
                for ( size_type i = vectorSize; i < s; i++ ) ::new ( hostPointer+i ) T;

                if ( vectorSize == 0 )   // wait until the transfer is really necessary, as other modifications might follow
                {
                    deviceIsValid = 0;
                }
                else
                {
                    if (MemoryManager::SUPPORT_GL_BUFFER && bufferObject) mapBuffer();

                    for (int d=0; d<MemoryManager::numDevices(); d++)
                    {
                        if (devicePointer[d]!=NULL &&  isDeviceValid(d) )
                            MemoryManager::memcpyHostToDevice(d, ((T*) devicePointer[d] ) +vectorSize, hostPointer+vectorSize, ( s-vectorSize ) *sizeof ( T ) );
                    }
                }
            }
        }
        else if (s < vectorSize && !(defaulttype::DataTypeInfo<T>::SimpleCopy))     // need to call destructors
        {
            copyToHost();
            // Call the destructor for the deleted elements
            for ( size_type i = s; i < vectorSize; i++ )
            {
                hostPointer[i].~T();
            }
        }
        vectorSize = s;

        if ( !vectorSize )   // special case when the vector is now empty -> host and device are valid
        {
            deviceIsValid = ALL_DEVICE_VALID;
            hostIsValid = true;
        }
    }

    void swap ( vector<T,MemoryManager>& v )
    {
#define VSWAP(type, var) { type t = var; var = v.var; v.var = t; }
        VSWAP ( size_type, vectorSize );
        VSWAP ( size_type, allocSize );
        for (int d=0; d<MemoryManager::numDevices(); d++) VSWAP ( void*    , devicePointer[d] );
        VSWAP ( T*       , hostPointer );
        VSWAP ( GLuint   , bufferObject );
        VSWAP ( int      , deviceIsValid );
        VSWAP ( bool     , hostIsValid );
        VSWAP ( bool     , bufferIsRegistered );
#undef VSWAP
    }

    const void* deviceReadAt ( int i ,int gpu = MemoryManager::getBufferDevice()) const
    {
        copyToDevice(gpu);
        return ( ( const T* ) devicePointer[gpu] ) +i;
    }

    const void* deviceRead ( int gpu = MemoryManager::getBufferDevice()) const { return deviceReadAt(0,gpu); }

    void* deviceWriteAt ( int i ,int gpu = MemoryManager::getBufferDevice())
    {
        copyToDevice(gpu);
        hostIsValid = false;
        int mask = 1<<gpu;
        deviceIsValid = mask;
        return ( ( T* ) devicePointer[gpu] ) +i;
    }

    void* deviceWrite (int gpu = MemoryManager::getBufferDevice()) { return deviceWriteAt(0,gpu); }

    const T* hostRead ( int i=0 ) const
    {
        copyToHost();
        return hostPointer+i;
    }

    T* hostWrite ( int i=0 )
    {
        copyToHost();
        deviceIsValid = 0;
        return hostPointer+i;
    }

    /// Get the OpenGL Buffer Object ID for reading
    GLuint bufferRead(bool create = false)
    {
        if (MemoryManager::SUPPORT_GL_BUFFER)
        {
            if (!bufferObject)
            {
                if (create) createBuffer();
                else return 0;
            }
            copyToDevice();
            return bufferObject;
        }
        return 0;
    }

    /// Get the OpenGL Buffer Object ID for writing
    GLuint bufferWrite(bool create = false)
    {
        if (MemoryManager::SUPPORT_GL_BUFFER)
        {
            if (!bufferObject)
            {
                if (create) createBuffer();
                else return 0;
            }
            copyToDevice();
            unmapBuffer();
            hostIsValid = false;
            int mask = 1<<MemoryManager::getBufferDevice();
            deviceIsValid = mask;
            return bufferObject;
        }
    }

    void push_back ( const T& t )
    {
        size_type i = size();
        copyToHost();
        deviceIsValid = 0;
        fastResize ( i+1 );
        ::new ( hostPointer+i ) T ( t );
    }

    void pop_back()
    {
        if (!empty()) resize ( size()-1 );
    }

    const T& operator[] ( size_type i ) const
    {
        checkIndex ( i );
        return hostRead() [i];
    }

    T& operator[] ( size_type i )
    {
        checkIndex ( i );
        return hostWrite() [i];
    }

    const T& getCached ( size_type i ) const
    {
        checkIndex ( i );
        return hostPointer[i];
    }

    const T& getSingle ( size_type i ) const
    {
        copyToHostSingle(i);
        return hostPointer[i];
    }

    const_iterator begin() const { return hostRead(); }
    const_iterator end() const { return hostRead()+size(); }

    iterator begin() { return hostWrite(); }
    iterator end() { return hostWrite()+size(); }

    /// Output stream
    inline friend std::ostream& operator<< ( std::ostream& os, const vector<T,MemoryManager>& vec )
    {
        if ( vec.size() >0 )
        {
            for ( unsigned int i=0; i<vec.size()-1; ++i ) os<<vec[i]<<" ";
            os<<vec[vec.size()-1];
        }
        return os;
    }

    /// Input stream
    inline friend std::istream& operator>> ( std::istream& in, vector<T,MemoryManager>& vec )
    {
        T t;
        vec.clear();
        while ( in>>t )
        {
            vec.push_back ( t );
        }
        if ( in.rdstate() & std::ios_base::eofbit ) { in.clear(); }
        return in;
    }



protected:
    void copyToHost() const
    {
        if ( hostIsValid ) return;
//#ifndef NDEBUG
        // COMM : if (mycudaVerboseLevel>=LOG_TRACE) {
        // COMM :     std::cout << "CUDA: GPU->CPU copy of "<<sofa::core::objectmodel::Base::decodeTypeName ( typeid ( *this ) ) <<": "<<vectorSize*sizeof ( T ) <<" B"<<std::endl;
        //sofa::helper::BackTrace::dump();
        // COMM : }
//#endif

        if (MemoryManager::SUPPORT_GL_BUFFER && bufferObject) mapBuffer();

        for (int d=0; d<MemoryManager::numDevices(); d++)
        {
            if (devicePointer[d]!=NULL && isDeviceValid(d))
            {
                MemoryManager::memcpyDeviceToHost (d, hostPointer, devicePointer[d], vectorSize*sizeof ( T ) );
                hostIsValid = true;
                return;
            }
        }
    }

    void copyToDevice(int d = 0) const
    {
        if (MemoryManager::SUPPORT_GL_BUFFER && bufferObject) mapBuffer();
        if (isDeviceValid(d)) return;
//#ifndef NDEBUG
        //COMM : if (mycudaVerboseLevel>=LOG_TRACE) std::cout << "CUDA: CPU->GPU copy of "<<sofa::core::objectmodel::Base::decodeTypeName ( typeid ( *this ) ) <<": "<<vectorSize*sizeof ( T ) <<" B"<<std::endl;
//#endif
        if ( !hostIsValid ) copyToHost();
        MemoryManager::memcpyHostToDevice (d, devicePointer[d], hostPointer, vectorSize*sizeof ( T ) );
        int mask = 1<<d;
        deviceIsValid = deviceIsValid | mask;
    }

    void copyToHostSingle(size_type i) const
    {
        if ( hostIsValid ) return;

//#ifndef NDEBUG
        //COMM : if (mycudaVerboseLevel>=LOG_TRACE) {
        // COMM : std::cout << "CUDA: GPU->CPU single copy of "<<sofa::core::objectmodel::Base::decodeTypeName ( typeid ( *this ) ) <<": "<<sizeof ( T ) <<" B"<<std::endl;
        //sofa::helper::BackTrace::dump();
        //COMM : }
//#endif

        if (MemoryManager::SUPPORT_GL_BUFFER && bufferObject) mapBuffer();
        for (int d=0; d<MemoryManager::numDevices(); d++)
        {
            if (devicePointer[d]!=NULL && isDeviceValid(d))
            {
                MemoryManager::memcpyDeviceToHost(d, ((T*)hostPointer)+i, ((const T*)devicePointer[d])+i, sizeof ( T ) );
                return;
            }
        }
    }

#ifdef NDEBUG
    void checkIndex ( size_type ) const {}
#else
    void checkIndex ( size_type i ) const
    {
        assert ( i<this->size() );
    }
#endif

    void registerBuffer() const
    {
        if (MemoryManager::SUPPORT_GL_BUFFER)
        {
            if (this->allocSize > 0 && !bufferIsRegistered)
            {
                bufferIsRegistered = MemoryManager::bufferRegister(bufferObject);
            }
        }
    }

    void mapBuffer() const
    {
        if (MemoryManager::SUPPORT_GL_BUFFER)
        {
            registerBuffer();
            if (bufferIsRegistered)
            {
                if (this->allocSize > 0 && this->devicePointer[MemoryManager::getBufferDevice()]==NULL)
                {
                    MemoryManager::bufferMapToDevice(&this->devicePointer[MemoryManager::getBufferDevice()], bufferObject);
                }
            }
            else
            {
                std::cout << "CUDA: Unable to map buffer to opengl" << std::endl;
            }
        }
    }

    void unmapBuffer() const
    {
        if (MemoryManager::SUPPORT_GL_BUFFER)
        {
            if (this->allocSize > 0 && this->devicePointer[MemoryManager::getBufferDevice()] != NULL)
            {
                MemoryManager::bufferUnmapToDevice(&this->devicePointer[MemoryManager::getBufferDevice()], bufferObject);
                this->devicePointer[MemoryManager::getBufferDevice()] = NULL;
            }
        }
    }

    void unregisterBuffer() const
    {
        if (MemoryManager::SUPPORT_GL_BUFFER)
        {
            unmapBuffer();
            if (this->allocSize > 0 && bufferIsRegistered)
            {
                MemoryManager::bufferUnregister(bufferObject);
                bufferIsRegistered = false;
            }
        }
    }

    void createBuffer()
    {
        if (MemoryManager::SUPPORT_GL_BUFFER)
        {
            if (bufferObject) return;

            if (this->allocSize > 0)
            {
                MemoryManager::bufferAlloc(&bufferObject,this->allocSize*sizeof(T));

                void* prevDevicePointer = this->devicePointer[MemoryManager::getBufferDevice()];
                this->devicePointer[MemoryManager::getBufferDevice()] = NULL;
                if (this->vectorSize>0 && this->isDeviceValid(MemoryManager::getBufferDevice()))
                {
                    deviceRead(MemoryManager::getBufferDevice());//check datas are on device MemoryManager::getBufferDevice()
                    mapBuffer();
                    if (prevDevicePointer) MemoryManager::memcpyDeviceToDevice (MemoryManager::getBufferDevice(), devicePointer[MemoryManager::getBufferDevice()], prevDevicePointer, vectorSize*sizeof ( T ) );
                }
                if ( prevDevicePointer != NULL ) MemoryManager::deviceFree(MemoryManager::getBufferDevice(), prevDevicePointer);
            }
        }
    }

};

//classic vector (using CPUMemoryManager, same behavior as std::helper)
template <class T>
class vector<T, CPUMemoryManager<T> > : public std::vector<T, std::allocator<T> >
{
public:
    typedef std::allocator<T> Alloc;
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
        //std::vector<T,Alloc>::operator = (x);
        return vector(x);
    }

#ifdef __STL_MEMBER_TEMPLATES
    /// Constructor
    template <class InputIterator>
    vector(InputIterator first, InputIterator last): std::vector<T,Alloc>(first,last) {}
#else /* __STL_MEMBER_TEMPLATES */
    /// Constructor
    vector(typename vector<T>::const_iterator first, typename vector<T>::const_iterator last): std::vector<T>(first,last) {}
#endif /* __STL_MEMBER_TEMPLATES */


#ifndef SOFA_NO_VECTOR_ACCESS_FAILURE

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

#endif // SOFA_NO_VECTOR_ACCESS_FAILURE


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
        T t=T();
        this->clear();
        while(in>>t)
        {
            this->push_back(t);
        }
        if( in.rdstate() & std::ios_base::eofbit ) { in.clear(); }
        return in;
    }

/// Output stream
    inline friend std::ostream& operator<< ( std::ostream& os, const vector<T>& vec )
    {
        return vec.write(os);
    }

/// Input stream
    inline friend std::istream& operator>> ( std::istream& in, vector<T>& vec )
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
inline std::istream& vector<int >::read( std::istream& in )
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
inline std::ostream& vector<unsigned char >::write(std::ostream& os) const
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
inline std::istream&  vector<unsigned char >::read(std::istream& in)
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
inline std::istream& vector<unsigned int >::read( std::istream& in )
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
#if !defined(NDEBUG) && !defined(SOFA_NO_VECTOR_ACCESS_FAILURE)
    //assert( 0<= static_cast<int>(index) && index <v.size() );
    if (index>=v.size())
        vector_access_failure(&v, v.size(), index, typeid(T));
#endif
    v[index] = v.back();
    v.pop_back();
}


} // namespace helper

} // namespace sofa

#endif //SOFA_HELPER_VECTOR_H


