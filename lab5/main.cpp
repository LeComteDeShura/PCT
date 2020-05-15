#include "quicksort.hpp"


void fill_arr(int *arr)
{
  for (int i = 0; i < SIZE; i++) {
    arr[i] = rand() / 1000000;
  }
}


int main()
{
  int *arr = new int[SIZE];

  fill_arr(arr);

  double t = omp_get_wtime();
  quicksort_serial(arr, 0, SIZE - 1);
  t = omp_get_wtime() - t;
  cout << "serial time: " << t << endl << endl;

  fill_arr(arr);

  omp_set_nested(1);
  omp_set_max_active_levels(4);

  t = omp_get_wtime();
  quicksort_nested(arr, 0, SIZE - 1);
  t = omp_get_wtime() - t;
  cout << "max_active_levels: " << t << endl << endl;

  fill_arr(arr);

  t = omp_get_wtime();
  quicksort_nested_threshold(arr, 0, SIZE - 1);
  t = omp_get_wtime() - t;
  cout << "threshold: " << t << endl << endl;

  fill_arr(arr);

  t = omp_get_wtime();
  #pragma omp parallel
  {
    #pragma omp single
    quicksort_tasks(arr, 0, SIZE - 1);
  }
  t = omp_get_wtime() - t;
  cout << "tasks: " << t << endl << endl;


  delete arr;
  return 0;
}
