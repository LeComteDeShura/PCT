#pragma once
#include <omp.h>
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

#define SIZE 10000000
#define THRESHOLD 10000

void partition(int *v, int &i, int &j, int low, int high);

void quicksort_serial(int *v, int low, int high);
void quicksort_nested(int *v, int low, int high);
void quicksort_nested_threshold(int *v, int low, int high);
void quicksort_tasks(int *v, int low, int high);
