#include "myopencl.h"

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if(_context)clReleaseContext(_context);
    myopenclShowError(__FILE__, __LINE__);
}


void releaseQueues()
{
    for(int i=0; i<_numDevices; i++)
        if(_queues[i])
        {
            clReleaseCommandQueue(_queues[i]);
            myopenclShowError(__FILE__, __LINE__);
        }
}

void releaseDevices()
{
    if(_devices)delete(_devices);;
}


cl_context createContext(cl_device_type type)
{
    if(_context)clReleaseContext(_context);
    myopenclShowError(__FILE__, __LINE__);
    return (_context = clCreateContextFromType(0, type, NULL, NULL, &_error));
}

void createDevices()
{
    if(_devices)delete(_devices);
    size_t devices_size;
    clGetContextInfo(_context, CL_CONTEXT_DEVICES,0,NULL, &devices_size);		//compter le nombre de matériel
    _numDevices = devices_size/sizeof(cl_device_id);
    _devices = new cl_device_id[_numDevices];					//allouer un espace mémoire pour recueillir les matériels
    clGetContextInfo(_context, CL_CONTEXT_DEVICES,devices_size,_devices, NULL);	//créer une liste de matériel
    myopenclShowError(__FILE__, __LINE__);

}

void createQueues()
{
    if(_queues)releaseQueues();
    _queues = new cl_command_queue[_numDevices];
    for(int i=0; i<_numDevices; i++)
    {
        _queues[i] = clCreateCommandQueue(_context, _devices[i], 0, NULL);
        myopenclShowError(__FILE__, __LINE__);
    }
}

//opencl public functions

int myopenclInit(int /*device*/)
{
    DEBUG_TEXT("myopenclInit");
    createPath();
    createContext(CL_DEVICE_TYPE_GPU);
    createDevices();
    createQueues();
    if(_error==CL_SUCCESS)return 1;
    else return 0;
}

int myopenclClose()
{
    releaseQueues();
    releaseDevices();
    releaseQueues();
    if(_error==CL_SUCCESS)return 1;
    else return 0;
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
}

void myopenclReleaseBuffer(int /*device*/,cl_mem p)
{
    DEBUG_TEXT("myopenclReleaseBuffer ");
    _error = clReleaseMemObject((cl_mem) p);
    myopenclShowError(__FILE__, __LINE__);
}

void myopenclEnqueueWriteBuffer(int device,cl_mem ddest,size_t offset,const void* hsrc,size_t n)
{
    DEBUG_TEXT("myopenclEnqueueWriteBuffer");


    _error = clEnqueueWriteBuffer(_queues[device], ddest, CL_TRUE, offset, n, hsrc,0,NULL,NULL);
    myopenclShowError(__FILE__, __LINE__);
}


void myopenclEnqueueReadBuffer(int device,void* hdest,const cl_mem dsrc,size_t offset, size_t n)
{
    DEBUG_TEXT("myopenclEnqueueReadBuffer");
    _error = clEnqueueReadBuffer(_queues[device],  dsrc, CL_TRUE, offset, n,hdest,0,NULL,NULL);
    myopenclShowError(__FILE__, __LINE__);
}

void myopenclEnqueueCopyBuffer(int device, cl_mem ddest,size_t destOffset,const cl_mem dsrc,size_t srcOffset, size_t n)
{
    DEBUG_TEXT("myopenclEnqueueCopyBuffer");
    _error = clEnqueueCopyBuffer(_queues[device],dsrc,ddest,srcOffset,destOffset, n,0,NULL,NULL);
    myopenclShowError(__FILE__, __LINE__);
}

cl_program myopenclProgramWithSource(const char * s,const size_t size)
{
    DEBUG_TEXT("myopenclProgramWithSource");
    return clCreateProgramWithSource(_context, 1, &s, &size, &_error);
    myopenclShowError(__FILE__, __LINE__);
}

cl_kernel myopenclCreateKernel(void* p,const char * kernel_name)
{
    DEBUG_TEXT("myopenclCreateKernel");
    return clCreateKernel((cl_program)p, kernel_name, &_error);
    myopenclShowError(__FILE__, __LINE__);
}

void myopenclSetKernelArg(cl_kernel kernel,int num_arg,int size,void* arg)
{
    DEBUG_TEXT("myopenclSetKernelArg");
    _error = clSetKernelArg(kernel, num_arg,size, arg);
    myopenclShowError(__FILE__, __LINE__);
}


void myopenclBuildProgram(void * program)
{
    DEBUG_TEXT("myopenclBuildProgram");
    _error = clBuildProgram((cl_program)program,0,NULL,NULL,NULL,NULL);

    myopenclShowError(__FILE__, __LINE__);
}

void myopenclExecKernel(int device,cl_kernel kernel,unsigned int work_dim,const size_t *global_work_offset,const size_t *global_work_size,const size_t *local_work_size)
{
    DEBUG_TEXT("myopenclExecKernel");
    myopenclShowError(__FILE__, __LINE__);
    _error = clEnqueueNDRangeKernel(_queues[device],kernel,work_dim,global_work_offset,global_work_size,local_work_size,0,NULL,NULL);
    myopenclShowError(__FILE__, __LINE__);
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

void myopenclShowError(std::string file, int line)
{
    if(_error!=CL_SUCCESS && _error!=1)
    {
        std::cout << "Error (file '" << file << "' line " << line << "): " << _error << std::endl;
        if(_error==CL_INVALID_CONTEXT)std::cout << "\t\t-> CL_INVALID_CONTEXT\n";
        if(_error==CL_INVALID_PROGRAM)std::cout << "\t\t-> CL_INVALID_PROGRAM\n";
        if(_error==CL_INVALID_PROGRAM_EXECUTABLE)std::cout << "\t\t-> CL_INVALID_PROGRAM_EXECUTABLE\n";
        if(_error==CL_INVALID_KERNEL)std::cout << "\t\tCL_INVALID_KERNEL\n";
        if(_error==CL_INVALID_VALUE)std::cout << "\tCL_INVALID_VALUE\n";
        if(_error==CL_INVALID_KERNEL_ARGS)std::cout << "\tCL_INVALID_KERNEL_ARGS\n";
        exit(0);
    }
}

std::string myopenclPath()
{
    return _mainPath;
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
