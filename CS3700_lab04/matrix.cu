// CS3700 Example matrix multpilcation using GPU

#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#define TILE_WIDTH 2
#define WIDTH 6

// Kernel function execute by the device (GPU)
__global__ void
product (float *d_a, float *d_b, float *d_c, const int n) {
   int col = blockIdx.x * blockDim.x + threadIdx.x ;
   int row = blockIdx.y * blockDim.y + threadIdx.y ;

   float sum = 0;
   if (row < n && col < n) {
      for (int i = 0 ; i<n ; ++i) {
         sum += d_a[row * n + i ] * d_b[i * n + col] ;
      }
      d_c[row * n + col] = sum;
   }
}

__global__ void
sum (float *d_a, float *d_b, float *d_d, const int n) {
//   int col = blockIdx.x * blockDim.x + threadIdx.x ;
//   int row = blockIdx.y * blockDim.y + threadIdx.y ;

//   float sum = 0;
//   if (row < n && col < n) 
//   {
//      for (int i = 0 ; i<n ; ++i) 
//      {
//         sum += d_a[row * n + i ] * d_b[i * n + col] ;
//      }
//      d_c[row * n + col] = sum;
//   }
   
   for(int i = 0; i < n*n; i++)
       d_d[i] = d_a[i] + d_b[i];
}

// Utility function to print the input matrix
void printMatrix (float m[][WIDTH]) {
   int i, j;
   for (i = 0; i<WIDTH; ++i) {
      for (j = 0; j< WIDTH; ++j) {
         printf ("%d\t", (int)m[i][j]);
      }
      printf ("\n");
   }
}

// Main function execute by the host (CPU)
int main () {
   // host matrices
   float host_a[WIDTH][WIDTH],
         host_b[WIDTH][WIDTH],
         host_c[WIDTH][WIDTH],
         host_d[WIDTH][WIDTH];

   // device arrays
   float *device_a, *device_b, *device_c, *device_d;

   int i, j;

   // initialize host matrices using random numbers
   time_t t;
   srand ((unsigned) time(&t));

   for (i = 0; i<WIDTH; ++i) {
      for (j = 0; j<WIDTH; j++) {
         host_a[i][j] = (float) (rand() % 50);
         host_b[i][j] = (float) (rand() % 50);
      }
   }

   printf ("Matrix A:\n");
   printMatrix (host_a);
   printf ("\n");

   printf ("Matrix B:\n");
   printMatrix (host_b);
   printf ("\n");

   // allocate device memory for input matrices
   size_t deviceSize = WIDTH * WIDTH * sizeof (float);
   cudaMalloc ((void **) &device_a, deviceSize);
   cudaMalloc ((void **) &device_b, deviceSize);

   // copy host matrices to device
   cudaMemcpy (device_a, host_a, deviceSize, cudaMemcpyHostToDevice );
   cudaMemcpy (device_b, host_b, deviceSize, cudaMemcpyHostToDevice );

   // allocate device memory to store computed result
   cudaMalloc((void **) &device_c, deviceSize);
   cudaMalloc((void **) &device_d, deviceSize);

   dim3 dimBlock (WIDTH, WIDTH);
   dim3 dimGrid (WIDTH/TILE_WIDTH, WIDTH/TILE_WIDTH);
   product<<<dimGrid, dimBlock>>> (device_a, device_b, device_c, WIDTH);
   sum<<<dimGrid, dimBlock>>> (device_a, device_b, device_d, WIDTH);

   // copy result from device back to host
   cudaMemcpy (host_c, device_c, deviceSize, cudaMemcpyDeviceToHost);
   cudaMemcpy (host_d, device_d, deviceSize, cudaMemcpyDeviceToHost);

   // output the computed result matrix
   printf ("A x B: \n");
   printMatrix (host_c);
   printf ("A + B: \n");
   printMatrix (host_d);

   cudaFree (device_a);
   cudaFree (device_b);
   cudaFree (device_c);
   cudaFree (device_d);
   return 0;
}