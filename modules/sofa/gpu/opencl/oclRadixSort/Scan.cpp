/*
* Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
*
* NVIDIA Corporation and its licensors retain all intellectual property and
* proprietary rights in and to this software and related documentation.
* Any use, reproduction, disclosure, or distribution of this software
* and related documentation without an express license agreement from
* NVIDIA Corporation is strictly prohibited.
*
* Please refer to the applicable NVIDIA end user license agreement (EULA)
* associated with this source code for terms and conditions that govern
* your use of this NVIDIA software.
*
*/

#include "../OpenCLMemoryManager.h"
#include "Scan.h"

Scan::Scan(
    unsigned int numElements
) :
    mNumElements(numElements)
{

//	cl_int ciErrNum;
    if (numElements > MAX_WORKGROUP_INCLUSIVE_SCAN_SIZE)
    {
        OpenCLMemoryManager<cl_uint>::deviceAlloc(0,&d_Buffer, numElements / MAX_WORKGROUP_INCLUSIVE_SCAN_SIZE * sizeof(cl_uint));
    }

    //shrLog("Create and build Scan program\n");
//	size_t szKernelLength; // Byte size of kernel code

    cpProgram = new OpenCLProgram(OpenCLProgram::loadSource("oclRadixSort/Scan_b.cl"));
    cpProgram->buildProgram();

    ckScanExclusiveLocal1 = new sofa::helper::OpenCLKernel(cpProgram,"scanExclusiveLocal1");
    ckScanExclusiveLocal2 = new sofa::helper::OpenCLKernel(cpProgram,"scanExclusiveLocal2");
    ckUniformUpdate =  new sofa::helper::OpenCLKernel(cpProgram,"uniformUpdate");

}

Scan::~Scan()
{
//	cl_int ciErrNum;
    delete(ckScanExclusiveLocal1);
    delete(ckScanExclusiveLocal2);
    delete(ckUniformUpdate);

    if (mNumElements > MAX_WORKGROUP_INCLUSIVE_SCAN_SIZE)
    {
        clReleaseMemObject(d_Buffer.m);
    }
    delete(cpProgram);

}

// main exclusive scan routine
void Scan::scanExclusiveLarge(
    sofa::gpu::opencl::_device_pointer d_Dst,
    sofa::gpu::opencl::_device_pointer d_Src,
    unsigned int batchSize,
    unsigned int arrayLength
)
{
    //Check power-of-two factorization
    unsigned int log2L;
    unsigned int factorizationRemainder = factorRadix2(log2L, arrayLength);

    if(factorizationRemainder == 1)printf("Error: %s %d\n",__FILE__,__LINE__);

    //Check supported size range
    if((arrayLength >= MIN_LARGE_ARRAY_SIZE) && (arrayLength <= MAX_LARGE_ARRAY_SIZE))printf("Error: %s %d\n",__FILE__,__LINE__);


    //Check total batch size limit
    if( (batchSize * arrayLength) <= MAX_BATCH_ELEMENTS)printf("Error: %s %d\n",__FILE__,__LINE__);

    scanExclusiveLocal1(
        d_Dst,
        d_Src,
        (batchSize * arrayLength) / (4 * WORKGROUP_SIZE),
        4 * WORKGROUP_SIZE
    );

    scanExclusiveLocal2(
        d_Buffer,
        d_Dst,
        d_Src,
        batchSize,
        arrayLength / (4 * WORKGROUP_SIZE)
    );

    uniformUpdate(
        d_Dst,
        d_Buffer,
        (batchSize * arrayLength) / (4 * WORKGROUP_SIZE)
    );
}


void Scan::scanExclusiveLocal1(
    sofa::gpu::opencl::_device_pointer d_Dst,
    sofa::gpu::opencl::_device_pointer d_Src,
    unsigned int n,
    unsigned int size
)
{
//   cl_int ciErrNum;
    size_t localWorkSize, globalWorkSize;

    ckScanExclusiveLocal1->setArg<cl_mem>(0,&d_Dst.m);
    ckScanExclusiveLocal1->setArg<cl_mem>( 1, &d_Src.m);
    ckScanExclusiveLocal1->setArg(2, 2 * WORKGROUP_SIZE * sizeof(unsigned int), NULL);
    ckScanExclusiveLocal1->setArg<unsigned int>( 3 , &size);

    localWorkSize = WORKGROUP_SIZE;
    globalWorkSize = (n * size) / 4;

    ckScanExclusiveLocal1->execute(0, 1, NULL, &globalWorkSize, &localWorkSize);
//   oclCheckError(ciErrNum, CL_SUCCESS);
}

void Scan::scanExclusiveLocal2(
    sofa::gpu::opencl::_device_pointer d_Buffer,
    sofa::gpu::opencl::_device_pointer d_Dst,
    sofa::gpu::opencl::_device_pointer d_Src,
    unsigned int n,
    unsigned int size
)
{

    size_t localWorkSize, globalWorkSize;

    unsigned int elements = n * size;
    ckScanExclusiveLocal2->setArg<cl_mem>(0,&d_Buffer.m);
    ckScanExclusiveLocal2->setArg<cl_mem>(1,&d_Dst.m);
    ckScanExclusiveLocal2->setArg<cl_mem>(2,&d_Src.m);
    ckScanExclusiveLocal2->setArg( 3, 2 * WORKGROUP_SIZE * sizeof(unsigned int), NULL);
    ckScanExclusiveLocal2->setArg<unsigned int>(4,&elements);
    ckScanExclusiveLocal2->setArg<unsigned int>(5,&size);

    localWorkSize = WORKGROUP_SIZE;
    globalWorkSize = iSnapUp(elements, WORKGROUP_SIZE);

    ckScanExclusiveLocal2->execute(0, 1, NULL, &globalWorkSize, &localWorkSize);
}

void Scan::uniformUpdate(
    sofa::gpu::opencl::_device_pointer d_Dst,
    sofa::gpu::opencl::_device_pointer d_Buffer,
    unsigned int n
)
{
    size_t localWorkSize, globalWorkSize;

    ckUniformUpdate->setArg<cl_mem>(0,&d_Dst.m);
    ckUniformUpdate->setArg<cl_mem>(1,&d_Buffer.m);

    localWorkSize = WORKGROUP_SIZE;
    globalWorkSize = n * WORKGROUP_SIZE;

    ckUniformUpdate->execute(0, 1, NULL, &globalWorkSize, &localWorkSize);
}
