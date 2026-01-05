#ifndef CUDA_TEST_H
#define CUDA_TEST_H

#include <cuda_runtime.h>
#include <iostream>

// Kernel definition
__global__ void test_cuda_kernel()
{

}

void test_cuda()
{
    int runtimeVersion = 0;
    cudaError_t err = cudaRuntimeGetVersion(&runtimeVersion);
    if (err == cudaSuccess)
    {
        std::cout << "CUDA Runtime Version: " << runtimeVersion / 1000 << "." 
                  << (runtimeVersion % 100) / 10 << std::endl;
    }
    else
    {
        std::cout << "Failed to get CUDA runtime version: " 
                  << cudaGetErrorString(err) << std::endl;
        return;
    }

    int deviceCount = 0;
    err = cudaGetDeviceCount(&deviceCount);
    if (err != cudaSuccess)
    {
        std::cout << "Failed to get device count: " << cudaGetErrorString(err) << std::endl;
        return;
    }

    std::cout << "CUDA Device Count: " << deviceCount << std::endl;
    
    for (int i = 0; i < deviceCount; i++)
    {
        cudaDeviceProp deviceProp;
        err = cudaGetDeviceProperties(&deviceProp, i);
        if (err == cudaSuccess)
        {
            std::cout << "Device " << i << ": " << deviceProp.name 
                      << " (Compute Capability: " << deviceProp.major << "." 
                      << deviceProp.minor << ")" << std::endl;
        }
        else
        {
            std::cout << "Failed to get properties for device " << i << ": " 
                      << cudaGetErrorString(err) << std::endl;
        }
    }
    
}

#endif