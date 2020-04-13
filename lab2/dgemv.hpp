#pragma once
#include <iostream>
#include <omp.h>

using namespace std;


void matrix_vector_product(double *a, double *b, double *c, int m, int n);
void matrix_vector_product_omp(double *a, double *b, double *c, int m, int n);
void run_serial();
void run_parallel();
