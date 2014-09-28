
/*
 * MergeSort.cpp
 *
 *  Created on: 17 Sep 2013
 *      Author: geoffrey
 */
/**********************************************************************
Copyright �2012 Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

�   Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
�   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

//For clarity,error checking has been omitted.
#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include "Util.hpp"
//#include "test_vector.h"
#include "AAlloc.h"
#include "AOCL_Utils.h"

#define N 16
#define HLEN (N+N/2)

typedef struct {
    unsigned int key;
    unsigned int value;
    char valid;
} hash_entry __attribute__((aligned(16)));
//volatile hash_entry ht[HLEN];
volatile unsigned int t1rids[N][N];
volatile int table1[N] = {
    10784, 13526, 32000, 22506, 26498, 14320, 5164, 28073, 6621,
    11413, 2759, 12222, 28137, 32000, 4709, 16120};
    volatile int table2[N] = {
        6103, 26498, 6147, 930, 11339, 8966, 5499, 18370,
        15296, 27942, 28073, 23254, 26498, 28455, 32663, 32000};
//volatile int outData[N];

#define SEED 5381
#define HASHC(v, h) ((((h) << 5) + (h)) ^ (v))
        unsigned int hash(unsigned int v) {
            unsigned int _v = v;
            unsigned int _h = HASHC(_v & 0xff, SEED);
            _v = _v >> 8;
            _h = HASHC(_v & 0xff, _h);
            _v = _v >> 8;
            _h = HASHC(_v & 0xff, _h);
            _v = _v >> 8;
            _h = HASHC(_v & 0xff, _h);
            return _h;
        }

/* convert the kernel file into a string */

int main(int argc, char* argv[])
    {

        hash_entry *ht;
        posix_memalign((void **) &ht, 64, sizeof(hash_entry)*HLEN);

            for (int i = 0; i < HLEN; i++) {
                ht[i].valid = 0;
                ht[i].key = 0;
                ht[i].value = 0;
            }

       

    /*Step1: Getting platforms and choose an available one.*/
    cl_uint numPlatforms;//the NO. of platforms
    cl_platform_id platform = NULL;//the chosen platform
    cl_int  status = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: Getting platforms!"<<std::endl;
        return 1;
    }

    platform =  aocl_utils::findPlatform("Altera");
    if(platform == NULL) {
        printf("ERROR: Unable to find Altera OpenCL platform.\n");
        return false;
    }

    /*Step 2:Query the platform and choose the first CPU device if has one.
     *Otherwise use the second CPU  device which should be intel.*/
    cl_uint             numDevices = 0;
    cl_device_id        *devices;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, NULL, NULL, &numDevices);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: Querying  number devices!"<<std::endl;
        return 1;
    }

    devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));


    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1,devices, NULL);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: Getting device ids!"<<std::endl;
        return 1;
    }





    /*Step 3: Create context.*/
    cl_context context = clCreateContext(NULL,1, devices,NULL,NULL, &status);
       if (status != CL_SUCCESS)
   {
       std::cout<<"Error: lCreateContext!"<<std::endl;
       std::cout << get_error_string(status)  <<std::endl;
       return 1;
    }

    /*Step 4: Creating command queue associate with the context.*/
    cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);
    if (status != CL_SUCCESS)
    {
       std::cout<<"Error: clCreateCommandQueue!"<<std::endl;
       std::cout << get_error_string(status)  <<std::endl;
       return 1;
    }

    /*Step 5: Create program object */
    const char * binary_name = "kernel2.aocx";
    std::cout << "Kernel is " << binary_name << std::endl;

    cl_program program = aocl_utils::createProgramFromBinary(context, binary_name, devices, 1);

    /*Step 6: Build program. */
    status = clBuildProgram(program, 0, NULL, "", NULL, NULL);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: error building program!"<<std::endl;
        std::cout << get_error_string(status)  <<std::endl;

        auto error = status;

        // check build error and build status first
        clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_STATUS,
                    sizeof(cl_build_status), &status, NULL);

            // check build log
         size_t logSize;
            clGetProgramBuildInfo(program, devices[0],
                    CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
            auto programLog = (char*) calloc (logSize+1, sizeof(char));
            clGetProgramBuildInfo(program, devices[0],
                    CL_PROGRAM_BUILD_LOG, logSize+1, programLog, NULL);
            printf("Build failed; error=%d, status=%d, programLog:\n\n%s",
                    error, status, programLog);
            free(programLog);




        return 1;
    }



    /*Step 7: Initial input,output for the host and create memory objects for the kernel*/


    auto  num_in  = HLEN;
    auto num_out =  4;
    //std::vector<hash_entry, AAlloc::AlignedAllocator<hash_entry, 64> > Keys_in(num_in);
    std::vector<cl_int, AAlloc::AlignedAllocator<cl_int, 64> > Keys_out(num_out);
    //Keys_in.assign(ht, ht+num_in);

    hash_entry* input =  ht;
    cl_int* output = Keys_out.data();

    std::vector<cl_event> events_write_buffer(1);
    std::vector<cl_event> events_read_buffer(1);

    cl_mem inputBuffer = clCreateBuffer(context,
        CL_MEM_READ_ONLY,
        (size_t)(num_in) * sizeof(hash_entry),
        NULL,
        &status);
    if (status != CL_SUCCESS)
    {
       std::cout<<"Error: inputBuffer!"<<std::endl;
       std::cout << get_error_string(status)  <<std::endl;
       return 1;
   }


   cl_mem outputBuffer = clCreateBuffer(context,
    CL_MEM_WRITE_ONLY,
    (size_t)(num_out) * sizeof(cl_int),
    NULL,
    &status);

   if (status != CL_SUCCESS)
   {
    std::cout<<"Error: outputBuffer!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}




status = clEnqueueWriteBuffer (commandQueue,
    inputBuffer,
    CL_TRUE,
    0,
    (size_t)num_in * sizeof(hash_entry),
    (void *)input,
    0,
    NULL,
    &events_write_buffer[0]);

if (status != CL_SUCCESS)
{
    std::cout<<"Error: clEnqueueWriteBuffer!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}



    /*Step 8: Create kernel object */
cl_kernel kernel = clCreateKernel(program,"hash_main", &status);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: creating kernels! : ";
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}



    /*Step 9: Sets Kernel arguments.*/
status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: setting up kernel argument no 0!"<<std::endl;
    return 1;
}

status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&outputBuffer);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: setting up kernel argument no 1!"<<std::endl;
    return 1;
}


std::cout<<"Lunching hash kernel!"<<std::endl;

    /*Step 10: Running the kernel.*/
cl_event kernel_exec_event;
size_t global_work_size[1] = {1};
size_t local_work_size[1] = {1};
status = clEnqueueNDRangeKernel(commandQueue,
    kernel,
    1,
    NULL,
    global_work_size,
    local_work_size,
    events_write_buffer.size(),
    events_write_buffer.data(),
    &kernel_exec_event);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: seting up clEnqueueNDRangeKernel!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}




    status = clEnqueueReadBuffer (commandQueue,
        outputBuffer,
        CL_TRUE,
        0,
        (size_t)num_out * sizeof(cl_int),
        (void *)output,
        1,
        &kernel_exec_event,
        &events_read_buffer[0]);

    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: clEnqueueReadBuffer!"<<std::endl;
        std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }
    clWaitForEvents(1, &events_read_buffer[0]);


    //std::cout<<"\n\noutput data:"<<std::endl;

/*    for (cl_short i : Keys_out )
    {
        std::cout <<  i << "\n";

    }*/

        std::cout<<"verifying hash_probe kernel results!"<<std::endl;

        for (int i  = 0; i < num_out; i++)
        {
            std::cout << "Key:" << output[i] << std::endl;

        }



    cl_ulong start = 0, end = 0;

    double  pcie_time= 0;

    for (unsigned i = 0; i < events_write_buffer.size(); ++i)
    {
         clGetEventProfilingInfo(events_write_buffer[i], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
         clGetEventProfilingInfo(events_write_buffer[i], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
         pcie_time += (cl_double)(end - start)*(cl_double)(1e-06);

    }


    for (unsigned i = 0; i < events_read_buffer.size(); ++i)
    {
         clGetEventProfilingInfo(events_read_buffer[i], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
         clGetEventProfilingInfo(events_read_buffer[i], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
         pcie_time += (cl_double)(end - start)*(cl_double)(1e-06);

    }


    clGetEventProfilingInfo(kernel_exec_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventProfilingInfo(kernel_exec_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
     //END-START gives you hints on kind of “pure HW execution time”
     //the resolution of the events is 1e-09 sec
    auto g_NDRangePureExecTimeMs = (cl_double)(end - start)*(cl_double)(1e-06);

    std::cout<<"\n\nKernel Execution Time: "<< g_NDRangePureExecTimeMs << " ms\n"
             << "PCIE Transfer Time: "<< pcie_time << " ms\n"
             << "Total ExecutionTime: "<< g_NDRangePureExecTimeMs +  pcie_time << " ms"
             <<std::endl;



    /*Step 12: Clean the resources.*/
    status = clReleaseKernel(kernel);//*Release kernel.
    //status = clReleaseKernel(kernel2);
    status = clReleaseProgram(program); //Release the program object.
    status = clReleaseMemObject(inputBuffer);//Release mem object.
    //status = clReleaseMemObject(inputBuffer2);
    status = clReleaseMemObject(outputBuffer);
    //status = clReleaseMemObject(outputBuffer2);
    status = clReleaseCommandQueue(commandQueue);//Release  Command queue.
    status = clReleaseContext(context);//Release context.

    if (devices != NULL)
    {
        free(devices);
        devices = NULL;
    }

    return 0;
    }