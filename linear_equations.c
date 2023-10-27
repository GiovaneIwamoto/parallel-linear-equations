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

int solve_linear_system_parallel(double *A, double *b, double *x, int n)
{
    int t;
    printf("\nNUMBER OF THREADS: ");
    scanf("%d", &t);
    double temp;
    int i, j, k;

    // Iterates over rows of matrix A
    for (i = 0; i < n - 1; i++)
    {
        // Parallel region started
#pragma omp parallel default(none) num_threads(t) shared(n, A, b, i) private(j, k, temp)
#pragma omp for schedule(static)
        for (j = i + 1; j < n; j++)
        {
            // Scalling factor to make element at pos (j,i) to zero
            temp = (A[j * (n) + i]) / (A[i * (n) + i]);

            // Subtraction of elements in matrix A and vector b to eliminate elements below main diagonal
            for (k = i; k < n; k++)
            {
                A[j * (n) + k] -= temp * (A[i * (n) + k]);
            }
            b[j] -= temp * (b[i]);
        }
        // Achived upper triangular form of matrix A
    }
    double tmp;
    // Solve vector x
#pragma omp parallel num_threads(t) default(none) private(i, j) shared(A, b, x, n, tmp)
    for (int i = n - 1; i >= 0; i--)
    {
#pragma omp single
        tmp = b[i];

#pragma omp for reduction(+ : tmp)
        for (j = i + 1; j < n; j++)
            tmp += -A[i * n + j] * x[j];

#pragma omp single
        x[i] = tmp / A[i * n + i];
    }
    return 0;
}

// TODO: Sequential system and Main

int main(int argc, char *argv[])
{
    return 0;
}