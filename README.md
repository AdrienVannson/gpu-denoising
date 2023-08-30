# GPU denoising

## Machine Learning

The executed notebooks that were used to obtain the results of the machine learning part are in the `ML` folder. Notebooks have been created with different parameters (number of iterations during the training, available features). Another notebook was created to test the performance of Tensorflow.

The `ML/models` folder contains trained Tensorflow models that can be used directly.

## Measurements

The `Measurements modules` folder contains the modules that were used to realize different experiences on the performance of the GPU.
 - `bankconf` measures the performance impact of bank conflicts.
 - `comaperf` measures the peak performance of cooperative matrices.
 - `contflow` measures the performance impact of thread divergence.
 - `memgloba` benchmarks the global memory.
 - `memshare` benchmarks the shared memory.
 - `permgen` generates a permutation (this is used to benchmark the memory).

## Matrix products

The code allowing to perform matrix multiplications is available in the `Matrix products` folder. The most important part of the code lies in the `matmul` module: this module contains the shaders that actually perform the multiplication.

Moreover, other modules are also used:
 - `matconv` converts the coefficients of a matrix to `float16`.
 - `matgen` creates a random matrix.
 - `matmove` reorganizes the coefficients of a matrix.
 - `matread` reads a matrix from a file.
 - `matrieq` checks if two matrices are equal.

## CNN

The `CNN` folder contains the modules that are used to evalute the CNN with the GPU.
 - `bufread` reads a buffer from a file.
 - `bufimg` creates an image from a buffer.
 - `bufconv` converts the coefficients of a buffer to `float16`.
 - `tfcnn` evaluates the CNN.