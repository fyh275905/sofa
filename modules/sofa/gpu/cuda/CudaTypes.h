#ifndef SOFA_GPU_CUDA_CUDATYPES_H
#define SOFA_GPU_CUDA_CUDATYPES_H

//#include "host_runtime.h" // CUDA
#include "CudaCommon.h"
#include "mycuda.h"
#include <sofa/defaulttype/Vec.h>
#include <sofa/helper/vector.h>
#include <sofa/core/objectmodel/Base.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <iostream>

namespace sofa
{

namespace gpu
{

namespace cuda
{

template<class T>
class CudaVector
{
public:
    typedef T      value_type;
    typedef size_t size_type;

protected:
    size_type    vectorSize;     ///< Current size of the vector
    size_type    allocSize;      ///< Allocated size
    void*        devicePointer;  ///< Pointer to the data on the GPU side
    T*           hostPointer;    ///< Pointer to the data on the CPU side
    mutable bool deviceIsValid;  ///< True if the data on the GPU is currently valid
    mutable bool hostIsValid;    ///< True if the data on the CPU is currently valid

public:

    CudaVector()
        : vectorSize ( 0 ), allocSize ( 0 ), devicePointer ( NULL ), hostPointer ( NULL ), deviceIsValid ( true ), hostIsValid ( true )
    {}
    CudaVector ( size_type n )
        : vectorSize ( 0 ), allocSize ( 0 ), devicePointer ( NULL ), hostPointer ( NULL ), deviceIsValid ( true ), hostIsValid ( true )
    {
        resize ( n );
    }
    CudaVector ( const CudaVector<T>& v )
        : vectorSize ( 0 ), allocSize ( 0 ), devicePointer ( NULL ), hostPointer ( NULL ), deviceIsValid ( true ), hostIsValid ( true )
    {
        *this = v;
    }
    void clear()
    {
        vectorSize = 0;
        deviceIsValid = true;
        hostIsValid = true;
    }

    void operator= ( const CudaVector<T>& v )
    {
        clear();
        fastResize ( v.size() );
        deviceIsValid = v.deviceIsValid;
        hostIsValid = v.hostIsValid;
        if ( vectorSize!=0 && deviceIsValid )
            mycudaMemcpyDeviceToDevice ( devicePointer, v.devicePointer, vectorSize*sizeof ( T ) );
        if ( vectorSize!=0 && hostIsValid )
            std::copy ( v.hostPointer, v.hostPointer+vectorSize, hostPointer );
    }
    ~CudaVector()
    {
        if ( hostPointer!=NULL )
            mycudaFreeHost ( hostPointer );
        if ( devicePointer!=NULL )
            mycudaFree ( devicePointer );
    }
    size_type size() const
    {
        return vectorSize;
    }
    bool empty() const
    {
        return vectorSize==0;
    }
    void reserve ( size_type s )
    {
        if ( s <= allocSize ) return;
        allocSize = ( s>2*allocSize ) ?s:2*allocSize;
        // always allocate multiples of BSIZE values
        allocSize = ( allocSize+BSIZE-1 ) &-BSIZE;

        void* prevDevicePointer = devicePointer;
        mycudaMalloc ( &devicePointer, allocSize*sizeof ( T ) );
        if ( vectorSize > 0 && deviceIsValid )
            mycudaMemcpyDeviceToDevice ( devicePointer, prevDevicePointer, vectorSize*sizeof ( T ) );
        if ( prevDevicePointer != NULL )
            mycudaFree ( prevDevicePointer );

        T* prevHostPointer = hostPointer;
        void* newHostPointer = NULL;
        mycudaMallocHost ( &newHostPointer, allocSize*sizeof ( T ) );
        hostPointer = (T*)newHostPointer;
        if ( vectorSize!=0 && hostIsValid )
            std::copy ( prevHostPointer, prevHostPointer+vectorSize, hostPointer );
        if ( prevHostPointer != NULL )
            mycudaFreeHost ( prevHostPointer );
    }
    /// resize the vector without calling constructors or destructors, and without synchronizing the device and host copy
    void fastResize ( size_type s )
    {
        if ( s == vectorSize ) return;
        reserve ( s );
        vectorSize = s;
        if ( !vectorSize )
        {
            // special case when the vector is now empty -> host and device are valid
            deviceIsValid = true;
            hostIsValid = true;
        }
    }
    void resize ( size_type s )
    {
        if ( s == vectorSize ) return;
        reserve ( s );
        if ( s > vectorSize )
        {
            // Call the constructor for the new elements
            for ( size_type i = vectorSize; i < s; i++ )
            {
                ::new ( hostPointer+i ) T;
            }
            if ( deviceIsValid )
            {
                if ( vectorSize == 0 )
                {
                    // wait until the transfer is really necessary, as other modifications might follow
                    deviceIsValid = false;
                }
                else
                {
                    mycudaMemcpyHostToDevice ( ( ( T* ) devicePointer ) +vectorSize, hostPointer+vectorSize, ( s-vectorSize ) *sizeof ( T ) );
                }
            }
        }
        else
        {
            // Call the destructor for the deleted elements
            for ( size_type i = s; i < vectorSize; i++ )
            {
                hostPointer[i].~T();
            }
        }
        vectorSize = s;
        if ( !vectorSize )
        {
            // special case when the vector is now empty -> host and device are valid
            deviceIsValid = true;
            hostIsValid = true;
        }
    }
    void swap ( CudaVector<T>& v )
    {
#define VSWAP(type, var) { type t = var; var = v.var; v.var = t; }
        VSWAP ( size_type, vectorSize );
        VSWAP ( size_type, allocSize );
        VSWAP ( void*    , devicePointer );
        VSWAP ( T*       , hostPointer );
        VSWAP ( bool     , deviceIsValid );
        VSWAP ( bool     , hostIsValid );
#undef VSWAP
    }
    const void* deviceRead ( int i=0 ) const
    {
        copyToDevice();
        return ( ( const T* ) devicePointer ) +i;
    }
    void* deviceWrite ( int i=0 )
    {
        copyToDevice();
        hostIsValid = false;
        return ( ( T* ) devicePointer ) +i;
    }
    const T* hostRead ( int i=0 ) const
    {
        copyToHost();
        return hostPointer+i;
    }
    T* hostWrite ( int i=0 )
    {
        copyToHost();
        deviceIsValid = false;
        return hostPointer+i;
    }
    bool isHostValid() const
    {
        return hostIsValid;
    }
    bool isDeviceValid() const
    {
        return deviceIsValid;
    }
    void push_back ( const T& t )
    {
        size_type i = size();
        copyToHost();
        deviceIsValid = false;
        fastResize ( i+1 );
        ::new ( hostPointer+i ) T ( t );
    }
    void pop_back()
    {
        if ( !empty() )
            resize ( size()-1 );
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

    /// Output stream
    inline friend std::ostream& operator<< ( std::ostream& os, const CudaVector<T>& vec )
    {
        if ( vec.size() >0 )
        {
            for ( unsigned int i=0; i<vec.size()-1; ++i ) os<<vec[i]<<" ";
            os<<vec[vec.size()-1];
        }
        return os;
    }

    /// Input stream
    inline friend std::istream& operator>> ( std::istream& in, CudaVector<T>& vec )
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
#ifndef NDEBUG
        std::cout << "CUDA: GPU->CPU copy of "<<sofa::core::objectmodel::Base::decodeTypeName ( typeid ( *this ) ) <<": "<<vectorSize*sizeof ( T ) <<" B"<<std::endl;
#endif
        mycudaMemcpyDeviceToHost ( hostPointer, devicePointer, vectorSize*sizeof ( T ) );
        hostIsValid = true;
    }
    void copyToDevice() const
    {
        if ( deviceIsValid ) return;
#ifndef NDEBUG
        std::cout << "CUDA: CPU->GPU copy of "<<sofa::core::objectmodel::Base::decodeTypeName ( typeid ( *this ) ) <<": "<<vectorSize*sizeof ( T ) <<" B"<<std::endl;
#endif
        mycudaMemcpyHostToDevice ( devicePointer, hostPointer, vectorSize*sizeof ( T ) );
        deviceIsValid = true;
    }
#ifdef NDEBUG
    void checkIndex ( size_type ) const
    {
    }
#else
    void checkIndex ( size_type i ) const
    {
        assert ( i<this->size() );
    }
#endif

};

template<class T>
class CudaMatrix
{
public:
    typedef T      value_type;
    typedef size_t size_type;

protected:
    size_type    sizeX;     ///< Current size of the vector
    size_type    sizeY;     ///< Current size of the vector
    size_type    pitch;     ///< Row alignment on the GPU
    size_type    hostAllocSize;  ///< Allocated size
    size_type    deviceAllocSize;///< Allocated size
    void*        devicePointer;  ///< Pointer to the data on the GPU side
    T*           hostPointer;    ///< Pointer to the data on the CPU side
    mutable bool deviceIsValid;  ///< True if the data on the GPU is currently valid
    mutable bool hostIsValid;    ///< True if the data on the CPU is currently valid
public:

    CudaMatrix()
        : sizeX ( 0 ), sizeY( 0 ), pitch(0), hostAllocSize ( 0 ), deviceAllocSize ( 0 ), devicePointer ( NULL ), hostPointer ( NULL ), deviceIsValid ( true ), hostIsValid ( true )
    {}

    CudaMatrix(size_t x, size_t y )
        : sizeX ( 0 ), sizeY ( 0 ), pitch(0), hostAllocSize ( 0 ), deviceAllocSize ( 0 ), devicePointer ( NULL ), hostPointer ( NULL ), deviceIsValid ( true ), hostIsValid ( true )
    {
        resize (x,y,0);
    }

    CudaMatrix(const CudaMatrix<T>& v )
        : sizeX ( 0 ), sizeY ( 0 ), pitch(0), hostAllocSize ( 0 ), deviceAllocSize ( 0 ), devicePointer ( NULL ), hostPointer ( NULL ), deviceIsValid ( true ), hostIsValid ( true )
    {
        *this = v;
    }

    void clear()
    {
        sizeY = 0;
        sizeY = 0;
        pitch = 0;
        deviceIsValid = true;
        hostIsValid = true;
    }

    ~CudaMatrix()
    {
        if (hostPointer!=NULL) mycudaFreeHost(hostPointer);
        if (devicePointer!=NULL) mycudaFree(devicePointer);
    }

    size_type getSizeX() const
    {
        return sizeX;
    }

    size_type getSizeY() const
    {
        return sizeY;
    }

    size_type getPitch() const
    {
        return pitch;
    }
    bool empty() const
    {
        return sizeX==0 || sizeY==0;
    }

    void fastResize(size_type x,size_type y,size_type WARP_SIZE)
    {
        size_type s = x*y;
        if (s > hostAllocSize)
        {
            hostAllocSize = ( s>2*hostAllocSize ) ? s : 2*hostAllocSize;
            // always allocate multiples of BSIZE values
            hostAllocSize = ( hostAllocSize+BSIZE-1 ) &-BSIZE;
            T* prevHostPointer = hostPointer;
            if ( prevHostPointer != NULL )
                mycudaFreeHost ( prevHostPointer );
            void* newHostPointer = NULL;
            mycudaMallocHost ( &newHostPointer, hostAllocSize*sizeof ( T ) );
            hostPointer = (T*)newHostPointer;
        }
        if (WARP_SIZE==0)
        {
            pitch = x*sizeof(T);
            if (y*pitch > deviceAllocSize)
            {
                void* prevDevicePointer = devicePointer;
                if (prevDevicePointer != NULL ) mycudaFree ( prevDevicePointer );

                mycudaMallocPitch(&devicePointer, &pitch, x, y);
                deviceAllocSize = y*pitch;
            }
        }
        else
        {
            pitch = ((x+WARP_SIZE-1)/WARP_SIZE)*WARP_SIZE*sizeof(T);
            if (y*pitch > deviceAllocSize)
            {
                void* prevDevicePointer = devicePointer;
                if (prevDevicePointer != NULL ) mycudaFree ( prevDevicePointer );

                mycudaMallocPitch(&devicePointer, &pitch, pitch, y);
                deviceAllocSize = y*pitch;
            }
        }
        sizeX = x;
        sizeY = y;
        deviceIsValid = true;
        hostIsValid = true;
    }

    void resize (size_type x,size_type y,size_t w)
    {
        fastResize(x,y,w);
    }

    void swap ( CudaMatrix<T>& v )
    {
#define VSWAP(type, var) { type t = var; var = v.var; v.var = t; }
        VSWAP ( size_type, sizeX );
        VSWAP ( size_type, sizeY );
        VSWAP ( size_type, pitch );
        VSWAP ( size_type, hostAllocSize );
        VSWAP ( size_type, deviceAllocSize );
        VSWAP ( void*    , devicePointer );
        VSWAP ( T*       , hostPointer );
        VSWAP ( bool     , deviceIsValid );
        VSWAP ( bool     , hostIsValid );
#undef VSWAP
    }

    const void* deviceRead ( int x=0, int y=0 ) const
    {
        copyToDevice();
        return (const T*)(( ( const char* ) devicePointer ) +(y*pitch+x*sizeof(T)));
    }

    void* deviceWrite ( int x=0, int y=0 )
    {
        copyToDevice();
        hostIsValid = false;
        return (T*)(( ( const char* ) devicePointer ) +(y*pitch+x*sizeof(T)));
    }

    const T* hostRead ( int x=0, int y=0 ) const
    {
        copyToHost();
        return hostPointer+(y*sizeX+x);
    }

    T* hostWrite ( int x=0, int y=0 )
    {
        copyToHost();
        deviceIsValid = false;
        return hostPointer+(y*sizeX+x);
    }

    bool isHostValid() const
    {
        return hostIsValid;
    }

    bool isDeviceValid() const
    {
        return deviceIsValid;
    }

    const T& operator() (size_type x,size_type y) const
    {
        checkIndex (x,y);
        return hostRead(x,y);
    }

    T& operator() (size_type x,size_type y)
    {
        checkIndex (x,y);
        return hostWrite(x,y);
    }

    const T* operator[] (size_type y) const
    {
        checkIndex (0,y);
        return hostRead(0,y);
    }

    T* operator[] (size_type y)
    {
        checkIndex (0,y);
        return hostWrite(0,y);
    }

    const T& getCached (size_type x,size_type y) const
    {
        checkIndex (x,y);
        return hostPointer[y*sizeX+x];
    }

protected:
    void copyToHost() const
    {
        if ( hostIsValid ) return;
#ifndef NDEBUG
        std::cout << "CUDA: GPU->CPU copy of "<<sofa::core::objectmodel::Base::decodeTypeName ( typeid ( *this ) ) <<": "<<vectorSize*sizeof ( T ) <<" B"<<std::endl;
#endif
        mycudaMemcpyDeviceToHost2D ( hostPointer, sizeX*sizeof(T), devicePointer, pitch, sizeX*sizeof(T), sizeY);
        hostIsValid = true;
    }
    void copyToDevice() const
    {
        if ( deviceIsValid ) return;
#ifndef NDEBUG
        std::cout << "CUDA: CPU->GPU copy of "<<sofa::core::objectmodel::Base::decodeTypeName ( typeid ( *this ) ) <<": "<<vectorSize*sizeof ( T ) <<" B"<<std::endl;
#endif
        mycudaMemcpyHostToDevice2D ( devicePointer, pitch, hostPointer, sizeX*sizeof(T),  sizeX*sizeof(T), sizeY);
        deviceIsValid = true;
    }

#ifdef NDEBUG
    void checkIndex ( size_type,size_type ) const
    {
    }
#else
    void checkIndex ( size_type x,size_type y) const
    {
        assert (x<this->sizeX());
        assert (y<this->sizeY());
    }
#endif

};

template<class TCoord, class TDeriv, class TReal = typename TCoord::value_type>
class CudaVectorTypes
{
public:
    typedef TCoord Coord;
    typedef TDeriv Deriv;
    typedef TReal Real;
    typedef CudaVector<Coord> VecCoord;
    typedef CudaVector<Deriv> VecDeriv;
    typedef CudaVector<Real> VecReal;

    template <class T>
    class SparseData
    {
    public:
        SparseData ( unsigned int _index, const T& _data ) : index ( _index ), data ( _data ) {};
        unsigned int index;
        T data;
    };

    typedef SparseData<Coord> SparseCoord;
    typedef SparseData<Deriv> SparseDeriv;

    typedef CudaVector<SparseCoord> SparseVecCoord;
    typedef CudaVector<SparseDeriv> SparseVecDeriv;

    //! All the Constraints applied to a state Vector
    typedef    sofa::helper::vector<SparseVecDeriv> VecConst;

    static void set ( Coord& c, double x, double y, double z )
    {
        if ( c.size() >0 )
            c[0] = ( typename Coord::value_type ) x;
        if ( c.size() >1 )
            c[1] = ( typename Coord::value_type ) y;
        if ( c.size() >2 )
            c[2] = ( typename Coord::value_type ) z;
    }

    static void get ( double& x, double& y, double& z, const Coord& c )
    {
        x = ( c.size() >0 ) ? ( double ) c[0] : 0.0;
        y = ( c.size() >1 ) ? ( double ) c[1] : 0.0;
        z = ( c.size() >2 ) ? ( double ) c[2] : 0.0;
    }

    static void add ( Coord& c, double x, double y, double z )
    {
        if ( c.size() >0 )
            c[0] += ( typename Coord::value_type ) x;
        if ( c.size() >1 )
            c[1] += ( typename Coord::value_type ) y;
        if ( c.size() >2 )
            c[2] += ( typename Coord::value_type ) z;
    }

    static const char* Name();
};

typedef sofa::defaulttype::Vec3f Vec3f;
typedef sofa::defaulttype::Vec2f Vec2f;

using defaulttype::Vec;
using defaulttype::NoInit;
using defaulttype::NOINIT;

class Vec3f1 : public Vec3f
{
public:
    typedef float real;
    enum { N=3 };
    Vec3f1() : dummy(0.0f) {}
    template<class real2>
    Vec3f1(const Vec<N,real2>& v): Vec3f(v), dummy(0.0f) {}
    Vec3f1(float x, float y, float z) : Vec3f(x,y,z), dummy(0.0f) {}

    /// Fast constructor: no initialization
    explicit Vec3f1(NoInit n) : Vec3f(n), dummy(0.0f)
    {
    }

    // LINEAR ALGEBRA

    /// Multiplication by a scalar f.
    template<class real2>
    Vec3f1 operator*(real2 f) const
    {
        Vec3f1 r(NOINIT);
        for (int i=0; i<N; i++)
            r[i] = this->elems[i]*(real)f;
        return r;
    }

    /// On-place multiplication by a scalar f.
    template<class real2>
    void operator*=(real2 f)
    {
        for (int i=0; i<N; i++)
            this->elems[i]*=(real)f;
    }

    template<class real2>
    Vec3f1 operator/(real2 f) const
    {
        Vec3f1 r(NOINIT);
        for (int i=0; i<N; i++)
            r[i] = this->elems[i]/(real)f;
        return r;
    }

    /// On-place division by a scalar f.
    template<class real2>
    void operator/=(real2 f)
    {
        for (int i=0; i<N; i++)
            this->elems[i]/=(real)f;
    }

    /// Dot product.
    template<class real2>
    real operator*(const Vec<N,real2>& v) const
    {
        real r = (real)(this->elems[0]*v[0]);
        for (int i=1; i<N; i++)
            r += (real)(this->elems[i]*v[i]);
        return r;
    }

    /// Dot product.
    real operator*(const Vec3f1& v) const
    {
        real r = (real)(this->elems[0]*v[0]);
        for (int i=1; i<N; i++)
            r += (real)(this->elems[i]*v[i]);
        return r;
    }

    /// linear product.
    template<class real2>
    Vec3f1 linearProduct(const Vec<N,real2>& v) const
    {
        Vec3f1 r(NOINIT);
        for (int i=0; i<N; i++)
            r[i]=this->elems[i]*(real)v[i];
        return r;
    }

    /// linear product.
    Vec3f1 linearProduct(const Vec3f1& v) const
    {
        Vec3f1 r(NOINIT);
        for (int i=0; i<N; i++)
            r[i]=this->elems[i]*(real)v[i];
        return r;
    }

    /// Vector addition.
    template<class real2>
    Vec3f1 operator+(const Vec<N,real2>& v) const
    {
        Vec3f1 r(NOINIT);
        for (int i=0; i<N; i++)
            r[i]=this->elems[i]+(real)v[i];
        return r;
    }

    /// Vector addition.
    Vec3f1 operator+(const Vec3f1& v) const
    {
        Vec3f1 r(NOINIT);
        for (int i=0; i<N; i++)
            r[i]=this->elems[i]+(real)v[i];
        return r;
    }

    /// On-place vector addition.
    template<class real2>
    void operator+=(const Vec<N,real2>& v)
    {
        for (int i=0; i<N; i++)
            this->elems[i]+=(real)v[i];
    }

    /// On-place vector addition.
    void operator+=(const Vec3f1& v)
    {
        for (int i=0; i<N; i++)
            this->elems[i]+=(real)v[i];
    }

    /// Vector subtraction.
    template<class real2>
    Vec3f1 operator-(const Vec<N,real2>& v) const
    {
        Vec3f1 r(NOINIT);
        for (int i=0; i<N; i++)
            r[i]=this->elems[i]-(real)v[i];
        return r;
    }

    /// Vector subtraction.
    Vec3f1 operator-(const Vec3f1& v) const
    {
        Vec3f1 r(NOINIT);
        for (int i=0; i<N; i++)
            r[i]=this->elems[i]-(real)v[i];
        return r;
    }

    /// On-place vector subtraction.
    template<class real2>
    void operator-=(const Vec<N,real2>& v)
    {
        for (int i=0; i<N; i++)
            this->elems[i]-=(real)v[i];
    }

    /// On-place vector subtraction.
    void operator-=(const Vec3f1& v)
    {
        for (int i=0; i<N; i++)
            this->elems[i]-=(real)v[i];
    }

    /// Vector negation.
    Vec3f1 operator-() const
    {
        Vec3f1 r(NOINIT);
        for (int i=0; i<N; i++)
            r[i]=-this->elems[i];
        return r;
    }

    Vec3f1 cross( const Vec3f1& b ) const
    {
        BOOST_STATIC_ASSERT(N == 3);
        return Vec3f1(
                (*this)[1]*b[2] - (*this)[2]*b[1],
                (*this)[2]*b[0] - (*this)[0]*b[2],
                (*this)[0]*b[1] - (*this)[1]*b[0]
                );
    }

protected:
    float dummy;
};

// GPUs do not support double precision yet
// ( NVIDIA announced at SuperComputing'06 that it will be supported in 2007... )
//typedef sofa::defaulttype::Vec3d Vec3d;
//typedef sofa::defaulttype::Vec2d Vec2d;

//typedef CudaVectorTypes<Vec3d,Vec3d,double> CudaVec3dTypes;
typedef CudaVectorTypes<Vec3f,Vec3f,float> CudaVec3fTypes;
typedef CudaVec3fTypes CudaVec3Types;

template<>
inline const char* CudaVec3fTypes::Name()
{
    return "CudaVec3f";
}

//typedef CudaVectorTypes<Vec2d,Vec2d,double> CudaVec2dTypes;
typedef CudaVectorTypes<Vec2f,Vec2f,float> CudaVec2fTypes;
typedef CudaVec2fTypes CudaVec2Types;

template<>
inline const char* CudaVec2fTypes::Name()
{
    return "CudaVec2f";
}

typedef CudaVectorTypes<Vec3f1,Vec3f1,float> CudaVec3f1Types;

template<>
inline const char* CudaVec3f1Types::Name()
{
    return "CudaVec3f1";
}

template<int N, typename real>
class CudaRigidTypes;

template<typename real>
class CudaRigidTypes<3, real>
{
public:
    typedef real Real;
    typedef sofa::defaulttype::RigidCoord<3,real> Coord;
    typedef sofa::defaulttype::RigidDeriv<3,real> Deriv;
    typedef typename Coord::Vec3 Vec3;
    typedef typename Coord::Quat Quat;
    typedef CudaVector<Coord> VecCoord;
    typedef CudaVector<Deriv> VecDeriv;
    typedef CudaVector<Real> VecReal;

    template <class T>
    class SparseData
    {
    public:
        SparseData ( unsigned int _index, const T& _data ) : index ( _index ), data ( _data ) {};
        unsigned int index;
        T data;
    };

    typedef SparseData<Coord> SparseCoord;
    typedef SparseData<Deriv> SparseDeriv;

    typedef CudaVector<SparseCoord> SparseVecCoord;
    typedef CudaVector<SparseDeriv> SparseVecDeriv;

    //! All the Constraints applied to a state Vector
    typedef    sofa::helper::vector<SparseVecDeriv> VecConst;

    static void set ( Coord& r, double x, double y, double z )
    {
        Vec3& c = r.getCenter();
        if ( c.size() >0 )
            c[0] = ( typename Coord::value_type ) x;
        if ( c.size() >1 )
            c[1] = ( typename Coord::value_type ) y;
        if ( c.size() >2 )
            c[2] = ( typename Coord::value_type ) z;
    }

    static void get ( double& x, double& y, double& z, const Coord& r )
    {
        const Vec3& c = r.getCenter();
        x = ( c.size() >0 ) ? ( double ) c[0] : 0.0;
        y = ( c.size() >1 ) ? ( double ) c[1] : 0.0;
        z = ( c.size() >2 ) ? ( double ) c[2] : 0.0;
    }

    static void add ( Coord& r, double x, double y, double z )
    {
        Vec3& c = r.getCenter();
        if ( c.size() >0 )
            c[0] += ( typename Coord::value_type ) x;
        if ( c.size() >1 )
            c[1] += ( typename Coord::value_type ) y;
        if ( c.size() >2 )
            c[2] += ( typename Coord::value_type ) z;
    }

    static void set ( Deriv& r, double x, double y, double z )
    {
        Vec3& c = r.getVCenter();
        if ( c.size() >0 )
            c[0] = ( typename Coord::value_type ) x;
        if ( c.size() >1 )
            c[1] = ( typename Coord::value_type ) y;
        if ( c.size() >2 )
            c[2] = ( typename Coord::value_type ) z;
    }

    static void get ( double& x, double& y, double& z, const Deriv& r )
    {
        const Vec3& c = r.getVCenter();
        x = ( c.size() >0 ) ? ( double ) c[0] : 0.0;
        y = ( c.size() >1 ) ? ( double ) c[1] : 0.0;
        z = ( c.size() >2 ) ? ( double ) c[2] : 0.0;
    }

    static void add ( Deriv& r, double x, double y, double z )
    {
        Vec3& c = r.getVCenter();
        if ( c.size() >0 )
            c[0] += ( typename Coord::value_type ) x;
        if ( c.size() >1 )
            c[1] += ( typename Coord::value_type ) y;
        if ( c.size() >2 )
            c[2] += ( typename Coord::value_type ) z;
    }

    static const char* Name();
};

//typedef CudaVectorTypes<Vec3d,Vec3d,double> CudaVec3dTypes;
typedef CudaRigidTypes<3,float> CudaRigid3fTypes;
typedef CudaRigid3fTypes CudaRigid3Types;

template<>
inline const char* CudaRigid3fTypes::Name()
{
    return "CudaRigid3f";
}

} // namespace cuda

} // namespace gpu

} // namespace sofa

#endif
