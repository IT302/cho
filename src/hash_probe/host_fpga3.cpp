
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

#define T_SIZE 69905066
#define H_SIZE 128*1024


typedef struct 
{
    unsigned short valid;
    unsigned short key;
    unsigned int ridptr;
} bucket_t;

bucket_t hashtable[H_SIZE] __attribute__((aligned(64)));


typedef struct 
{
    unsigned short key;
    unsigned int rid;
} row_t;

typedef struct 
{
    unsigned int ridptr;
    unsigned int rid;
} match_t;


row_t table[T_SIZE] __attribute__((aligned(64)));
match_t matches[T_SIZE] __attribute__((aligned(64)));

int main(int argc, char* argv[])
{
    std::ifstream inf;

    inf.open("/tmp/ht600.bin", std::ifstream::in | std::ios::binary);
    if (!inf.good()) {
        std::cout << "Error opening ht600.bin" << std::endl;
        return 1;
    }

    unsigned int index = 0;
    while (inf.good()) {
        inf.read(reinterpret_cast<char*>(&hashtable[index].valid),
            sizeof(hashtable[index].valid));
        inf.read(reinterpret_cast<char*>(&hashtable[index].key),
            sizeof(hashtable[index].key));
        inf.read(reinterpret_cast<char*>(&hashtable[index].ridptr),
            sizeof(hashtable[index].ridptr));

        index++;
    }
    inf.close();



    inf.open("/tmp/table400.bin", std::ifstream::in | std::ios::binary);
    if (!inf.good()) {
        std::cout << "Error opening table400.bin" << std::endl;
        return 1;
    }

    index = 0;
    while (inf.good() & index < T_SIZE) {
        inf.read(reinterpret_cast<char*>(&table[index].key),
            sizeof(table[index].key));
        inf.read(reinterpret_cast<char*>(&table[index].rid),
            sizeof(table[index].rid));

        index++;
    }
    inf.close();



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
 const char * binary_name = "kernel3.aocx";
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


int num_matches =  0;

std::vector<cl_event> events_write_buffer(2);
std::vector<cl_event> events_read_buffer(2);

cl_mem inputBuffer = clCreateBuffer(context,
    CL_MEM_READ_ONLY,
    (size_t)H_SIZE * sizeof(bucket_t),
    NULL,
    &status);
if (status != CL_SUCCESS)
{
 std::cout<<"Error: inputBuffer!"<<std::endl;
 std::cout << get_error_string(status)  <<std::endl;
 return 1;
}

cl_mem inputBuffer2 = clCreateBuffer(context,
    CL_MEM_READ_ONLY,
    (size_t)T_SIZE * sizeof(row_t),
    NULL,
    &status);
if (status != CL_SUCCESS)
{
 std::cout<<"Error: inputBuffer2!"<<std::endl;
 std::cout << get_error_string(status)  <<std::endl;
 return 1;
}


cl_mem outputBuffer = clCreateBuffer(context,
    CL_MEM_WRITE_ONLY,
    (size_t)T_SIZE * sizeof(match_t),
    NULL,
    &status);

if (status != CL_SUCCESS)
{
    std::cout<<"Error: outputBuffer2!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}

cl_mem outputBuffer2 = clCreateBuffer(context,
    CL_MEM_WRITE_ONLY,
    (size_t)1 * sizeof(cl_int),
    NULL,
    &status);

if (status != CL_SUCCESS)
{
    std::cout<<"Error: outputBuffer2!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}




status = clEnqueueWriteBuffer (commandQueue,
    inputBuffer,
    CL_FALSE,
    0,
    (size_t)H_SIZE * sizeof(bucket_t),
    (void *)hashtable,
    0,
    NULL,
    &events_write_buffer[0]);

if (status != CL_SUCCESS)
{
    std::cout<<"Error: clEnqueueWriteBuffer!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}


status = clEnqueueWriteBuffer (commandQueue,
    inputBuffer2,
    CL_FALSE,
    0,
    (size_t)T_SIZE * sizeof(row_t),
    (void *)table,
    0,
    NULL,
    &events_write_buffer[1]);

if (status != CL_SUCCESS)
{
    std::cout<<"Error: clEnqueueWriteBuffer2!"<<std::endl;
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

status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&inputBuffer2);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: setting up kernel argument no 1!"<<std::endl;
    return 1;
}

status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&outputBuffer);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: setting up kernel argument no 2!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}

status = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&outputBuffer2);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: setting up kernel argument no 3!"<<std::endl;
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
    CL_FALSE,
    0,
    (size_t)T_SIZE * sizeof(match_t),
    (void *)matches,
    1,
    &kernel_exec_event,
    &events_read_buffer[0]);

if (status != CL_SUCCESS)
{
    std::cout<<"Error: clEnqueueReadBuffer!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}

status = clEnqueueReadBuffer (commandQueue,
    outputBuffer2,
    CL_FALSE,
    0,
    (size_t)1 * sizeof(cl_int),
    (void *)&num_matches,
    1,
    &kernel_exec_event,
    &events_read_buffer[1]);

if (status != CL_SUCCESS)
{
    std::cout<<"Error: clEnqueueReadBuffer!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}


clWaitForEvents(events_read_buffer.size(), events_read_buffer.data());


 
        std::cout<<"verifying hash_probe kernel results!"<<std::endl;

        for (int i  = 0; i < 3; i++)
        {
            std::cout << "Key:" << matches[i].ridptr << std::endl;

        }

        std::cout << "Number matches:" << num_matches<< std::endl;



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
    status = clReleaseMemObject(inputBuffer2);
    status = clReleaseMemObject(outputBuffer);
    status = clReleaseMemObject(outputBuffer2);
    status = clReleaseCommandQueue(commandQueue);//Release  Command queue.
    status = clReleaseContext(context);//Release context.

    if (devices != NULL)
    {
        free(devices);
        devices = NULL;
    }

    return 0;
}