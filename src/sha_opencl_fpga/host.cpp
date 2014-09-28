#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include "Util.hpp"
#include "test_vector.h"
#include "AAlloc.h"
#include "AOCL_Utils.h"



int main(int argc, char* argv[])
{

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
    const char * binary_name = "kernel.aocx";
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



    auto num_in = VSIZE * BLOCK_SIZE; //Number of inputs;
    auto num_out = 5;

    std::vector<BYTE, AAlloc::AlignedAllocator<BYTE, 64> > Keys_in(num_in);
    std::vector<cl_int, AAlloc::AlignedAllocator<cl_int, 64> > Keys_out(num_out);

    //flatten array into buffer
    for( int  row = 0; row < VSIZE; row++)
    {
        for (int col = 0; col < BLOCK_SIZE; col++)
        {
            auto offset = row * BLOCK_SIZE + col;
            Keys_in[offset] = indata[row][col];
            //std::cout << "[" <<offset<<"] = "<< (int)Keys_in[row*BLOCK_SIZE + col] << "\n";
            //std::cout << "[" <<offset<<"] = "<< (int)indata[row][col] << "\n";
            //std::cout << offset << "\n";
            //printf("[%d][%d] = %d\n",row, col, indata[row][col] );
        }
    }


    BYTE* input =  Keys_in.data();
    cl_int* output = Keys_out.data();

    std::vector<cl_event> events_write_buffer(1);
    std::vector<cl_event> events_read_buffer(1);


    cl_mem inputBuffer = clCreateBuffer(context,
                                        CL_MEM_READ_ONLY,
                                        (size_t)(num_in) * sizeof(BYTE),
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
                                    (size_t)num_in * sizeof(BYTE),
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
    cl_kernel kernel = clCreateKernel(program,"sha_main", &status);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: creating kernels! : ";
        std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }

//    for (i = 0; i < IN_END; i += 2)
//      {
//        compressed[i / 2] = encode (test_data[i], test_data[i + 1]);
//      }


    /*Step 9: Sets Kernel arguments.*/
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 1!"<<std::endl;
        return 1;
    }

    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&outputBuffer);
    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: setting up kernel argument no 1!"<<std::endl;
        return 1;
    }

    std::cout<<"Lunching sha kernel!"<<std::endl;

    /*Step 10: Running the kernel.*/
    cl_event kernel_exec_event;
    status = clEnqueueTask( commandQueue,
                            kernel,
                            events_write_buffer.size(),
                            events_write_buffer.data(),
                            &kernel_exec_event);

    if (status != CL_SUCCESS)
    {
        std::cout<<"Error: seting up clEnqueueTask!"<<std::endl;
        std::cout << get_error_string(status)  <<std::endl;
        return 1;
    }

    /*Step 11: Read the std::cout put back to host memory.*/



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


    std::cout<<"verifying sha kernel results!"<<std::endl;

    for (int i  = 0; i < 5; i++)
    {
        if (Keys_out[i] != outData[i])
        {
            std::cout<<"sha hash failed"<<std::endl;
            std::cout << Keys_out[i] << " i " << i << "\n";
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
