#include "dgemv.hpp"


int main()
{
  double t = omp_get_wtime();
  run_serial();
  t = omp_get_wtime() - t;
  cout << "Elapsed time (serial): " << t << endl;

  t = omp_get_wtime();
  run_parallel();
  t = omp_get_wtime() - t;
  cout << "Elapsed time (parallel): " << t << endl;

  return 0;
}
