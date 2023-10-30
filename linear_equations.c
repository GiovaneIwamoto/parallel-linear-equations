#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <math.h>

/*
Solve systems of linear equations Ax = b
Achieves the upper triangular form of the matrix A using the Gaussian elimination method
Perform back substitution to solve vector x which represents the solutions to the system
*/

// Parallel version
int solve_linear_system_parallel(double *A, double *x, double *b, int n, int threads) // Remove threads from parameters to allow custom input
{
    int num_threads = threads;
    printf("NUMBER OF THREADS %d: ", threads);
    // scanf("%d", &threads);
    printf("CALCULATING...\n");

    double temporary;
    int i, j, k;

    // Iterates over rows of matrix A
    for (i = 0; i < n - 1; i++)
    {
        // Parallel region started
#pragma omp parallel default(none) num_threads(num_threads) shared(n, A, b, i) private(j, k, temporary)
#pragma omp for schedule(static)
        for (j = i + 1; j < n; j++)
        {
            // Scalling factor to make element at pos (j,i) to zero
            temporary = (A[j * (n) + i]) / (A[i * (n) + i]);

            // Subtraction of elements in matrix A and vector b to eliminate elements below main diagonal
            for (k = i; k < n; k++)
            {
                A[j * (n) + k] -= temporary * (A[i * (n) + k]);
            }
            b[j] -= temporary * (b[i]);
        }
        // Achived upper triangular form of matrix A
    }
    double temp;
    // Solve vector x
#pragma omp parallel num_threads(num_threads) default(none) private(i, j) shared(A, b, x, n, temp)
    for (int i = n - 1; i >= 0; i--)
    {
#pragma omp single
        temp = b[i];

#pragma omp for reduction(+ : temp)
        for (j = i + 1; j < n; j++)
            temp += -A[i * n + j] * x[j];

#pragma omp single
        x[i] = temp / A[i * n + i];
    }
    return 0;
}

// Sequential version
int solve_linear_system_sequential(double *A, double *x, double *b, int n)
{
    int i, j, k;

    // Gaussian elimination to get an upper triangular matrix
    for (i = 0; i < n - 1; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            // Calc scaling factor to make the element at position (j, i) zero
            double temporary = (A[j * (n) + i]) / (A[i * (n) + i]);

            // Subtraction of elements in matrix A and vector b to eliminate elements below main diagonal
            for (k = i; k < n; k++)
            {
                A[j * (n) + k] -= temporary * (A[i * (n) + k]);
            }
            b[j] -= temporary * (b[i]);
        }
    }
    double temp;

    // Backward substitution to find the solution vector x
    for (int i = n - 1; i >= 0; i--)
    {
        temp = b[i];
        for (j = i + 1; j < n; j++)
            temp += -A[i * n + j] * x[j];
        x[i] = temp / A[i * n + i];
    }
    return 0;
}

int main(int argc, char *argv[])
{
    long dimension_matrix;
    printf("\nSYSTEMS OF LINEAR EQUATIONS A.X = B\n");
    printf("\nACHIEVES UPPER TRIANGULAR FORM OF THE MATRIX USING GAUSSIAN METHOD");
    printf("\nPERFORM BACK SUBSTITUTION TO SOLVE VECTOR\n");

    printf("\nSET MATRIX DIMENSION: ");
    scanf("%ld", &dimension_matrix);

    // Allocate memory for calculating parallel solution
    double *matrix_parallel = malloc(sizeof(double) * dimension_matrix * dimension_matrix);
    double *x_parallel = malloc(sizeof(double) * dimension_matrix);
    double *b_parallel = malloc(sizeof(double) * dimension_matrix);

    // Populate parallel matrix
    for (int i = 0; i < (dimension_matrix * dimension_matrix); i++)
    {
        double value = (rand() / (double)RAND_MAX); // Generate random integers
        matrix_parallel[i] = value;
    }

    // Populate parallel vector
    for (int i = 0; i < dimension_matrix; i++)
    {
        double value = (rand() / (double)RAND_MAX); // Generate random integers
        b_parallel[i] = value;
    }

    // Allocate memory for calculating sequential solution
    double *matrix_sequential = malloc(sizeof(double) * dimension_matrix * dimension_matrix);
    double *x_sequential = malloc(sizeof(double) * dimension_matrix);
    double *b_sequential = malloc(sizeof(double) * dimension_matrix);

    // Populate sequential matrix
    for (int i = 0; i < dimension_matrix * dimension_matrix; i++)
    {
        matrix_sequential[i] = matrix_parallel[i]; // Same value from parallel
    }

    // Populate sequential vector
    for (int i = 0; i < dimension_matrix; i++)
    {
        b_sequential[i] = b_parallel[i]; // Same value from parallel
    }

    // Timer for resolving parallel solutions 2 threads
    printf("\nPARALLEL SOLUTION 2 THREADS\n");
    double time_parallel_2threads = omp_get_wtime();
    solve_linear_system_parallel(matrix_parallel, x_parallel, b_parallel, dimension_matrix, 2);
    time_parallel_2threads = omp_get_wtime() - time_parallel_2threads;

    // Timer for resolving parallel solutions 4 threads
    printf("\nPARALLEL SOLUTION 4 THREADS\n");
    double time_parallel_4threads = omp_get_wtime();
    solve_linear_system_parallel(matrix_parallel, x_parallel, b_parallel, dimension_matrix, 4);
    time_parallel_4threads = omp_get_wtime() - time_parallel_4threads;

    // Timer for resolving parallel solutions 6 threads
    printf("\nPARALLEL SOLUTION 6 THREADS\n");
    double time_parallel_6threads = omp_get_wtime();
    solve_linear_system_parallel(matrix_parallel, x_parallel, b_parallel, dimension_matrix, 6);
    time_parallel_6threads = omp_get_wtime() - time_parallel_6threads;

    // Timer for resolving parallel solutions 8 threads
    printf("\nPARALLEL SOLUTION 8 THREADS\n");
    double time_parallel_8threads = omp_get_wtime();
    solve_linear_system_parallel(matrix_parallel, x_parallel, b_parallel, dimension_matrix, 8);
    time_parallel_8threads = omp_get_wtime() - time_parallel_8threads;

    // Timer for resolving sequential solutions
    printf("\nSEQUENTIAL SOLUTION:\n");
    printf("CALCULATING...\n");
    double time_sequential = omp_get_wtime();
    solve_linear_system_sequential(matrix_sequential, x_sequential, b_sequential, dimension_matrix);
    time_sequential = omp_get_wtime() - time_sequential;

    // Printing
    printf("\nEXECUTION TIME - PARALLEL 2 THREADS: %0.10f\n", time_parallel_2threads);
    printf("\nEXECUTION TIME - PARALLEL 4 THREADS: %0.10f\n", time_parallel_4threads);
    printf("\nEXECUTION TIME - PARALLEL 6 THREADS: %0.10f\n", time_parallel_6threads);
    printf("\nEXECUTION TIME - PARALLEL 8 THREADS: %0.10f\n", time_parallel_8threads);
    printf("\nEXECUTION TIME - SEQUENTIAL        : %0.10f\n", time_sequential);
    printf("\n------------------------------------\n");

    free(matrix_parallel);
    free(x_parallel);
    free(b_parallel);

    free(matrix_sequential);
    free(x_sequential);
    free(b_sequential);
    return 0;
}
