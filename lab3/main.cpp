#include "lab3.hpp"


int main()
{
  double t = omp_get_wtime();
  runge_serial();
  t = omp_get_wtime() - t;
  cout << "serial time: " << t << endl << endl;

  t = omp_get_wtime();
  runge_parallel();
  t = omp_get_wtime() - t;
  cout << "parallel time: " << t << endl;
  cout << "__________________" << endl;

  t = omp_get_wtime();
  mc_serial();
  t = omp_get_wtime() - t;
  cout << "serial time: " << t << endl << endl;

  t = omp_get_wtime();
  mc_parallel();
  t = omp_get_wtime() - t;
  cout << "parallel time: " << t << endl;

  return 0;
}
