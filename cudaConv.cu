#include <cuda_runtime.h>
#include <helper_cuda.h>
#include "header.h"


__global__  void checkIfMatch(int *img_cuda, int *obj_cuda, int numElements, int imgDim, int objDim, int startI, int startJ, double threshold, double *dev_res) {
    int thread = blockDim.x * blockIdx.x + threadIdx.x;


    // Increment the proper value of the arrray according to thread ID 
    if (thread < numElements){
        int i = thread / objDim;
        int j = thread % objDim;
        double p = img_cuda[(startI + i) * imgDim + (startJ + j)];
        double o = obj_cuda[i*objDim + j];
        dev_res[thread] = fabsf((p-o)/p);
    }
}

int isMatch(image_object img, image_object obj, int i, int j, double threshold){
    
    cudaError_t err = cudaSuccess;
    int dim2 = obj.dim * obj.dim;
    double *res = (double*)malloc(sizeof(double) * (img.dim * img.dim));
    // Allocate memory on GPU to copy the image from the host
    int *img_cuda;
    err = cudaMalloc((void **)&img_cuda, img.dim * img.dim * sizeof(int));
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
    // Copy image from host to the GPU memory
    err = cudaMemcpy(img_cuda, img.matrix, img.dim * img.dim * sizeof(int), cudaMemcpyHostToDevice);

    // Allocate memory on GPU to copy the object from the host
    int *obj_cuda;
    err = cudaMalloc((void **)&obj_cuda, dim2 * sizeof(int));
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
    // Copy object from host to the GPU memory
    err = cudaMemcpy(obj_cuda, obj.matrix, dim2 * sizeof(int), cudaMemcpyHostToDevice);

    double *dev_res;
    err = cudaMalloc((void **)&dev_res, dim2 * sizeof(double));
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Launch the Kernel
    int threadsPerBlock = 256;
    int blocksPerGrid =((dim2) + threadsPerBlock - 1) / threadsPerBlock;

    checkIfMatch<<<blocksPerGrid, threadsPerBlock>>>(img_cuda, obj_cuda, (dim2), img.dim, obj.dim, i, j, threshold, dev_res);
    err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to launch vectorAdd kernel -  %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    err = cudaMemcpy(res, dev_res, dim2 * sizeof(double), cudaMemcpyDeviceToHost);

    cudaFree(img_cuda);
    cudaFree(obj_cuda);
    cudaFree(dev_res);

    int k, sum = 0;
    for (k = 0 ; k < dim2 ; k++){
        sum += res[k];
    }
    if(sum/(dim2) < threshold)
        return 1;
    return 0;
}