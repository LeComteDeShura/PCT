#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>

using namespace std;


const double PI = 3.14159265358979323846;
const int n = 10000000;


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


int main(int argc, char **argv)
{
  int commsize, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

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

  MPI_Finalize();
  return 0;
}
