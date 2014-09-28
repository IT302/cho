//For clarity,error checking has been omitted.
#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include "Util.hpp"
#include "AAlloc.h"
#include "test_vector.h"


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
///
//  Attempt to create the program object from a cached binary.  Note that
//  on first run this will fail because the binary has not yet been created.
//
cl_program CreateProgramFromBinary(cl_context context, cl_device_id device, const char* fileName)
{
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL)
    {
        return NULL;
    }

    // Determine the size of the binary
    size_t binarySize;
    fseek(fp, 0, SEEK_END);
    binarySize = ftell(fp);
    rewind(fp);

    unsigned char *programBinary = new unsigned char[binarySize];
    fread(programBinary, 1, binarySize, fp);
    fclose(fp);

    cl_int errNum = 0;
    cl_program program;
    cl_int binaryStatus;

    program = clCreateProgramWithBinary(context,
                                        1,
                                        &device,
                                        &binarySize,
                                        (const unsigned char**)&programBinary,
                                        &binaryStatus,
                                        &errNum);
    delete [] programBinary;
    if (errNum != CL_SUCCESS)
    {
        std::cerr << "Error loading program binary." << std::endl;
        return NULL;
    }

    if (binaryStatus != CL_SUCCESS)
    {
        std::cerr << "Invalid binary for device" << std::endl;
        return NULL;
    }

    errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        // Determine the reason for the error
        char buildLog[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                              sizeof(buildLog), buildLog, NULL);

        std::cerr << "Error in program: " << std::endl;
        std::cerr << buildLog << std::endl;
        clReleaseProgram(program);
        return NULL;
    }

    return program;
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
        platform = platforms[2];
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

    /*status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
    std::cout << "Choose CPU as default device."<<std::endl;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
    devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);*/



    /*Step 3: Create context.*/
    cl_context context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);

    /*Step 4: Creating command queue associate with the context.*/
    cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, NULL);

    /*Step 5: Create program object */
    // const char *filename = "kernel.cl";
    // string sourceStr;
    // status = convertToString(filename, sourceStr);
    // const char *source = sourceStr.c_str();
    // size_t sourceSize[] = {strlen(source)};
    // cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);

    // /*Step 6: Build program. */
    // std::string c_flags = std::string("-I ./");
    // status=clBuildProgram(program, 1,devices,
    //                       c_flags.c_str(),NULL,NULL);
    // if (status != CL_SUCCESS)
    // {
    //     std::cout<<"Error: error building program!"<<std::endl;
    //     std::cout << get_error_string(status)  <<std::endl;

    //     auto error = status;

    //     // check build error and build status first
    //     clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_STATUS,
    //                 sizeof(cl_build_status), &status, NULL);

    //         // check build log
    //      size_t logSize;
    //         clGetProgramBuildInfo(program, devices[0],
    //                 CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
    //         auto programLog = (char*) calloc (logSize+1, sizeof(char));
    //         clGetProgramBuildInfo(program, devices[0],
    //                 CL_PROGRAM_BUILD_LOG, logSize+1, programLog, NULL);
    //         printf("Build failed; error=%d, status=%d, programLog:\n\n%s",
    //                 error, status, programLog);
    //         free(programLog);




    //     return 1;
    // }

    /*Step 7: Initial input,output for the host and create memory objects for the kernel*/

    cl_program program = CreateProgramFromBinary(context, devices[0], "kernel.ir");
    if (program == NULL)
    {
        std::cout << "Binary not loaded, create from source..." << std::endl;
        exit(1);
  
    }


    auto  num  = N;
    std::vector<cl_ulong, AAlloc::AlignedAllocator<cl_ulong, 128> > Keys_inA;
    std::vector<cl_ulong, AAlloc::AlignedAllocator<cl_ulong, 128> > Keys_inB;
    std::vector<cl_ulong, AAlloc::AlignedAllocator<cl_ulong, 128> > Keys_out;
    //Keys_in.resize(num_in);
    Keys_inA.assign(a_input, a_input + num);
    Keys_inB.assign(b_input, b_input + num);

    Keys_out.resize(num);



    cl_ulong* inputA =  Keys_inA.data();
    cl_ulong* inputB =  Keys_inB.data();
    cl_ulong* output = Keys_out.data();

    std::vector<cl_event> events_write_buffer(2);
    std::vector<cl_event> events_read_buffer(2);

    cl_mem inputBufferA = clCreateBuffer(context,
                                        CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR,
                                        (size_t)(num) * sizeof(cl_ulong),
                                        (void *)inputA,
                                        &status);
   if (status != CL_SUCCESS)
   {
       std::cout<<"Error: inputBufferA!"<<std::endl;
       std::cout << get_error_string(status)  <<std::endl;
       return 1;
    }

    cl_mem inputBufferB = clCreateBuffer(context,
                                        CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR,
                                        (size_t)(num) * sizeof(cl_ulong),
                                        (void *)inputB,
                                        &status);
   if (status != CL_SUCCESS)
   {
       std::cout<<"Error: inputBufferB!"<<std::endl;
       std::cout << get_error_string(status)  <<std::endl;
       return 1;
    }


    cl_mem outputBuffer = clCreateBuffer(context,
    CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR,
    (size_t)(num) * sizeof(cl_ulong),
    (void *)output,
    &status);

   if (status != CL_SUCCESS)
   {
    std::cout<<"Error: outputBuffer!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
    return 1;
   }




    status = clEnqueueWriteBuffer (commandQueue,
                                    inputBufferA,
                                    CL_TRUE,
                                    0,
                                    (size_t)num * sizeof(cl_ulong),
                                    (void *)inputA,
                                    0,
                                    NULL,
                                    &events_write_buffer[0]);

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
                                    (size_t)num * sizeof(cl_ulong),
                                    (void *)inputB,
                                    0,
                                    NULL,
                                    &events_write_buffer[1]);

    if (status != CL_SUCCESS)
    {
    std::cout<<"Error: clEnqueueWriteBufferB!"<<std::endl;
    std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }



    /*Step 8: Create kernel object */
    cl_kernel kernel = clCreateKernel(program,"dfadd_main", &status);
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


    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&outputBuffer);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 2!"<<std::endl;
        return 1;
    }

    std::cout<<"Lunching dfadd kernel!"<<std::endl;

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

    /*Step 11: Read the std::cout put back to host memory.*/



    status = clEnqueueReadBuffer (commandQueue,
        outputBuffer,
        CL_TRUE,
        0,
        (size_t)num * sizeof(cl_ulong),
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

    std::cout<<"verifying dfadd kernel results!"<<std::endl;

    for (int i  = 0; i < N; i++)
    {
        if (Keys_out[i] != z_output[i])
        {
            std::cout<<"dfadd failed"<<std::endl;
            std::cout << z_output[i] << " i " << i << "\n";
            break;
        }

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
    status = clReleaseMemObject(inputBufferA);//Release mem object.
    status = clReleaseMemObject(inputBufferB);
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
