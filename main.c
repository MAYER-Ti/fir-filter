#include "global.h"

cl_platform_id* platform;
cl_device_id* device;
cl_context context;
cl_command_queue queue;
cl_kernel kernel;
cl_program program;

int main()
{
    cl_int status;
    if(!init()) {
        return -1;
    }
    // Set the memory elements
    printf("\nAllocating memory on the device.\n");
    short* inputs = malloc(sizeof(short)*INPUT_SIZE);
    float* outputs = malloc(sizeof(float)*INPUT_SIZE);
    float* taps = malloc(sizeof(float)*TAPS_SIZE);
    srand(time(0));

    //Generate a random input
    for(int i = 0; i < INPUT_SIZE; i++){
        if(i < TAPS_SIZE){
            if(i%2 == 0){
                inputs[i] = (short)0;
            }
            else{
                inputs[i] = (short)10;
            }
        }
        else{
            if(i%2 == 0){
                inputs[i] = (short)0;
            }
            else{
                inputs[i] = (short)20;
            }
        }
        outputs[i] = 0;
    }
    //Generate taps
    for(int i = 0; i < TAPS_SIZE; i++){
        if(i%2==0){
            taps[i] = (float)0.5;
        }
        else{
            taps[i] = (float)0;
        }
    }
    // Create memory buffer on the device for the vector

    cl_mem Input_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY, ((INPUT_SIZE) * sizeof(short)*2), NULL, &status);
    clCreateBuffer(context, CL_MEM_READ_ONLY, (INPUT_SIZE)*sizeof(short), NULL, &status);
    error(status, "Failed to create memory for input");
    cl_mem Output_clmem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, (INPUT_SIZE) * sizeof(float), NULL, &status);
    error(status, "Failed to create memory for output");
    cl_mem Taps_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY, (TAPS_SIZE) * sizeof(float), NULL, &status);
    error(status, "Failed to create memory for taps");
    //Create events for time control
    cl_event input_event, kernel_event, read_event;

    //Copy the buffers to the device
    status = clEnqueueWriteBuffer(queue, Input_clmem, CL_TRUE, 0, (INPUT_SIZE) * sizeof(short), inputs, 0, NULL, &input_event);
    status = clEnqueueWriteBuffer(queue, Taps_clmem, CL_TRUE, 0, (TAPS_SIZE) * sizeof(float), taps, 0, NULL, NULL);

    printf(stderr, "\nKernel initialization is complete.\n");

    // Set the arguments of the kernel
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&Input_clmem);
    error(status, "Failed to set kernel arg 1 - input_clmem");
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&Output_clmem);
    error(status, "Failed to set kernel arg 2 - output_clmem");
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&Taps_clmem);
    error(status, "Failed to set kernel arg 3 - taps_clmem");


    // Launch the Kernel
    status = clFinish(queue);
    error(status, "Failed to launch kernel");
    unsigned int razm = 1;
    while (razm < TAPS_SIZE)
      razm *= 2;
    size_t local_work_size[3] = {razm, 1, 1};
    unsigned int glsize;
    if(INPUT_SIZE < 1024)
      glsize = INPUT_SIZE;
     else
      glsize = 1024;


     size_t global_work_size[3] = {(glsize*2), 1, 1};
    status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, &kernel_event);
    error(status, "Failed to launch kernel");
    status = clFinish(queue);
    error(status, "Failed to finish (after execution)");

    printf(" Done! \n");

    // Read the cl memory Output_clmem on device to the host variable Output
    status = clEnqueueReadBuffer(queue, Output_clmem, CL_TRUE, 0, INPUT_SIZE * sizeof(float), outputs, 0, NULL, &read_event);
    error(status, "Failed read the variable output");

    // Wait for command queue to complete pending events
    status = clFinish(queue);
    error(status, "Failed to finish");

    printf("Comparing with CPU code...\n");
    float* result_cpu = malloc(sizeof(float)*INPUT_SIZE);
    long long timeCPU = cpuFilter(inputs, taps, result_cpu);
    printf("Done! \n");

    // Check times
    cl_ulong copy_start, copy_end, kernel_end, kernel_start, read_start, read_end;
    cl_ulong copy_time, kernel_time, read_time;
    clGetEventProfilingInfo(input_event, CL_PROFILING_COMMAND_START, sizeof(copy_start), &copy_start, NULL);
    clGetEventProfilingInfo(input_event, CL_PROFILING_COMMAND_END, sizeof(copy_end), &copy_end, NULL);
    clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_START, sizeof(kernel_start), &kernel_start, NULL);
    clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_END, sizeof(kernel_end), &kernel_end, NULL);
    clGetEventProfilingInfo(read_event, CL_PROFILING_COMMAND_START, sizeof(read_start), &read_start, NULL);
    clGetEventProfilingInfo(read_event, CL_PROFILING_COMMAND_END, sizeof(read_end), &read_end, NULL);
    copy_time = copy_end - copy_start;
    kernel_time = kernel_end - kernel_start;
    read_time = read_end - read_start;
 //   long long microseconds = (end.tv_sec - start.tv_sec)*1000000 +(end.tv_usec - start.tv_usec);
    printf("\nTime to copy memory in milliseconds = %f ms\n", (copy_time / 1000000.0) );
    printf("Time read output from buffer in milliseconds = %f ms\n", (read_time / 1000000.0));
    printf("Time execution in milliseconds GPU = %f ms\n", (kernel_time / 1000000.0));
    printf("Time execution in milliseconds CPU = %lld ms\n", timeCPU);



    printf("Results:\n");
    printf("taps\t-\tinput\t-\tgpu\t-\tcpu\n");
    for(int i = 0; i < INPUT_SIZE; i++){
        printf("%0.3f\t-\t%i\t-\t%0.3f\t-\t%0.3f\n", taps[i%TAPS_SIZE], inputs[i], outputs[i], result_cpu[i]);
    }

    // Free the resources allocated
    cleanup();
    free(taps);
    free(inputs);
    free(outputs);
    free(result_cpu);
    clReleaseMemObject(Input_clmem);
    clReleaseMemObject(Output_clmem);
    clReleaseMemObject(Taps_clmem);
    return 0;
}


