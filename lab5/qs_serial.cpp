#include "quicksort.hpp"


void partition(int *v, int &i, int &j, int low, int high)
{
  i = low;
  j = high;
  int pivot = v[(low + high) / 2];

  do {
    while (v[i] < pivot) i++;
    while (v[j] > pivot) j--;
    if (i <= j) {
      swap(v[i], v[j]);
      i++;
      j--;
    }
  } while (i <= j);
}


void quicksort_serial(int *v, int low, int high)
{
  int i, j;
  partition(v, i, j, low, high);

  if (low < j)
    quicksort_serial(v, low, j);
  if (i < high)
    quicksort_serial(v, i, high);
}
