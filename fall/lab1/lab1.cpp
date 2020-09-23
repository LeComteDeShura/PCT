#include <iostream>
#include <cstring>
#include <mpi.h>

using namespace std;

void ring(int argc, char **argv)
{
  int commsize, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);

  double t = MPI_Wtime();

  int count = 1;
  char *sbuf = new char[count];
  sbuf[0] = rank;
  char *rbuf = new char[count];

  int left = (rank - 1 + commsize) % commsize;
  int right = (rank + 1) % commsize;

  for (int i = 0; i < commsize - 1; i++) {
    MPI_Status s[2];
    MPI_Request reqs[2];
    MPI_Isend(sbuf, count, MPI_CHAR, right, 0, MPI_COMM_WORLD, &reqs[0]);
    MPI_Irecv(rbuf, count, MPI_CHAR, left, 0, MPI_COMM_WORLD, &reqs[1]);

    MPI_Waitall(2, reqs, s);

    cout << "send_to: " << right << endl;
    cout << "recieve_from: " << left << endl;

    char *p = sbuf;
    sbuf = rbuf;
    rbuf = p;
  }
  t = MPI_Wtime() - t;
  cout << "time: " << t << endl;

  delete sbuf;
  delete rbuf;
}


void one_to_all(int argc, char **argv)
{
  int commsize, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);

  double t = MPI_Wtime();

  int count = 1024;
  char *sbuf = new char[count];
  sbuf[0] = rank;
  char *rbuf = new char[count];

  if (rank == 0) {
    for (int i = 1; i < commsize; i++) {
      MPI_Request reqs;
      MPI_Status s;
      MPI_Isend(sbuf, count, MPI_CHAR, i, 0, MPI_COMM_WORLD, &reqs);
      MPI_Wait(&reqs, &s);
    }
  } else {
    MPI_Request reqs;
    MPI_Status s;
    MPI_Irecv(rbuf, count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &reqs);
    MPI_Wait(&reqs, &s);
  }

  t = MPI_Wtime() - t;
  cout << "time: " << t << endl;

  delete sbuf;
  delete rbuf;
  MPI_Finalize();
}


void all_to_one(int argc, char **argv)
{
  int commsize, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);

  double t = MPI_Wtime();

  int count = 1024;
  char *sbuf = new char[count];

  for (int i = 0; i < count; i++)
    sbuf[i] = '#';
  //sbuf[0] = rank;

  char *rbuf = new char[count * commsize];

  if (rank == 0) {
    for (int i = 1; i < commsize; i++) {
      MPI_Request reqs;
      MPI_Status s;
      MPI_Irecv(rbuf, count, MPI_CHAR, i, 0, MPI_COMM_WORLD, &reqs);
      MPI_Wait(&reqs, &s);
    }
  } else {
    MPI_Request reqs;
    MPI_Status s;
    MPI_Isend(sbuf, count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &reqs);
    MPI_Wait(&reqs, &s);
  }

  t = MPI_Wtime() - t;
  cout << "time: " << t << endl;

  delete sbuf;
  delete rbuf;
  MPI_Finalize();
}


void all_to_all(int argc, char **argv)
{
  int commsize, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);

  double t = MPI_Wtime();

  int count = 1024;
  char *sbuf = new char[count];

  for (int i = 0; i < count; i++)
    sbuf[i] = '#';

  char *rbuf = new char[count];

  for (int i = 0; i < commsize; i++) {
    MPI_Request reqs[2];
    MPI_Status s[2];
    MPI_Isend(sbuf, count, MPI_CHAR, i, 0, MPI_COMM_WORLD, &reqs[0]);
    MPI_Irecv(rbuf, count, MPI_CHAR, i, 0, MPI_COMM_WORLD, &reqs[1]);

    MPI_Waitall(2, reqs, s);
  }

  t = MPI_Wtime() - t;
  cout << "time: " << t << endl;

  delete sbuf;
  delete rbuf;
  MPI_Finalize();
}


int main(int argc, char **argv)
{
  //ring(argc, argv);
  //one_to_all(argc, argv);
  //all_to_one(argc, argv);
  all_to_all(argc, argv);

  return 0;
}
