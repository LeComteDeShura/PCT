#include "dgemv.hpp"


void matrix_vector_product(double *a, double *b, double *c, int m, int n)
{
  for (int i = 0; i < m; i++) {
    c[i] = 0.0;
    for (int j = 0; j < n; j++)
      c[i] += a[i * n + j] * b[j];
  }
}


void matrix_vector_product_omp(double *a, double *b, double *c, int m, int n)
{
  #pragma omp parallel
  {
    int nthreads = omp_get_num_threads();
    cout << "total threads: " << nthreads << endl;

    int threadid = omp_get_thread_num();
    cout << "thread: " << threadid << endl;

    int items_per_thread = m / nthreads;
    cout << "items per thread: " << items_per_thread << endl;

    int lb = threadid * items_per_thread;
    cout << "start on item: " << lb << endl;

    int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1);
    cout << "end on item: " << ub << endl;

    for (int i = lb; i <= ub; i++) {
      c[i] = 0.0;
      for (int j = 0; j < n; j++)
        c[i] += a[i * n + j] * b[j];
    }
  }
}


void run_serial()
{
  double *a, *b, *c;
  int m = 20000, n = 20000;

  a = (double *) malloc(sizeof(*a) * m * n);
  b = (double *) malloc(sizeof(*b) * n);
  c = (double *) malloc(sizeof(*c) * m);

  if (!a || !b || !c) {
    cout << "allocation fail" << endl;
    return;
  }

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++)
      a[i * n + j] = i + j;
    }
    for (int j = 0; j < n; j++)
      b[j] = j;

    matrix_vector_product(a, b, c, m, n);

    free(a);
    free(b);
    free(c);
}


void run_parallel()
{
  double *a, *b, *c;
  int m = 20000, n = 20000;

  a = (double *) malloc(sizeof(*a) * m * n);
  b = (double *) malloc(sizeof(*b) * n);
  c = (double *) malloc(sizeof(*c) * m);

  if (!a || !b || !c) {
    cout << "allocation fail" << endl;
    return;
  }

  #pragma omp parallel
  {
    int nthreads = omp_get_num_threads();
    int threadid = omp_get_thread_num();
    int items_per_thread = m / nthreads;
    int lb = threadid * items_per_thread;
    int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1);

    for (int i = lb; i <= ub; i++) {
      for (int j = 0; j < n; j++)
        a[i * n + j] = i + j;
      c[i] = 0.0;
    }
  }

  for (int j = 0; j < n; j++)
    b[j] = j;

  matrix_vector_product_omp(a, b, c, m, n);

  free(a);
  free(b);
  free(c);
}
