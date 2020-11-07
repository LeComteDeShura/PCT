#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <cmath>

using namespace std;


const double eps = 1E-6;
const int n0 = 100000000;
const double a = 0.1, b = 1;


double wtime()
{
  struct timeval t;
  gettimeofday(&t, NULL);

  return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}


double func(double x)
{
  return log(1 + x) / x;
}


void runge_serial()
{
  double t = wtime();
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
  t = wtime() - t;
  cout << "serial time: " << t << endl;
  cout << "Result Pi: " << sq[k] * sq[k] << "; " << "Runge rule: EPS = " << eps << ", n = " << n / 2 << endl;
}


int main(int argc, char **argv)
{
  int commsize, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double t = MPI_Wtime();

  int n = n0, k;
  double sq[2], delta = 1;

  for (k = 0; delta > eps; n *= 2, k ^= 1) {
    int points_per_proc = n / commsize;
    int lb = rank * points_per_proc;
    int ub = (rank == commsize - 1) ? (n - 1) : (lb + points_per_proc - 1);

    double h = (b - a) / n;
    double s = 0.0;

    for (int i = lb; i <= ub; i++)
      s += func(a + h * (i + 0.5));

    MPI_Allreduce(&s, &sq[k], 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    sq[k] *= h;

    if (n > n0)
      delta = fabs(sq[k] - sq[k ^ 1]) / 3.0;
  }

  if (rank == 0)
    cout << "Result Pi: " << sq[k] * sq[k] << "; " << "Runge rule: EPS = " << eps << ", n = " << n / 2 << endl;

  t = MPI_Wtime() - t;
  double time_global;
  MPI_Reduce(&t, &time_global, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    cout << "commsize: " << commsize << endl;
    cout << "time: " << time_global << endl << endl;
    runge_serial();
  }

  MPI_Finalize();
  return 0;
}
