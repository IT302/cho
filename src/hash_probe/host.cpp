/*#include <stdio.h>

#define N 16
#define HLEN (N+N/2)

typedef struct {
	char valid;
	unsigned int key;
	unsigned int value;
} hash_entry;
volatile hash_entry ht[HLEN];
volatile unsigned int t1rids[N][N];
volatile int table1[N] = {
	10784, 13526, 32000, 22506, 26498, 14320, 5164, 28073, 6621,
	11413, 2759, 12222, 28137, 32000, 4709, 16120};
volatile int table2[N] = {
	6103, 26498, 6147, 930, 11339, 8966, 5499, 18370,
	15296, 27942, 28073, 23254, 26498, 28455, 32663, 32000};
volatile int outData[N];

#define SEED 5381
#define HASHC(v, h) ((((h) << 5) + (h)) ^ (v))
inline unsigned int hash(v) {
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

// hash probe
int main() {
	int i;
	int j;
	
	for (i = 0; i < HLEN; i++) {
		ht[i].valid = 0;
		ht[i].key = 0;
		ht[i].value = 0;
	}
	
	// Constructing the hash table
	for (i = 0; i < N; i++) {
		unsigned int index = hash(table1[i]) % HLEN;
		while (ht[index].valid && table1[i] != ht[index].key) {
			// Hash collision, next hash entry
			index = (index + 1) % HLEN;
		}
		// Insert T1 row 'i' in entry 'index'
		ht[index].valid = 1;
		ht[index].key = table1[i];
		t1rids[index][ht[index].value++] = i;
	}
	
	printf("Hash table:\n\n");
	for (i = 0; i < HLEN; i++) {
		if (!ht[i].valid)
			printf("Entry %u: Empty\n", i);
		else {
			printf("Entry %u: {valid:%u, key:%u, # T1 rids:%u}\n", i, ht[i].valid, ht[i].key, ht[i].value);
			for (j = 0; j < ht[i].value; j++)
				printf("  T1 rid %u\n", t1rids[i][j]);
		}
	}
	
	printf("\n\nStart\n");
	
	for (i = 0; i < N; i++) {
		unsigned int key = table2[i];
		unsigned int index = hash(key) % HLEN;
		while (ht[index].valid) {
			if (ht[index].key == key) {
				printf("Match: (key:%u, T1 vec sz:%u, T2 rid:%u)\n", key, ht[index].value, i);
				break;
			}
			index = (index + 1) % HLEN;
		}
	}
	
	printf("Finish\n");
	
	return 0;
}*/

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

#define N 16
#define HLEN (N+N/2)

typedef struct {
	unsigned int key;
	unsigned int value;
	char valid;
} hash_entry;
volatile hash_entry ht[HLEN];
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
int convertToString(const char *filename, std::string& s)
{
    size_t size;
    char*  str;
    std::fstream f(filename, (std::fstream::in | std::fstream::binary));

    if(f.is_open())
    {
        size_t fileSize;
        f.seekg(0, std::fstream::end);
        size = fileSize = (size_t)f.tellg();
        f.seekg(0, std::fstream::beg);
        str = new char[size+1];
        if(!str)
        {
            f.close();
            return 0;
        }

        f.read(str, fileSize);
        f.close();
        str[size] = '\0';
        s = str;
        delete[] str;
        return 0;
    }
    std::cout<<"Error: failed to open file\n:"<<filename<<std::endl;
    return -1;
}
int main(int argc, char* argv[])
{

	for (int i = 0; i < HLEN; i++) {
		ht[i].valid = 0;
		ht[i].key = 0;
		ht[i].value = 0;
	}

		// Constructing the hash table
	for (int i = 0; i < N; i++) {
		unsigned int index = hash(table1[i]) % HLEN;
		while (ht[index].valid && table1[i] != ht[index].key) {
			// Hash collision, next hash entry
			index = (index + 1) % HLEN;
		}
		// Insert T1 row 'i' in entry 'index'
		ht[index].valid = 1;
		ht[index].key = table1[i];
		t1rids[index][ht[index].value++] = i;
	}
	
	printf("Hash table:\n\n");
	for (int i = 0; i < HLEN; i++) {
		if (!ht[i].valid)
			printf("Entry %u: Empty\n", i);
		else {
			printf("Entry %u: {valid:%u, key:%u, # T1 rids:%u}\n", i,
			         ht[i].valid, ht[i].key, ht[i].value);
			for (int j = 0; j < ht[i].value; j++)
				printf("  T1 rid %u\n", t1rids[i][j]);
		}
	}
	for (int i = 0; i < HLEN; ++i)
	{
		printf("Entry %u: {valid:%u, key:%u, # T1 rids:%u}\n", 
			    i, ht[i].valid, ht[i].key, ht[i].value);

	}

	std::cout << "**********************************************************"<< std::endl;
	




    /*Step1: Getting platforms and choose an available one.*/
    cl_uint numPlatforms;//the NO. of platforms
    cl_platform_id platform;//the chosen platform
    cl_int  status = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: Getting platforms!"<<std::endl;
        return 1;
    }

    /*For clarity, choose the first available platform. */
    if(numPlatforms > 0)
    {
        cl_platform_id* platforms = (cl_platform_id*)malloc(numPlatforms* sizeof(cl_platform_id));
        status = clGetPlatformIDs(numPlatforms, platforms, NULL);
        platform = platforms[0];
        free(platforms);
    }

    /*Step 2:Query the platform and choose the first CPU device if has one.
     *Otherwise use the second CPU  device which should be intel.*/
    cl_uint             numDevices = 0;
    cl_device_id        *devices;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);
    std::cout << "Choose CPU as default device."<<std::endl;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);
    devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);



    /*Step 3: Create context.*/
    cl_context context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);

    /*Step 4: Creating command queue associate with the context.*/
    cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, NULL);
    if (status != CL_SUCCESS)
    {
       std::cout<<"Error: clCreateCommandQueue!"<<std::endl;
       std::cout << get_error_string(status)  <<std::endl;
       return 1;
    }

    /*Step 5: Create program object */
    const char *filename = "kernel.cl";
    std::string sourceStr;
    status = convertToString(filename, sourceStr);
    const char *source = sourceStr.c_str();
    size_t sourceSize[] = {strlen(source)};
    cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);

    /*Step 6: Build program. */
    std::string c_flags = std::string("-I ./");
    status=clBuildProgram(program, 1,devices,
                          c_flags.c_str(),NULL,NULL);
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


    auto  num  = 16;
    auto num_matches =  4;
    std::vector<cl_int> Keys_out(num_matches);
    //Keys_in.resize(num_in);
    //Keys_in.assign(inData, inData+num);

    cl_int* output = Keys_out.data();

    cl_mem inputBuffer = clCreateBuffer(context,
                                        CL_MEM_READ_ONLY|CL_MEM_ALLOC_HOST_PTR|CL_MEM_COPY_HOST_PTR,
                                        (size_t)(HLEN) * sizeof(hash_entry),
                                        (void *)ht,
                                        &status);
   if (status != CL_SUCCESS)
   {
       std::cout<<"Error: inputBuffer!"<<std::endl;
       std::cout << get_error_string(status)  <<std::endl;
       return 1;
    }


    cl_mem outputBuffer = clCreateBuffer(context,
    CL_MEM_WRITE_ONLY|CL_MEM_ALLOC_HOST_PTR|CL_MEM_COPY_HOST_PTR,
    (size_t)(num_matches) * sizeof(cl_int),
    (void *)ht,
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
                                    (size_t)HLEN * sizeof(hash_entry),
                                    (void *)ht,
                                    0,
                                    NULL,
                                    NULL);

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
                                    0,
                                    NULL,
                                    &kernel_exec_event);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: seting up clEnqueueNDRangeKernel!"<<std::endl;
        std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }

    /*Step 11: Read the std::cout put back to host memory.*/


    //status = clFinish(commandQueue);
    status = clWaitForEvents(1, &kernel_exec_event);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: couldn't finish!"<<std::endl;
        std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }


    status = clEnqueueReadBuffer (commandQueue,
        outputBuffer,
        CL_TRUE,
        0,
        (size_t)num_matches * sizeof(cl_int),
        (void *)output,
        0,
        NULL,
        NULL);

    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: clEnqueueReadBuffer!"<<std::endl;
        std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }

 

    //std::cout<<"\n\noutput data:"<<std::endl;

/*    for (cl_short i : Keys_out )
    {
        std::cout <<  i << "\n";

    }*/

    std::cout<<"verifying hash_probe kernel results!"<<std::endl;

    for (int i  = 0; i < num_matches; i++)
    {
        std::cout << "Key:" << output[i] << std::endl;

    }



    cl_ulong start = 0, end = 0;
    clGetEventProfilingInfo(kernel_exec_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventProfilingInfo(kernel_exec_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
     //END-START gives you hints on kind of “pure HW execution time”
     //the resolution of the events is 1e-09 sec
    auto g_NDRangePureExecTimeMs = (cl_double)(end - start)*(cl_double)(1e-06);

    std::cout<<"\n\nKernel1 Execution Time: "<< g_NDRangePureExecTimeMs << " ms"<<std::endl;



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