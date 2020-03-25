#include "lab3.hpp"


const double eps = 1E-5;
const int n0 = 100000000;
const double a = 0.1, b = 1;


double func(double x)
{
  return log(1 + x) / x;
}


void runge_serial()
{
  int n = n0, k;
  double sq[2], delta = 1;

  for (k = 0; delta > eps; n *= 2, k ^= 1) {
    double h = (b - a) / n;
    double s = 0.0;
    for (int i = 0; i < n; i++)
      s += func(a + h * (i + 0.5));
    sq[k] = s * h;
    if (n > n0)
      delta = fabs(sq[k] - sq[k ^ 1]) / 3.0;
  }
  printf("Result Pi: %.12f; Runge rule: EPS %e, n = %d\n", sq[k] * sq[k], eps, n / 2);
}


void runge_parallel()
{
  printf("Numerical integration: [%f, %f], n0 = %d, EPS = %f\n", a, b, n0, eps);

  double sq[2];
  #pragma omp parallel
  {
    int n = n0, k;
    double delta = 1;
    for (k = 0; delta > eps; n *= 2, k ^= 1) {
      double h = (b - a) / n;
      double s = 0.0;
      sq[k] = 0;

      #pragma omp barrier

      #pragma omp for nowait
      for (int i = 0; i < n; i++)
        s += func(a + h * (i + 0.5));

      #pragma omp atomic
      sq[k] += s * h;

      #pragma omp barrier
      if (n > n0)
        delta = fabs(sq[k] - sq[k ^ 1]) / 3.0;
      //printf("n = %d i = %d sq = %.12f delta = %.12f\n", n, k, sq[k], delta);
    }

    #pragma omp master
    printf("Result Pi: %.12f; Runge rule: EPS %e, n %d\n", sq[k] * sq[k], eps, n / 2);
  }
}
