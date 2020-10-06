#include <iostream>
#include <cstring>
#include <mpi.h>

using namespace std;

void ring()
{
  int commsize, rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);

  double t = MPI_Wtime();

  int count = 1024 * 1024;
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

    // cout << "send_to: " << right << endl;
    // cout << "recieve_from: " << left << endl;

    char *p = sbuf;
    sbuf = rbuf;
    rbuf = p;
  }
  t = MPI_Wtime() - t;

  double time_global;
  MPI_Reduce(&t, &time_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0)
    cout << "time: " << time_global << endl;

  delete sbuf;
  delete rbuf;
}


void one_to_all()
{
  int commsize, rank;
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
  double time_global;
  MPI_Reduce(&t, &time_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0)
    cout << "time: " << time_global << endl;

  delete sbuf;
  delete rbuf;
}


void all_to_one()
{
  int commsize, rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);

  double t = MPI_Wtime();

  int count = 1024;
  char *sbuf = new char[count];
  sbuf[0] = rank;
  char *rbuf = new char[count * commsize];

  if (rank == 0) {
    for (int i = 0; i < commsize; i++) {
      size_t nbytes = count * sizeof(char);
      char *p = rbuf + i * nbytes;
      if (rank == i) {
        memcpy(p, sbuf, nbytes);
      } else {
        MPI_Request reqs;
        MPI_Status s1, s2;
        MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &s1);
        int src = s1.MPI_SOURCE;
        MPI_Irecv(rbuf + src * nbytes, count, MPI_CHAR, src, 0, MPI_COMM_WORLD, &reqs);
        MPI_Wait(&reqs, &s2);
      }
    }
  } else {
    MPI_Request reqs;
    MPI_Status s;
    MPI_Isend(sbuf, count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &reqs);
    MPI_Wait(&reqs, &s);
  }

  t = MPI_Wtime() - t;
  double time_global;
  MPI_Reduce(&t, &time_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0)
    cout << "time: " << time_global << endl;

  delete sbuf;
  delete rbuf;
}


void all_to_all()
{
  int commsize, rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);

  double t = MPI_Wtime();
  int count = 1024;

  MPI_Request *reqs = new MPI_Request[2 * (commsize - 1)];
  char *sbuf = new char[count];
  sbuf[0] = rank;
  char *rbuf = new char[count * commsize];

  int nreqs = 0;
  for (int i = 0; i < commsize; i++) {
    char *p = rbuf + (ptrdiff_t)count * sizeof(char) * i;
    if (i != rank) {
      MPI_Isend(sbuf, count, MPI_CHAR, i, 0, MPI_COMM_WORLD, &reqs[nreqs++]);
      MPI_Irecv(p, count, MPI_CHAR, i, 0, MPI_COMM_WORLD, &reqs[nreqs++]);
    } else {
      memcpy(p, sbuf, count * sizeof(char));
    }
  }
  MPI_Waitall(nreqs, reqs, MPI_STATUSES_IGNORE);

  t = MPI_Wtime() - t;
  double time_global;
  MPI_Reduce(&t, &time_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0)
    cout << "time: " << time_global << endl;

  delete sbuf;
  delete rbuf;
  delete reqs;
}


int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  //ring();
  //one_to_all();
  //all_to_one();
  all_to_all();
  MPI_Finalize();

  return 0;
}
