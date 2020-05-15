#include "quicksort.hpp"


void quicksort_nested(int *v, int low, int high)
{
  int i, j;
  partition(v, i, j, low, high);

  #pragma omp parallel sections num_threads(2)
  {
    #pragma omp section
    {
      if (low < j) quicksort_nested(v, low, j);
    }

    #pragma omp section
    {
      if (i < high) quicksort_nested(v, i, high);
    }
  }
}


void quicksort_nested_threshold(int *v, int low, int high)
{
  int i, j;
  partition(v, i, j, low, high);

  if (high - low < THRESHOLD || (j - low < THRESHOLD || high - i < THRESHOLD)) {
    if (low < j)
      quicksort_nested_threshold(v, low, j);
    if (i < high)
      quicksort_nested_threshold(v, i, high);
    } else {
      #pragma omp parallel sections num_threads(2)
      {
        #pragma omp section
        { quicksort_nested_threshold(v, low, j); }

        #pragma omp section
        { quicksort_nested_threshold(v, i, high); }
      }
    }
}


void quicksort_tasks(int *v, int low, int high)
{
  int i, j;
  partition(v, i, j, low, high);

  if (high - low < THRESHOLD || (j - low < THRESHOLD || high - i < THRESHOLD)) {
    if (low < j)
      quicksort_tasks(v, low, j);
    if (i < high)
      quicksort_tasks(v, i, high);
    } else {
      #pragma omp task untied
      {
        quicksort_tasks(v, low, j);
      }
      quicksort_tasks(v, i, high);
    }
}
