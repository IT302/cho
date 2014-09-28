//For clarity,error checking has been omitted.
#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include "Util.hpp"
#include "test_vector.h"
using namespace std;

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
        platform = platforms[1];
        free(platforms);
    }

    /*Step 2:Query the platform and choose the first CPU device if has one.
     *Otherwise use the second CPU  device which should be intel.*/
    cl_uint             numDevices = 0;
    cl_device_id        *devices;
    /*status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);
    std::cout << "Choose CPU as default device."<<std::endl;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);
    devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);*/

    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
    std::cout << "Choose CPU as default device."<<std::endl;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
    devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);



    /*Step 3: Create context.*/
    cl_context context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);

    /*Step 4: Creating command queue associate with the context.*/
    cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, NULL);

    /*Step 5: Create program object */
    const char *filename = "kernel.cl";
    string sourceStr;
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


    auto  numA  = 8;
    auto  numB  = 4;
    std::vector<cl_int> InputA(numA);
    std::vector<cl_int> InputB(numB);
    std::vector<cl_int> OutputA(numA);
    std::vector<cl_int> OutputB(numA);


    for( int i  = 0;i < 2 ; ++i)
        for( int j  = 0;j < 2 ; ++j)
        {
            InputB[i*2 +j ] = inmvfs[i][j];
            //std::cout << inmvfs[i][j]<< "\n";
            for( int k  = 0;k < 2 ; ++k)
                {
                    InputA[i + 2 * (j + 2 * k)] = inPMV[i][j][k];
                    //std::cout << inPMV[i][j][k] << "\n";
                }

        }




    cl_int* inputA =  InputA.data();
    cl_int* inputB =  InputB.data();
    cl_int* outputA = OutputA.data();
    cl_int* outputB = OutputB.data();

    cl_mem inputBufferA = clCreateBuffer(context,
                                        CL_MEM_READ_ONLY|CL_MEM_ALLOC_HOST_PTR|CL_MEM_COPY_HOST_PTR,
                                        (size_t)(numA) * sizeof(cl_int),
                                        (void *)inputA,
                                        &status);
   if (status != CL_SUCCESS)
   {
       std::cout<<"Error: inputBufferA!"<<std::endl;
       std::cout << get_error_string(status)  <<std::endl;
       return 1;
    }

    cl_mem inputBufferB = clCreateBuffer(context,
                                        CL_MEM_READ_ONLY|CL_MEM_ALLOC_HOST_PTR|CL_MEM_COPY_HOST_PTR,
                                        (size_t)(numB) * sizeof(cl_int),
                                        (void *)inputB,
                                        &status);
   if (status != CL_SUCCESS)
   {
       std::cout<<"Error: inputBufferB!"<<std::endl;
       std::cout << get_error_string(status)  <<std::endl;
       return 1;
    }


    cl_mem outputBufferA = clCreateBuffer(context,
    CL_MEM_WRITE_ONLY|CL_MEM_ALLOC_HOST_PTR|CL_MEM_COPY_HOST_PTR,
    (size_t)(numA) * sizeof(cl_int),
    (void *)outputA,
    &status);

   if (status != CL_SUCCESS)
   {
    std::cout<<"Error: outputBufferA!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
   }

    cl_mem outputBufferB = clCreateBuffer(context,
    CL_MEM_WRITE_ONLY|CL_MEM_ALLOC_HOST_PTR|CL_MEM_COPY_HOST_PTR,
    (size_t)(numB) * sizeof(cl_int),
    (void *)outputB,
    &status);

   if (status != CL_SUCCESS)
   {
    std::cout<<"Error: outputBufferB!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
   }


    status = clEnqueueWriteBuffer (commandQueue,
                                    inputBufferA,
                                    CL_TRUE,
                                    0,
                                    (size_t)numA * sizeof(cl_int),
                                    (void *)inputA,
                                    0,
                                    NULL,
                                    NULL);

    if (status != CL_SUCCESS)
    {
    std::cout<<"Error: clEnqueueWriteBufferA!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }


    status = clEnqueueWriteBuffer (commandQueue,
                                    inputBufferB,
                                    CL_TRUE,
                                    0,
                                    (size_t)numB * sizeof(cl_int),
                                    (void *)inputB,
                                    0,
                                    NULL,
                                    NULL);

    if (status != CL_SUCCESS)
    {
    std::cout<<"Error: clEnqueueWriteBufferB!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }


    /*Step 8: Create kernel object */
    cl_kernel kernel = clCreateKernel(program,"motion_main", &status);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: creating kernels! : ";
        std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }



    /*Step 9: Sets Kernel arguments.*/
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBufferA);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 0!"<<std::endl;
        return 1;
    }

    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&inputBufferB);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 1!"<<std::endl;
        return 1;
    }

    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&outputBufferA);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 2!"<<std::endl;
        return 1;
    }

    status = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&outputBufferB);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 3!"<<std::endl;
        return 1;
    }

    std::cout<<"Lunching motion kernel!"<<std::endl;

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
        outputBufferA,
        CL_TRUE,
        0,
        (size_t)numA * sizeof(cl_int),
        (void *)outputA,
        0,
        NULL,
        NULL);

    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: clEnqueueReadBufferA!"<<std::endl;
        std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }



    status = clEnqueueReadBuffer (commandQueue,
        outputBufferB,
        CL_TRUE,
        0,
        (size_t)numB * sizeof(cl_int),
        (void *)outputB,
        0,
        NULL,
        NULL);

    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: clEnqueueReadBufferB!"<<std::endl;
        std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }
    //std::cout<<"\n\noutput data:"<<std::endl;

/*    for (cl_int i : Keys_out )
    {
        std::cout <<  i << "\n";

    }*/

    std::cout<<"verifying motion kernel results!"<<std::endl;

    for( int i  = 0;i < 2 ; ++i)
        for( int j  = 0;j < 2 ; ++j)
            for( int k  = 0;k < 2 ; ++k)
            {
                 if (outPMV[i][j][k] != outputA[i + 2 * (j + 2 * k)])
                    std::cout << i << "   outPMV failed\n";

            }


    for( int i  = 0;i < 2 ; ++i)
        for( int j  = 0;j < 2 ; ++j)
        {
             if (outmvfs[i][j] != OutputB[i*2 +j])
                std::cout <<  i << "outmvfs failed\n";

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
    status = clReleaseMemObject(inputBufferA);//Release mem object.
    status = clReleaseMemObject(inputBufferB);
    status = clReleaseMemObject(outputBufferA);
    status = clReleaseMemObject(outputBufferB);
    status = clReleaseCommandQueue(commandQueue);//Release  Command queue.
    status = clReleaseContext(context);//Release context.

    if (devices != NULL)
    {
        free(devices);
        devices = NULL;
    }

    return 0;
}
