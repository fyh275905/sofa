#ifndef COMPLIANT_MISC_PYTHON_H
#define COMPLIANT_MISC_PYTHON_H

#include <sofa/defaulttype/DataTypeInfo.h>


namespace python {

template<class T>
struct opaque;


template<class T>
struct opaque<T*> {

    explicit operator bool() const { return data; }
    
    opaque() : data(0) { }
    
    T* data;
    
    friend std::istream& operator>>(std::istream& in , const opaque& ) {
        // TODO emit warning
        return in;
    }

    friend std::ostream& operator<<(std::ostream& out, const opaque& ) {
        return out << "#<opaque>";
    }

};

template<class Real>
struct vec {
    Real* data;

    std::size_t outer;
    std::size_t inner;
    
    template<class T>
    static vec map(const std::vector<T>& value) {
        return {const_cast<Real*>(&value[0][0]),
                value.size(),
                T::total_size};
    }

    static vec map(const std::vector<Real>& value) {
        return {const_cast<Real*>(value.data()),
                value.size(),
                1};
    }
    
};
}


namespace sofa {
namespace defaulttype {

template<class T>
struct DataTypeInfo< python::opaque<T*> > {
    // can't believe there is no default-impl i can override

    typedef python::opaque<T*> DataType;
    typedef DataType BaseType;
    typedef DataType ValueType;
    
    typedef DataTypeInfo<BaseType> BaseTypeInfo;
    typedef DataTypeInfo<ValueType> ValueTypeInfo;

    static const bool CopyOnWrite = false;
    static const bool ValidInfo = false;
    static const bool FixedSize = true;
    static const bool ZeroConstructor = true;
    static const bool SimpleCopy = true;
    static const bool SimpleLayout = true;
    static const bool Integer = false;
    static const bool Scalar = false;
    static const bool Text = false;
    static const bool Container = false;        

    static std::size_t size() { return 1; }
    static std::size_t size(const DataType& /*data*/) { return 1; }
    
    static std::size_t byteSize() { return sizeof(python::opaque<T*>); }    
    static bool setSize(DataType& /*data*/, std::size_t /*size*/) { return false; }

    template <typename U>
    static void getValue(const DataType& /*data*/, std::size_t /*index*/, U& /*value*/)
    {
    }

    template<typename U>
    static void setValue(DataType& /*data*/, std::size_t /*index*/, const U& /*value*/)
    {
    }

    static void getValueString(const DataType& /*data*/, std::size_t /*index*/, std::string& /*value*/)
    {
    }

    static void setValueString(DataType& /*data*/, std::size_t /*index*/, const std::string& /*value*/)
    {
    }

    
    static const void* getValuePtr(const DataType& type)
    {
        return (void*) &type;
    }

    static void* getValuePtr(DataType& type)
    {
        return (void*) &type;
    }

    static const char* name() { return "opaque"; }
};

}
}







#endif
