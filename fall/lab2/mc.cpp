#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <cmath>

using namespace std;


const double PI = 3.14159265358979323846;
const int n = 100000000;


double wtime()
{
  struct timeval t;
  gettimeofday(&t, NULL);

  return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}


double getrandX()
{
  double x = (double)rand() / RAND_MAX;

  if (x == 0 || x == 1)
    getrandX();
  else
    return x;
}


double getrandY(double x)
{
  double y = 1 - x;
  double rez = (double)rand() * y / RAND_MAX;

  if (rez == 0 || rez == y)
    getrandY(x);
  else
    return rez;
}


double func(double x, double y)
{
  return exp(pow((x + y), 2));
}


void mc_serial()
{
  double t = wtime();
  int in = 0;
  double s = 0;

  for (int i = 0; i < n; i++) {
    double x = getrandX();
    double y = getrandY(x);
    if (y < (1 - x)) {
      in++;
      s += func(x, y);
    }
  }
  double v = PI * in / n;
  double res = v * s / in;

  t = wtime() - t;
  cout << "serial time: " << t << endl;
  cout << "Result: " << res << ", n = " << n << endl;
}


int main(int argc, char **argv)
{
  int commsize, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double t = MPI_Wtime();

  srand(rank);
  int in = 0;
  double s = 0;

  for (int i = rank; i < n; i += commsize) {
    double x = getrandX();
    double y = getrandY(x);

    if (y <= sin(x)) {
      in++;
      s += func(x, y);
    }
  }

  int gin = 0;
  MPI_Reduce(&in, &gin, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  double gsum = 0.0;
  MPI_Reduce(&s, &gsum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    double v = PI * gin / n;
    double res = v * gsum / gin;
    cout << "Result: " << res << ", n = " << n << endl;
  }

  t = MPI_Wtime() - t;
  double time_global;
  MPI_Reduce(&t, &time_global, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    cout << "commsize: " << commsize << endl;
    cout << "time: " << time_global << endl << endl;
    mc_serial();
  }

  MPI_Finalize();
  return 0;
}
