#ifndef GLOBAL_H
#define GLOBAL_H
//#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <CL/cl.h>
#include <stdbool.h>

extern  cl_platform_id* platform;
extern  cl_device_id* device;
extern  cl_context context;
extern  cl_command_queue queue;
extern  cl_kernel kernel;
extern  cl_program program;

//#define INPUT_SIZE (4194304)
//#define INPUT_SIZE 2048*128*2
#define INPUT_SIZE 512
//#define INPUT_SIZE 1024 * 256
#define TAPS_SIZE (16)
#define KERNEL_PATH ((const char*)"D:\\fir-filter\\kernel.cl")

// Function prototypes
bool init();
void cleanup();
void display_device_info();
void error(cl_int status, const char* msg_to_print);
void check_cl_error(cl_int err_num, const char* msg);
char* readkernelFromFile(const char* filename);
long long cpuFilter(const short* input, const float* taps, float* output);
#endif // GLOBAL_H





