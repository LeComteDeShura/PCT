#include "lab3.hpp"


const double PI = 3.14159265358979323846;
const int n = 10000000;


// double getrandX()
// {
//   double x = (double)rand() / RAND_MAX;
//
//   if (x == 0 || x == 1)
//     getrandX();
//   else
//     return x;
// }
//
//
// double getrandY(double x)
// {
//   double y = 1 - x;
//   double rez = (double)rand() * y / RAND_MAX;
//
//   if (rez == 0 || rez == y)
//     getrandY(x);
//   else
//     return rez;
// }


double getrandX(unsigned int *seed)
{
  double x = (double)rand_r(seed) / RAND_MAX;

  if (x == 0 || x == 1)
    getrandX(seed);
  else
    return x;
}


double getrandY(unsigned int *seed, double x)
{
  double y = 1 - x;
  double rez = (double)rand_r(seed) * y / RAND_MAX;

  if (rez == 0 || rez == y)
    getrandY(seed, x);
  else
    return rez;
}


double func(double x, double y)
{
  return exp(pow((x + y), 2));
}


void mc_serial()
{
  int in = 0;
  double s = 0;
  unsigned int seed;

  for (int i = 0; i < n; i++) {
    seed = i;
    double x = getrandX(&seed);
    double y = getrandY(&seed, x);
    if (y < (1 - x)) {
      in++;
      s += func(x, y);
    }
  }
  double v = PI * in / n;
  double res = v * s / in;

  printf("Result: %.12f, n = %d\n", res, n);
}



void mc_parallel()
{
  printf("Numerical integration by Monte Carlo method: n = %d\n", n);

  int in = 0;
  double s = 0;

  #pragma omp parallel
  {
    double s_loc = 0;
    int in_loc = 0;
    unsigned int seed = omp_get_thread_num();

    #pragma omp for nowait
    for (int i = 0; i < n; i++) {
      double x = getrandX(&seed);
      double y = getrandY(&seed, x);
      if (y < (1 - x)) {
        in_loc++;
        s_loc += func(x, y);
      }
    }
    #pragma omp atomic
    s += s_loc;
    #pragma omp atomic
    in += in_loc;
  }
  double v = PI * in / n;
  double res = v * s / in;
  printf("Result: %.12f, n = %d\n", res, n);
}
