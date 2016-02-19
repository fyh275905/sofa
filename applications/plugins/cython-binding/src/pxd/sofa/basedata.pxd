# -*- coding: ASCII -*-
from libcpp cimport bool
from libcpp.string cimport string as libcpp_string 

cdef extern from "../../../../../framework/sofa/defaulttype/DataTypeInfo.h" namespace "sofa::defaulttype": 
    cdef cppclass _AbstractTypeInfo "sofa::defaulttype::AbstractTypeInfo":
        _AbstractTypeInfo() except + 
        size_t size()
        size_t size(const void* data) 
        bool Integer()
        bool Scalar()
        bool Text()
        bool Container()
        bool FixedSize()
          
        double getScalarValue (void* data, size_t index) 
        long long getIntegerValue (void* data, size_t index) 
        libcpp_string getTextValue (void* data, size_t index) 
        void setIntegerValue(void* data, size_t index, long long value) 
        void setScalarValue (void* data, size_t index, double value) 
        void setTextValue(void* data, size_t index, libcpp_string& value) 
        bool setSize(void* data, size_t size)


cdef extern from "../../../../../framework/sofa/core/objectmodel/BaseData.h" namespace "sofa::core::objectmodel":       
    cdef cppclass _BaseData "sofa::core::objectmodel::BaseData":
        _BaseData() except + 
        libcpp_string getName() 
        libcpp_string getValueString() 
        libcpp_string getValueTypeString()
        _AbstractTypeInfo* getValueTypeInfo() 
        void* getValueVoidPtr()
        void* beginEditVoidPtr()
        void endEditVoidPtr()
        char* getHelp() 
        
        void setPersistent(bool) 
        bool isPersistent()
        
cdef class BaseData:
        cdef _BaseData* realptr

