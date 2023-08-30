# GPU denoising

## Machine Learning

The executed notebooks that were used to obtain the results of the machine learning part are in the `ML` folder. Notebooks have been created with different parameters (number of iterations during the training, available features). Another notebook was created to test the performance of Tensorflow.

The `ML/models` folder contains trained Tensorflow models that can be used directly.

## Matrix products

The code allowing to perform matrix multiplications is available in the `Matrix products` folder. The most important part of the code lies in the `matmul` module: this module contains the shaders that actually perform the multiplication.

Moreover, other modules are also used:
 - `matconv` converts the coefficients of a matrix to `float16`.
 - `matgen` creates a random matrix.
 - `matmove` reorganizes the coefficients of a matrix.
 - `matread` reads a matrix from a file.
 - `matrieq` checks if two matrices are equal.