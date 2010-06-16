#include "myopencl.h"

#include <iostream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sofa/helper/BackTrace.h>
#include <sofa/helper/vector.h>
#include <map>

#define DEBUG_TEXT(t) //printf("\t  %s\n",t);
#define CL_KERNEL_PATH "/modules/sofa/gpu/opencl/kernels/"

#if defined(__cplusplus)
namespace sofa
{
namespace gpu
{
namespace opencl
{
#endif



//private data
int _numDevices = 0;
int myopenclMultiOpMax = 0;
cl_context _context = NULL;
cl_command_queue* _queues = NULL;
cl_device_id* _devices = NULL;
cl_int _error=CL_SUCCESS;

std::string _mainPath;

//private functions

void createPath()
{
    _mainPath = getcwd(NULL,0);
    _mainPath += CL_KERNEL_PATH;
}


void releaseContext()
{
    if(_context)
        clReleaseContext(_context);
    myopenclShowError(__FILE__, __LINE__);
}


std::string myopenclGetDeviceString(cl_device_id device, cl_device_info param_name)
{
    size_t size;
    clGetDeviceInfo(device,param_name,0,NULL,&size);
    char * data = new char[size];
    clGetDeviceInfo(device,param_name,size,data,NULL);
    std::string str(data);
    delete[] data;
    return str;
}

template<class T>
void myopenclGetDeviceInfo(cl_device_id device, cl_device_info param_name, T* result)
{
    clGetDeviceInfo(device,param_name,sizeof(T), result,NULL);
}

std::string myopenclGetPlatformString(cl_platform_id platform, cl_platform_info param_name)
{
    size_t size;
    clGetPlatformInfo(platform,param_name,0,NULL,&size);
    char * data = new char[size];
    clGetPlatformInfo(platform,param_name,size,data,NULL);
    std::string str(data);
    delete[] data;
    return str;
}

template<class T>
void myopenclGetPlatformInfo(cl_platform_id platform, cl_platform_info param_name, T* result)
{
    clGetPlatformInfo(platform,param_name,sizeof(T),result,NULL);
}

class OpenCLDeviceInfo
{
public:
    cl_device_id id;
    cl_device_type type;
    cl_platform_id platformId;
    int platformIndex;
    std::string name;
    std::string version;
    cl_ulong globalMemSize;
    cl_ulong globalMemCacheSize;
    cl_ulong localMemSize;
    cl_uint  maxFreq;
    cl_uint  maxUnits;
    OpenCLDeviceInfo()
        : id(0), type(0), platformId(0), platformIndex(0),
          globalMemSize(0), globalMemCacheSize(0), localMemSize(0), maxFreq(0), maxUnits(0)
    {
    }
};

helper::vector<OpenCLDeviceInfo> devices;
std::map<cl_device_id, int> devIdMap;

const char* myopenclDeviceTypeName(cl_device_type type)
{
    switch(type)
    {
    case CL_DEVICE_TYPE_CPU :  return "CPU";
    case CL_DEVICE_TYPE_GPU :  return "GPU";
    case CL_DEVICE_TYPE_ACCELERATOR :  return "ACCELERATOR";
    case CL_DEVICE_TYPE_ALL :  return "ALL";
    case CL_DEVICE_TYPE_DEFAULT :  return "DEFAULT";
    default: return "OTHER";
    }
}

void listDevices(cl_device_type type, cl_platform_id platformId, int platformIndex)
{
    //number of devices on the platform
    cl_uint nb_devices;
    clGetDeviceIDs(platformId,type,0,NULL,&nb_devices);
    if (nb_devices == 0) return;
    cl_device_id *deviceIds = new cl_device_id[nb_devices];
    clGetDeviceIDs(platformId,type,nb_devices,deviceIds,NULL);
    std::cout << "------------\n " << myopenclDeviceTypeName(type) << std::endl;

    int index0 = devices.size();
    devices.resize(index0+nb_devices);
    //for each device, display info
    for(cl_uint i=0; i<nb_devices; i++)
    {
        OpenCLDeviceInfo& dev = devices[index0+i];
        dev.id = deviceIds[i];
        devIdMap[dev.id] = index0+i;
        dev.type = type;
        dev.platformId = platformId;
        dev.platformIndex = platformIndex;
        dev.name = myopenclGetDeviceString(dev.id, CL_DEVICE_NAME);
        dev.version = myopenclGetDeviceString(dev.id, CL_DEVICE_VERSION);
        myopenclGetDeviceInfo(dev.id,CL_DEVICE_GLOBAL_MEM_SIZE,&dev.globalMemSize);
        myopenclGetDeviceInfo(dev.id,CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,&dev.globalMemCacheSize);
        myopenclGetDeviceInfo(dev.id,CL_DEVICE_LOCAL_MEM_SIZE,&dev.localMemSize);
        myopenclGetDeviceInfo(dev.id,CL_DEVICE_MAX_CLOCK_FREQUENCY,&dev.maxFreq);
        myopenclGetDeviceInfo(dev.id,CL_DEVICE_MAX_COMPUTE_UNITS,&dev.maxUnits);

        std::cout << "----------\n";
        std::cout << "  DEVICE NAME:    " << dev.name << std::endl;
        std::cout << "  DEVICE VERSION: " << dev.version << std::endl;
        std::cout << "  GLOBAL MEM SIZE: " << dev.globalMemSize << std::endl;
        std::cout << "  GLOBAL MEM CACHE SIZE: " << dev.globalMemCacheSize << std::endl;
        std::cout << "  LOCAL MEM SIZE: " << dev.localMemSize << std::endl;
        std::cout << "  MAX CLOCK FREQUENCY: " << dev.maxFreq << std::endl;
        std::cout << "  MAX COMPUTE UNITS: " << dev.maxUnits << std::endl;
    }
    delete[] deviceIds;
}

class OpenCLPlatformInfo
{
public:
    cl_platform_id id;
    std::string name;
    std::string version;
    std::string profile;
    std::string vendor;
    std::string extensions;
    int indexDevice0;
    int nbDevices;
    OpenCLPlatformInfo()
        : id(0),
          indexDevice0(0), nbDevices(0)
    {
    }
};

helper::vector<OpenCLPlatformInfo> platforms;
std::map<cl_platform_id,int> platIdMap;

void listPlatform()
{
    //number of platforms in the computer
    cl_uint nb_platforms;
    clGetPlatformIDs(0,NULL,&nb_platforms);
    cl_platform_id * platformIds = new cl_platform_id();
    clGetPlatformIDs(nb_platforms,platformIds,NULL);

    std::cout << std::endl << std::endl << "=======================================" << std::endl << "OPENCL PLATFORMS LIST"<< std::endl;

    platforms.resize(nb_platforms);
    //for each platform, display info and search devices
    for(cl_uint i=0; i<nb_platforms; i++)
    {
        OpenCLPlatformInfo& plat = platforms[i];
        plat.id = platformIds[i];
        platIdMap[plat.id] = i;
        plat.name = myopenclGetPlatformString(plat.id,CL_PLATFORM_NAME);
        plat.version = myopenclGetPlatformString(plat.id,CL_PLATFORM_VERSION);
        plat.profile = myopenclGetPlatformString(plat.id,CL_PLATFORM_PROFILE);
        plat.vendor = myopenclGetPlatformString(plat.id,CL_PLATFORM_VENDOR);
        plat.extensions = myopenclGetPlatformString(plat.id,CL_PLATFORM_EXTENSIONS);

        //display info
        std::cout << "=======================================" << std::endl;


        std::cout << "PLATFORM NAME:    " << plat.name << std::endl;
        std::cout << "PLATFORM VERSION: " << plat.version << std::endl;
        std::cout << "PLATFORM PROFILE: " << plat.profile << std::endl;
        std::cout << "PLATFORM VENDOR:  " << plat.vendor << std::endl;
        std::cout << "PLATFORM EXTENSI: " << plat.extensions << std::endl;
        plat.indexDevice0 = devices.size();
        listDevices(CL_DEVICE_TYPE_CPU,plat.id, i);
        listDevices(CL_DEVICE_TYPE_GPU,plat.id, i);
        listDevices(CL_DEVICE_TYPE_ACCELERATOR,plat.id, i);
        plat.nbDevices = devices.size() - plat.indexDevice0;
    }
    std::cout << std::endl << "=======================================" << std::endl;

    delete[] platformIds;
}

void releaseQueues()
{
    if (_queues)
    {
        for(int i=0; i<_numDevices; i++)
            if(_queues[i])
            {
                clReleaseCommandQueue(_queues[i]);
                myopenclShowError(__FILE__, __LINE__);
            }
    }
    delete[] _queues;
}

void releaseDevices()
{
    if(_devices) delete[] _devices;
}

int selectedDeviceIndex = -1;
helper::vector<int> activeDevices;

cl_context createContext(cl_device_type default_type)
{
    if(_context)
    {
        clReleaseContext(_context);
        myopenclShowError(__FILE__, __LINE__);
    }
    if (selectedDeviceIndex >= (int)devices.size())
    {
        std::cerr << "OPENCL ERROR: selected device " << selectedDeviceIndex << " NOT FOUND." << std::endl;
        selectedDeviceIndex = -1;
    }
    if (selectedDeviceIndex < 0)
    {
        int platformIndex = 0;
        // find the first device having the requested type
        for (unsigned int i=0; i<devices.size(); ++i)
        {
            if (devices[i].type == default_type)
            {
                platformIndex = devices[i].platformIndex;
                break;
            }
        }
        if (platformIndex < (int) platforms.size())
        {
            cl_context_properties context_props[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[platformIndex].id, 0 };
            _context = clCreateContextFromType(context_props, default_type, NULL, NULL, &_error);
        }
        else
        {
            _context = clCreateContextFromType(0, default_type, NULL, NULL, &_error);
        }
    }
    else
    {
        cl_context_properties context_props[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)devices[selectedDeviceIndex].platformId, 0 };
        cl_device_id dev_ids[1] = { devices[selectedDeviceIndex].id };
        _context = clCreateContext(context_props, 1, dev_ids, NULL, NULL, &_error);
    }
    myopenclShowError(__FILE__, __LINE__);
    return _context;
}

void createDevices()
{
    if(_devices) delete[] _devices;
    if (selectedDeviceIndex < 0)
    {
        size_t devices_size;
        clGetContextInfo(_context, CL_CONTEXT_DEVICES,0, NULL, &devices_size);		//compter le nombre de matériel
        _numDevices = devices_size/sizeof(cl_device_id);
        _devices = new cl_device_id[_numDevices];					//allouer un espace mémoire pour recueillir les matériels
        clGetContextInfo(_context, CL_CONTEXT_DEVICES,devices_size,_devices, NULL);	//créer une liste de matériel
        myopenclShowError(__FILE__, __LINE__);
    }
    else
    {
        _numDevices = 1;
        _devices = new cl_device_id[1];
        _devices[0] = devices[selectedDeviceIndex].id;
    }
    std::cout << "OpenCL: " << _numDevices  << " active devices :" << std::endl;
    activeDevices.resize(_numDevices);
    for (int i = 0; i < _numDevices; ++i)
    {
        cl_device_id id = _devices[i];
        std::map<cl_device_id,int>::const_iterator it = devIdMap.find(id);
        int index = -1;
        if (it != devIdMap.end())
            index = it->second;
        activeDevices[i] = index;
        std::cout << "  ";
        if (index >= 0)
        {
            OpenCLDeviceInfo& dev = devices[index];
            std::cout << myopenclDeviceTypeName(dev.type) << " " << platforms[dev.platformIndex].vendor << " " << dev.name;
            std::cout << ", " << dev.maxUnits << " cores @ " << dev.maxFreq / 1000.0 << " GHz";
            std::cout << ", " << ((dev.maxFreq+512*1024)/(1024*1024)) / 1024.0 << " GiB";
        }
        else
            std::cout << "UNKNOWN";
        std::cout << " (ID " << index << ")" << std::endl;
    }
    std::cout << std::endl;
}

void createQueues()
{
    if(_queues)
        releaseQueues();
    _queues = new cl_command_queue[_numDevices];
    for(int i=0; i<_numDevices; i++)
    {
        _queues[i] = clCreateCommandQueue(_context, _devices[i], 0, NULL);
        myopenclShowError(__FILE__, __LINE__);
    }
}

//opencl public functions

int myopenclInit(int device)
{
    DEBUG_TEXT("myopenclInit");
    createPath();

    listPlatform();
    if (device==-1)
    {
        const char* var = getenv("OPENCL_DEVICE");
        device = (var && *var) ? atoi(var):0;
    }
    if (device >= (int)devices.size())
    {
        std::cerr << "OPENCL ERROR: Device " << device << " not found." << std::endl;
        return 0;
    }

    createContext(CL_DEVICE_TYPE_GPU);
    createDevices();
    createQueues();

    if(_error==CL_SUCCESS)
        return 1;
    else
        return 0;
}

int myopenclClose()
{
    releaseQueues();
    releaseDevices();
    releaseQueues();
    if(_error==CL_SUCCESS)
        return 1;
    else
        return 0;
}

int myopenclGetnumDevices()
{
    return _numDevices;
}

void myopenclCreateBuffer(int /*device*/,cl_mem* dptr,int n)
{
    DEBUG_TEXT("myopenclCreateBuffer ");
    *dptr = clCreateBuffer(_context,CL_MEM_READ_WRITE,n,NULL,&_error);
    myopenclShowError(__FILE__, __LINE__);
    DEBUG_TEXT("~myopenclCreateBuffer ");
}

void myopenclReleaseBuffer(int /*device*/,cl_mem p)
{
    DEBUG_TEXT("myopenclReleaseBuffer ");
    _error = clReleaseMemObject((cl_mem) p);
    myopenclShowError(__FILE__, __LINE__);
    DEBUG_TEXT("~myopenclReleaseBuffer ");
}

void myopenclEnqueueWriteBuffer(int device,cl_mem ddest,size_t offset,const void* hsrc,size_t n)
{
    DEBUG_TEXT("myopenclEnqueueWriteBuffer");
//std::cout << "clEnqueueWriteBuffer(" << device << ", " << ddest << ", " << offset << ", " << hsrc << ", " << n << ")" << std::endl;
    _error = clEnqueueWriteBuffer(_queues[device], ddest, CL_TRUE, offset, n, hsrc,0,NULL,NULL);
    myopenclShowError(__FILE__, __LINE__);
    DEBUG_TEXT("~myopenclEnqueueWriteBuffer");
}


void myopenclEnqueueReadBuffer(int device,void* hdest,const cl_mem dsrc,size_t offset, size_t n)
{
    DEBUG_TEXT("myopenclEnqueueReadBuffer");
    _error = clEnqueueReadBuffer(_queues[device],  dsrc, CL_TRUE, offset, n,hdest,0,NULL,NULL);
    myopenclShowError(__FILE__, __LINE__);
    DEBUG_TEXT("~myopenclEnqueueReadBuffer");
}

void myopenclEnqueueCopyBuffer(int device, cl_mem ddest,size_t destOffset,const cl_mem dsrc,size_t srcOffset, size_t n)
{
    DEBUG_TEXT("myopenclEnqueueCopyBuffer");
    _error = clEnqueueCopyBuffer(_queues[device],dsrc,ddest,srcOffset,destOffset, n,0,NULL,NULL);
    myopenclShowError(__FILE__, __LINE__);
    DEBUG_TEXT("~myopenclEnqueueCopyBuffer");
}

cl_program myopenclProgramWithSource(const char * s,const size_t size)
{
    DEBUG_TEXT("myopenclProgramWithSource");
    return clCreateProgramWithSource(_context, 1, &s, &size, &_error);
    myopenclShowError(__FILE__, __LINE__);
    DEBUG_TEXT("~myopenclProgramWithSource");
}

cl_kernel myopenclCreateKernel(void* p,const char * kernel_name)
{
    DEBUG_TEXT("myopenclCreateKernel");
    std::cout << "clCreateKernel(p, " << kernel_name << ");" << std::endl;
    return clCreateKernel((cl_program)p, kernel_name, &_error);
    myopenclShowError(__FILE__, __LINE__);
    DEBUG_TEXT("~myopenclCreateKernel");
}

template<>
void myopenclSetKernelArg<_device_pointer>(cl_kernel kernel, int num_arg, const _device_pointer* arg)
{
    if (arg->offset) std::cerr << "OpenCL ERROR: non-zero offset " << arg->offset << std::endl;
    myopenclSetKernelArg(kernel, num_arg, sizeof(cl_mem), (void*)&arg->m);
}

void myopenclSetKernelArg(cl_kernel kernel,int num_arg,int size,void* arg)
{
    DEBUG_TEXT("myopenclSetKernelArg");
//std::cout << "clSetKernelArg(kernel, " << num_arg << ", " << size << ", " << arg << ");" << std::endl;
    _error = clSetKernelArg(kernel, num_arg,size, arg);
    myopenclShowError(__FILE__, __LINE__);
    DEBUG_TEXT("~myopenclSetKernelArg");
}


void myopenclBuildProgram(void * program)
{
    DEBUG_TEXT("myopenclBuildProgram");
    _error = clBuildProgram((cl_program)program,0,NULL,NULL,NULL,NULL);

    myopenclShowError(__FILE__, __LINE__);
    DEBUG_TEXT("~myopenclBuildProgram");
}

void myopenclBuildProgramWithFlags(void * program, char * flags)
{
    DEBUG_TEXT("myopenclBuildProgram");
    _error = clBuildProgram((cl_program)program,0,NULL,flags,NULL,NULL);

    myopenclShowError(__FILE__, __LINE__);
    DEBUG_TEXT("~myopenclBuildProgram");
}

void myopenclExecKernel(int device,cl_kernel kernel,unsigned int work_dim,const size_t *global_work_offset,const size_t *global_work_size,const size_t *local_work_size)
{
    DEBUG_TEXT("myopenclExecKernel");

    _error = clEnqueueNDRangeKernel(_queues[device],kernel,work_dim,global_work_offset,global_work_size,local_work_size,0,NULL,NULL);

    DEBUG_TEXT("~myopenclExecKernel");
}

// information public functions

int myopenclNumDevices()
{
    DEBUG_TEXT("myopenclNumDevices");
    return _numDevices;
}

extern void* myopencldevice(int device)
{
    DEBUG_TEXT("myopencldevice");
    return (void*) _devices[device];
}

//error public functions

cl_int & myopenclError()
{
    return _error;
}

std::string myopenclErrorMsg(cl_int err)
{
    switch(err)
    {
#define SOFA_CL_ERR(e) case e: return #e
        SOFA_CL_ERR(CL_SUCCESS);
        SOFA_CL_ERR(CL_DEVICE_NOT_FOUND);
        SOFA_CL_ERR(CL_DEVICE_NOT_AVAILABLE);
        SOFA_CL_ERR(CL_COMPILER_NOT_AVAILABLE);
        SOFA_CL_ERR(CL_MEM_OBJECT_ALLOCATION_FAILURE);
        SOFA_CL_ERR(CL_OUT_OF_RESOURCES);
        SOFA_CL_ERR(CL_OUT_OF_HOST_MEMORY);
        SOFA_CL_ERR(CL_PROFILING_INFO_NOT_AVAILABLE);
        SOFA_CL_ERR(CL_MEM_COPY_OVERLAP);
        SOFA_CL_ERR(CL_IMAGE_FORMAT_MISMATCH);
        SOFA_CL_ERR(CL_IMAGE_FORMAT_NOT_SUPPORTED);
        SOFA_CL_ERR(CL_BUILD_PROGRAM_FAILURE);
        SOFA_CL_ERR(CL_MAP_FAILURE);
        SOFA_CL_ERR(CL_INVALID_VALUE);
        SOFA_CL_ERR(CL_INVALID_DEVICE_TYPE);
        SOFA_CL_ERR(CL_INVALID_PLATFORM);
        SOFA_CL_ERR(CL_INVALID_DEVICE);
        SOFA_CL_ERR(CL_INVALID_CONTEXT);
        SOFA_CL_ERR(CL_INVALID_QUEUE_PROPERTIES);
        SOFA_CL_ERR(CL_INVALID_COMMAND_QUEUE);
        SOFA_CL_ERR(CL_INVALID_HOST_PTR);
        SOFA_CL_ERR(CL_INVALID_MEM_OBJECT);
        SOFA_CL_ERR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
        SOFA_CL_ERR(CL_INVALID_IMAGE_SIZE);
        SOFA_CL_ERR(CL_INVALID_SAMPLER);
        SOFA_CL_ERR(CL_INVALID_BINARY);
        SOFA_CL_ERR(CL_INVALID_BUILD_OPTIONS);
        SOFA_CL_ERR(CL_INVALID_PROGRAM);
        SOFA_CL_ERR(CL_INVALID_PROGRAM_EXECUTABLE);
        SOFA_CL_ERR(CL_INVALID_KERNEL_NAME);
        SOFA_CL_ERR(CL_INVALID_KERNEL_DEFINITION);
        SOFA_CL_ERR(CL_INVALID_KERNEL);
        SOFA_CL_ERR(CL_INVALID_ARG_INDEX);
        SOFA_CL_ERR(CL_INVALID_ARG_VALUE);
        SOFA_CL_ERR(CL_INVALID_ARG_SIZE);
        SOFA_CL_ERR(CL_INVALID_KERNEL_ARGS);
        SOFA_CL_ERR(CL_INVALID_WORK_DIMENSION);
        SOFA_CL_ERR(CL_INVALID_WORK_GROUP_SIZE);
        SOFA_CL_ERR(CL_INVALID_WORK_ITEM_SIZE);
        SOFA_CL_ERR(CL_INVALID_GLOBAL_OFFSET);
        SOFA_CL_ERR(CL_INVALID_EVENT_WAIT_LIST);
        SOFA_CL_ERR(CL_INVALID_EVENT);
        SOFA_CL_ERR(CL_INVALID_OPERATION);
        SOFA_CL_ERR(CL_INVALID_GL_OBJECT);
        SOFA_CL_ERR(CL_INVALID_BUFFER_SIZE);
        SOFA_CL_ERR(CL_INVALID_MIP_LEVEL);
//       SOFA_CL_ERR(CL_INVALID_GLOBAL_WORK_SIZE);
#undef SOFA_CL_ERR
    default:
    {
        std::ostringstream o;
        o << err;
        return o.str();
    }
    }
}

void myopenclShowError(std::string file, int line)
{
    if(_error!=CL_SUCCESS && _error!=1)
    {
        std::cout << "Error (file '" << file << "' line " << line << "): " << myopenclErrorMsg(_error) << std::endl;
        sofa::helper::BackTrace::dump();
        exit(1);
    }
}

std::string myopenclPath()
{
    return _mainPath;
}

void myopenclBarrier(_device_pointer m, std::string file, int line)
{
    std::cout << file << " " << line << "\n";
    std::cout << "myopenclbarrier-------------------------------------------------\n";
    char p[1];
    myopenclEnqueueReadBuffer(0,p,m.m,0, 1);
    std::cout <<"~myopenclbarrier-------------------------------------------------\n";
}








_device_pointer deviceTmpArray;
size_t TmpArraySize = 0;
int valueTmpArray =0;
void* hostTmpArray = NULL;

void myopenclMemsetDevice(int d, _device_pointer dDestPointer, int value, size_t n)
{
    DEBUG_TEXT("myopenclMemsetDevice");
    if(TmpArraySize<n || value != valueTmpArray)
    {
        DEBUG_TEXT("myopenclMemsetDevice1");
        if(deviceTmpArray.m!=NULL)  myopenclReleaseBuffer(d,deviceTmpArray.m);
        if(hostTmpArray!=NULL)free(hostTmpArray) ;
        myopenclCreateBuffer(d,&(deviceTmpArray.m),n);
        deviceTmpArray.offset=0;
        hostTmpArray = malloc(n);
        memset((void*)hostTmpArray, value, n);
        myopenclEnqueueWriteBuffer(d, deviceTmpArray.m,0, hostTmpArray,n);
        TmpArraySize = n;
        valueTmpArray = value;
    }
    myopenclEnqueueCopyBuffer(d, dDestPointer.m, dDestPointer.offset, deviceTmpArray.m,0, n);

}



















#if defined(__cplusplus)
}
}
}
#endif

#undef DEBUG_TEXT
