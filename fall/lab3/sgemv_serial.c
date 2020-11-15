#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>
#include <math.h>


double wtime()
{
  struct timeval t;
  gettimeofday(&t, NULL);

  return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}


void dgemv(float *a, float *b, float *c, int m, int n)
{
  for (int i = 0; i < m; i++) {
    c[i] = 0.0;
    for (int j = 0; j < n; j++)
      c[i] += a[i * n + j] * b[j];
  }
}


int main(int argc, char **argv)
{
  int m = 9000, n = 9000;
  double t = wtime();

  float *a = malloc(sizeof(*a) * m * n);
  float *b = malloc(sizeof(*b) * n);
  float *c = malloc(sizeof(*c) * m);

  if (a == NULL || b == NULL || c == NULL) {
    printf("Memory allocation failed");
    return 1;
  }

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++)
      a[i * n + j] = i + 1;
    }

  for (int j = 0; j < n; j++)
    b[j] = j + 1;

  dgemv(a, b, c, m, n);
  t = wtime() -t;

  // Валидация и вывод
  for (int i = 0; i < m; i++) {
    double r = (i + 1) * (n / 2.0 + pow(n, 2) / 2.0);
    if (fabs(c[i] - r) > 1E-6) {
      fprintf(stderr, "Validation failed: elem %d = %f (real value %f)\n", i, c[i], r);
      break;
    }
  }

  double gflop = 2.0 * m * n * 1E-9;
  printf("Memory used: %"PRIu64 " MiB\n", (uint64_t)(((float)m * n + m + n) * sizeof(float)) >> 20);
  printf("Elapsed time (serial): %.6f sec.\n", t);
  printf("Performance: %.2f GFLOPS\n", gflop / t);

  free(a); free(b); free(c);
  return 0;
}
