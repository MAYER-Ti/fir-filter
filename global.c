#include "global.h"
bool init() {
    cl_int status;

    //Set up the Platform
    cl_uint num_platforms = 0;
    status = clGetPlatformIDs(0, NULL, &num_platforms);
    error(status, "Failed count the platforms");
    platform = (cl_platform_id *)malloc(sizeof(cl_platform_id)*num_platforms);
    status = clGetPlatformIDs(num_platforms, platform, NULL);
    error(status, "Failed to create the platform list");

    // Query the available OpenCL devices.
    cl_uint num_devices;
    status = clGetDeviceIDs( platform[1], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
    error(status, "Failed count the devices");
    device = (cl_device_id *) malloc(sizeof(cl_device_id)*(num_devices));
    status = clGetDeviceIDs( platform[1], CL_DEVICE_TYPE_GPU, num_devices, device, NULL);
    error(status, "Failed to create the device list");

    // Display some device information.
    display_device_info();

    // Create the context.
    if(device == NULL){
        exit(1);
    }
    context = clCreateContext(NULL, num_devices, device, NULL, NULL, &status);
    error(status, "Failed to create context");

    // Create the command queue.
    queue = clCreateCommandQueue(context, device[0], CL_QUEUE_PROFILING_ENABLE, &status);
    error(status, "Failed to create command queue");

    // Create the program.   (From .cl text file)
    //------------------------------------


    char* sourceCode = readkernelFromFile(KERNEL_PATH);
    if(sourceCode == NULL){
        fprintf(stderr, "Failed to load kernel.\n");
        free(sourceCode);
        exit(1);
    }

    size_t sourceSize = _mbstrlen(sourceCode);
    //printf("!%s\n", sourceCode);
    //------------------------------------
    program = clCreateProgramWithSource(context, 1, (const char**)&sourceCode, &sourceSize, &status);
    error(status, "Failed to create program");

    // Build the program that was just created.
    status = clBuildProgram(program, 1, device, NULL, NULL, NULL);
    error(status, "Failed to build program");

    // Create the kernel
    kernel = clCreateKernel(program, "FirFilter", &status);
    error(status, "Failed to create kernel");

    return true;
}

char* readkernelFromFile(const char *filename){
    char* sourceCode;
    long fileSize;
    FILE* file = fopen(filename, "rb");
    if(file == NULL){
        printf("Open file failed: %s\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    rewind(file);

    sourceCode = (char*)malloc(sizeof(char)*(fileSize+1));
    if(NULL == sourceCode){
        printf("Error to set memory for source code\n");
        fclose(file);
        return NULL;
    }
    size_t result = fread(sourceCode, sizeof(char), fileSize, file);
    if(result != fileSize){
        printf("Error read file\n");
        return NULL;
    }
    sourceCode[fileSize] = '\0';
    fclose(file);
    return sourceCode;
}

// Query and display OpenCL information on device and runtime environment
void display_device_info() {

    printf("\nStarting OpenCL device query: \n");
    printf("------------------------------\n");
    cl_int err_num;
    char str_buffer[1024];
    cl_uint num_platforms_available;

    // Get the number of OpenCL capable platforms available
    err_num = clGetPlatformIDs(0, NULL, &num_platforms_available);
    check_cl_error(err_num, "clGetPlatformIDs: Getting number of available platforms");

    // Exit if no OpenCL capable platform found
    if(num_platforms_available == 0){
        printf("No OpenCL capable platforms found ! \n");
        exit(1);
    } else {
        printf("\n Number of OpenCL capable platforms available: %d \n", num_platforms_available);
        printf("--------------------------------------------------\n\n");
    }

    // Create a list for storing the platform id's
    cl_platform_id cl_platforms[num_platforms_available];

    err_num = clGetPlatformIDs(num_platforms_available, cl_platforms, NULL);
    check_cl_error(err_num, "clGetPlatformIDs: Getting available platform id's");

    // Get attributes of each platform available
    for(cl_uint platform_idx = 0; platform_idx <  num_platforms_available; platform_idx++) {
        printf("\t Platform ID: %d \n", platform_idx);
        printf("\t ----------------\n\n");

        // Get platform name
        err_num = clGetPlatformInfo(cl_platforms[platform_idx], CL_PLATFORM_NAME, sizeof(str_buffer), &str_buffer, NULL);
        check_cl_error(err_num, "clGetPlatformInfo: Getting platform name");
        printf("\t\t [Platform %d] CL_PLATFORM_NAME: %s\n", platform_idx, str_buffer);

        // Get platform vendor
        err_num = clGetPlatformInfo(cl_platforms[platform_idx], CL_PLATFORM_VENDOR, sizeof(str_buffer), &str_buffer, NULL);
        check_cl_error(err_num, "clGetPlatformInfo: Getting platform vendor");
        printf("\t\t [Platform %d] CL_PLATFORM_VENDOR: %s\n", platform_idx, str_buffer);

        // Get platform OpenCL version
        err_num = clGetPlatformInfo(cl_platforms[platform_idx], CL_PLATFORM_VERSION, sizeof(str_buffer), &str_buffer, NULL);
        check_cl_error(err_num, "clGetPlatformInfo: Getting platform version");
        printf("\t\t [Platform %d] CL_PLATFORM_VERSION: %s\n", platform_idx, str_buffer);

        // Get platform OpenCL profile
        err_num = clGetPlatformInfo(cl_platforms[platform_idx], CL_PLATFORM_PROFILE, sizeof(str_buffer), &str_buffer, NULL);
        check_cl_error(err_num, "clGetPlatformInfo: Getting platform profile");
        printf("\t\t [Platform %d] CL_PLATFORM_PROFILE: %s\n", platform_idx, str_buffer);

        //Get platform OpenCL supported extensions
        err_num = clGetPlatformInfo(cl_platforms[platform_idx], CL_PLATFORM_EXTENSIONS, sizeof(str_buffer), &str_buffer, NULL);
        check_cl_error(err_num, "clGetPlatformInfo: Getting platform supported extensions");
        printf("\t\t [Platform %d] CL_PLATFORM_EXTENSIONS: %s\n", platform_idx, str_buffer);



        // Get the number of OpenCL supported device available in this platform
        cl_uint num_devices_available;
        err_num = clGetDeviceIDs(cl_platforms[platform_idx], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices_available);
        check_cl_error(err_num, "clGetDeviceIDs: Get number of OpenCL supported devices available");
        printf("\n\t\t [Platform %d] Number of devices available: %d \n", platform_idx, num_devices_available);
        printf("\t\t ---------------------------------------------\n\n");
        cl_device_id cl_devices[num_devices_available];
        err_num = clGetDeviceIDs(cl_platforms[platform_idx], CL_DEVICE_TYPE_GPU, num_devices_available, cl_devices, NULL);
        check_cl_error(err_num, "clGetDeviceIDs: Getting available OpenCL capable device id's");

        // Get attributes of each device
        for(cl_uint device_idx = 0; device_idx < num_devices_available; device_idx++) {

            printf("\t\t\t [Platform %d] Device ID: %d\n", platform_idx, device_idx);
            printf("\t\t\t ---------------------------\n\n");

            // Get device name
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_NAME, sizeof(str_buffer), &str_buffer, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device name");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_NAME: %s\n", platform_idx, device_idx,str_buffer);

            // Get device hardware version
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_VERSION, sizeof(str_buffer), &str_buffer, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device hardware version");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_VERSION: %s\n", platform_idx, device_idx,str_buffer);

            // Get device software version
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DRIVER_VERSION, sizeof(str_buffer), &str_buffer, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device software version");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DRIVER_VERSION: %s\n", platform_idx, device_idx,str_buffer);

            // Get device OpenCL C version
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_OPENCL_C_VERSION, sizeof(str_buffer), &str_buffer, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device OpenCL C version");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_OPENCL_C_VERSION: %s\n", platform_idx, device_idx,str_buffer);

            // Get device max clock frequency
            cl_uint max_clock_freq = 0;
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(max_clock_freq), &max_clock_freq, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device max clock frequency");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_MAX_CLOCK_FREQUENCY: %d MHz\n", platform_idx, device_idx, max_clock_freq);

            // Get device max compute units available
            cl_uint max_compute_units_available;
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units_available), &max_compute_units_available, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device max compute units available");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", platform_idx, device_idx, max_compute_units_available);

            // Get device global mem size
            cl_ulong global_mem_size;
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(global_mem_size), &global_mem_size, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device global mem size");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_GLOBAL_MEM_SIZE: %llu MB\n", platform_idx, device_idx, (unsigned long long)global_mem_size/(1024*1024));

            // Get device max compute units available
            cl_ulong max_mem_alloc_size;
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device max mem alloc size");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_MAX_MEM_ALLOC_SIZE: %llu MB\n", platform_idx, device_idx, (unsigned long long)max_mem_alloc_size/(1024*1024));

            // Get device local mem size
            cl_ulong local_mem_size;
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(local_mem_size), &local_mem_size, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device local mem size");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_LOCAL_MEM_SIZE: %llu KB\n", platform_idx, device_idx, (unsigned long long)local_mem_size/1024);

            // Get device max work group size
            size_t max_work_group_size;
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device max work group size");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_MAX_WORK_GROUP_SIZE: %ld\n", platform_idx, device_idx, (long int)max_work_group_size);

            // Get device max work item dim
            cl_uint max_work_item_dims;
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(max_work_item_dims), &max_work_item_dims, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device max work item dimension");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %d\n", platform_idx, device_idx, max_work_item_dims);

            // Get device max work item sizes in each dimension
            size_t work_item_sizes[max_work_item_dims];
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(work_item_sizes), &work_item_sizes, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device max work items dimension");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_MAX_WORK_ITEM_SIZES: ", platform_idx, device_idx);
            for (size_t work_item_dim = 0; work_item_dim < max_work_item_dims; work_item_dim++) {
                printf("%ld ", (long int)work_item_sizes[work_item_dim]);
            }
            printf("\n");

            // Get device image support
            cl_bool image_support;
            err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_IMAGE_SUPPORT, sizeof(image_support), &image_support, NULL);
            check_cl_error(err_num, "clGetDeviceInfo: Getting device image support");
            printf("\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_IMAGE_SUPPORT: %u (%s)\n", platform_idx, device_idx, image_support, image_support? "Available" : "Not available");

            //            if(image_support) {

            //                size_t image_size;

            //                // Get device image 2d max width
            //                err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(image_size), &image_size, NULL);
            //                check_cl_error(err_num, "clGetDeviceInfo: Getting device image max 2d width");
            //                printf("\t\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_IMAGE2D_MAX_WIDTH: %li\n", platform_idx, device_idx, (long int)image_size);

            //                // Get device image 2d max height
            //                err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(image_size), &image_size, NULL);
            //                check_cl_error(err_num, "clGetDeviceInfo: Getting device image max 2d width");
            //                printf("\t\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_IMAGE2D_MAX_HEIGHT: %ld\n", platform_idx, device_idx, (long int)image_size);

            //                // Get device image 3d max width
            //                err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(image_size), &image_size, NULL);
            //                check_cl_error(err_num, "clGetDeviceInfo: Getting device image max 3d width");
            //                printf("\t\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_IMAGE3D_MAX_WIDTH: %ld\n", platform_idx, device_idx, (long int)image_size);

            //                // Get device image 3d max height
            //                err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(image_size), &image_size, NULL);
            //                check_cl_error(err_num, "clGetDeviceInfo: Getting device image max 3d height");
            //                printf("\t\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_IMAGE3D_MAX_HEIGHT: %ld\n", platform_idx, device_idx, (long int)image_size);

            //                // Get device image 2d max depth
            //                err_num = clGetDeviceInfo(cl_devices[device_idx], CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(image_size), &image_size, NULL);
            //                check_cl_error(err_num, "clGetDeviceInfo: Getting device image max 3d depth");
            //                printf("\t\t\t\t\t [Platform %d] [Device %d] CL_DEVICE_IMAGE3D_MAX_DEPTH: %ld\n", platform_idx, device_idx, (long int)image_size);

            //            }
            //            printf("\n\n\n");
        }
    }

}

void check_cl_error(cl_int err_num, const char* msg)
{
    if(err_num != CL_SUCCESS) {
        printf("[Error] OpenCL error code: %d in %s \n", err_num, msg);
        exit(EXIT_FAILURE);
    }
}


void error( cl_int status, const char* msg_to_print){

    if(status != 0){
        printf("ERROR: ");
        printf("%s", msg_to_print);
        printf(" (Error = %d)\n", status);
        exit(1);
    }

    return;
}

// Free the resources allocated during initialization
void cleanup() {
    if(kernel) {
        clReleaseKernel(kernel);
    }
    if(program) {
        clReleaseProgram(program);
    }
    if(queue) {
        clReleaseCommandQueue(queue);
    }
    if(context) {
        clReleaseContext(context);
    }
}

long long cpuFilter(const short* input, const float* taps, float* output) {
    struct timeval start, end;
    mingw_gettimeofday(&start, NULL);
    for(unsigned int i = 0; i <= INPUT_SIZE-TAPS_SIZE; i++){
        for(unsigned int j = 0; j < TAPS_SIZE; j++){
            //            if((i+j) > INPUT_SIZE)
            //                break;
            output[i] += input[i+j] * taps[j];
        }
    }
    mingw_gettimeofday(&end, NULL);
    long long microseconds = (end.tv_sec - start.tv_sec)*1000000 +(end.tv_usec - start.tv_usec);
    return (microseconds/1000);
}


