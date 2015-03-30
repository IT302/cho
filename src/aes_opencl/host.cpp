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



void fill_statement(cl_int statemt[32], cl_int key[32])
{
    /*
+--------------------------------------------------------------------------+
| * Test Vectors (added for CHStone)                                       |
|     statemt, key : input data                                            |
+--------------------------------------------------------------------------+
*/
statemt[0] = 50;
statemt[1] = 67;
statemt[2] = 246;
statemt[3] = 168;
statemt[4] = 136;
statemt[5] = 90;
statemt[6] = 48;
statemt[7] = 141;
statemt[8] = 49;
statemt[9] = 49;
statemt[10] = 152;
statemt[11] = 162;
statemt[12] = 224;
statemt[13] = 55;
statemt[14] = 7;
statemt[15] = 52;

key[0] = 43;
key[1] = 126;
key[2] = 21;
key[3] = 22;
key[4] = 40;
key[5] = 174;
key[6] = 210;
key[7] = 166;
key[8] = 171;
key[9] = 247;
key[10] = 21;
key[11] = 136;
key[12] = 9;
key[13] = 207;
key[14] = 79;
key[15] = 60;

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
    

    auto  num_in  = 32;
    auto  num_out  = 16;
    cl_int key[32];
    cl_int statemt[32];
    cl_int type = 128128;
    cl_int enc_or_dec = 0;

    fill_statement(statemt,key);
    std::vector<cl_int> Input1;
    std::vector<cl_int> Input2;
    std::vector<cl_int> Output(num_out);
    //Keys_in.resize(num_in);
    Input1.assign(statemt, statemt+32);
    Input2.assign(key, key+32);

    

/*    for (int i  = 0; i < 16; ++i )
   {
    printf("[%d] = %d\n", i , Input1[i]);
   }
   exit(10);
*/


   cl_int* input1 =  Input1.data();
   cl_int* input2 =  Input2.data();
   cl_int* output =  Output.data();

   cl_mem inputBuffer1 = clCreateBuffer(context,
    CL_MEM_READ_ONLY|CL_MEM_ALLOC_HOST_PTR|CL_MEM_COPY_HOST_PTR,
    (size_t)(num_in) * sizeof(cl_int),
    (void *)input1,
    &status);
   if (status != CL_SUCCESS)
   {
     std::cout<<"Error: inputBuffer1!"<<std::endl;
     std::cout << get_error_string(status)  <<std::endl;
     return 1;
 }

 cl_mem inputBuffer2 = clCreateBuffer(context,
    CL_MEM_READ_ONLY|CL_MEM_ALLOC_HOST_PTR|CL_MEM_COPY_HOST_PTR,
    (size_t)(num_in) * sizeof(cl_int),
    (void *)input2,
    &status);
 if (status != CL_SUCCESS)
 {
     std::cout<<"Error: inputBuffer2!"<<std::endl;
     std::cout << get_error_string(status)  <<std::endl;
     return 1;
 }
 cl_mem outputBuffer = clCreateBuffer(context,
    CL_MEM_WRITE_ONLY|CL_MEM_ALLOC_HOST_PTR|CL_MEM_COPY_HOST_PTR,
    (size_t)(num_out) * sizeof(cl_int),
    (void *)output,
    &status);

 if (status != CL_SUCCESS)
 {
    std::cout<<"Error: outputBuffer!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}




status = clEnqueueWriteBuffer (commandQueue,
    inputBuffer1,
    CL_TRUE,
    0,
    (size_t)num_in * sizeof(cl_int),
    (void *)input1,
    0,
    NULL,
    NULL);

if (status != CL_SUCCESS)
{
    std::cout<<"Error: clEnqueueWriteBuffer1!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}


status = clEnqueueWriteBuffer (commandQueue,
    inputBuffer2,
    CL_TRUE,
    0,
    (size_t)num_in * sizeof(cl_int),
    (void *)input2,
    0,
    NULL,
    NULL);

if (status != CL_SUCCESS)
{
    std::cout<<"Error: clEnqueueWriteBuffer2!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}

    /*Step 8: Create kernel object */
cl_kernel kernel = clCreateKernel(program,"aes_main", &status);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: creating kernels! : ";
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
}



    /*Step 9: Sets Kernel arguments.*/
status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer1);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: setting up kernel argument no 0!"<<std::endl;
    return 1;
}
        /*Step 9: Sets Kernel arguments.*/
status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&inputBuffer2);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: setting up kernel argument no 0!"<<std::endl;
    return 1;
}

status = clSetKernelArg(kernel, 2, sizeof(cl_int), &type) ;
if (status != CL_SUCCESS)
{
    std::cout<<"Error: setting up kernel argument no 2!"<<std::endl;
    return 1;
}

status = clSetKernelArg(kernel, 3, sizeof(cl_int), &enc_or_dec);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: setting up kernel argument no 3!"<<std::endl;
    return 1;
}


status = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&outputBuffer);
if (status != CL_SUCCESS)
{
    std::cout<<"Error: setting up kernel argument no 4!"<<std::endl;
    return 1;
}


std::cout<<"Lunching aes kernel !"<<std::endl;

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
    (size_t)num_out * sizeof(cl_int),
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

/*    for (cl_int i : Keys_out )
    {
        std::cout <<  i << "\n";

    }*/

        std::cout<<"verifying aes encrypt kernel results!"<<std::endl;

        for (int i  = 0; i < num_out; i++)
        {
            if (Output[i] != out_enc_statemt[i])
            {
                std::cout<<"aes encrypt failed"<<std::endl;
                std::cout << Output[i] << " i " << i << "\n";
                break;
            }

        }


        cl_ulong start = 0, end = 0;
        clGetEventProfilingInfo(kernel_exec_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
        clGetEventProfilingInfo(kernel_exec_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
     //END-START gives you hints on kind of “pure HW execution time”
     //the resolution of the events is 1e-09 sec
        auto g_NDRangePureExecTimeMs = (cl_double)(end - start)*(cl_double)(1e-06);

        std::cout<<"\n\nKernel1 Execution Time: "<< g_NDRangePureExecTimeMs << " ms"<<std::endl;

        


   /* enque decrpyt*/

    /* copy enc ouput */
        status = clEnqueueCopyBuffer ( commandQueue,
            outputBuffer,
            inputBuffer1,
            0,
            0,
            (size_t)(16) * sizeof(cl_int),
            0,
            NULL,
            NULL);

        if (status != CL_SUCCESS)
        {
         std::cout<<"Error: clEnqueueCopyBuffer!"<<std::endl;
         std::cout << get_error_string(status)  <<std::endl;
         return 1;
     }

     status = clEnqueueBarrier (commandQueue);

     if (status != CL_SUCCESS)
     {
         std::cout<<"Error: clEnqueueBarrier !"<<std::endl;
         std::cout << get_error_string(status)  <<std::endl;
         return 1;
     }

     int enc_or_dec2 = 2;
    // scalar args seems to retain their value from one call to another
    //hence int enc_or_dec2 = 2

     status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer1);
     if (status != CL_SUCCESS)
     {
        std::cout<<"Error: setting up kernel argument no 0!"<<std::endl;
        return 1;
    }
        /*Step 9: Sets Kernel arguments.*/
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&inputBuffer2);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 0!"<<std::endl;
        return 1;
    }

    status = clSetKernelArg(kernel, 2, sizeof(cl_int), &type) ;
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 2!"<<std::endl;
        return 1;
    }

    status = clSetKernelArg(kernel, 3, sizeof(cl_int), &enc_or_dec2);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 3!"<<std::endl;
        return 1;
    }


    status = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&outputBuffer);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 4!"<<std::endl;
        return 1;
    }

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

    status = clWaitForEvents(1, &kernel_exec_event);


    status = clEnqueueReadBuffer (commandQueue,
        outputBuffer,
        CL_TRUE,
        0,
        (size_t)num_out * sizeof(cl_int),
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


    std::cout<<"verifying aes decrypt kernel results!"<<std::endl;

    for (int i  = 0; i < num_out; i++)
    {
        if (Output[i] != out_dec_statemt[i])
        {
            std::cout<<"aes decrypt failed"<<std::endl;
            std::cout << Output[i] << " i " << i << "\n";
            break;
        }

    }

    start = end = 0;
    clGetEventProfilingInfo(kernel_exec_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventProfilingInfo(kernel_exec_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
     //END-START gives you hints on kind of “pure HW execution time”
     //the resolution of the events is 1e-09 sec
    g_NDRangePureExecTimeMs = (cl_double)(end - start)*(cl_double)(1e-06);

    std::cout<<"\n\nKernel2 Execution Time: "<< g_NDRangePureExecTimeMs << " ms"<<std::endl;


    /*Step 12: Clean the resources.*/
    status = clReleaseKernel(kernel);//*Release kernel.
    //status = clReleaseKernel(kernel2);
    status = clReleaseProgram(program); //Release the program object.
    status = clReleaseMemObject(inputBuffer1);//Release mem object.
    status = clReleaseMemObject(inputBuffer2);
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
