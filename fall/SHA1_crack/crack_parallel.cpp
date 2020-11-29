#include <algorithm>
#include <mpi.h>
#include <openssl/sha.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;


long int factorial(int number)
{
  long int fact = 1;

  for (int i = 1; i <= number; i++)
    fact *= i;

  return fact;
}


void ithPermutation(int *perm, const int n, long int i)
{
   int j, k = 0;
   int *fact = (int *)calloc(n, sizeof(int));

   // compute factorial numbers
   fact[k] = 1;
   while (++k < n) fact[k] = fact[k - 1] * k;

   // compute factorial code
   for (k = 0; k < n; ++k) {
     perm[k] = i / fact[n - 1 - k];
     i = i % fact[n - 1 - k];
   }

   // readjust values to obtain the permutation
   // start from the end and check if preceding values are lower
   for (k = n - 1; k > 0; --k)
     for (j = k - 1; j >= 0; --j)
       if (perm[j] <= perm[k])
         perm[k]++;

   free(fact);
}


void PasswGenerate_parallel(char *lb, char *ub, int length, int *hash)
{
  int commsize, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  unsigned char *digest = new unsigned char[SHA_DIGEST_LENGTH];
  long int PassQuantity = 0;

  do {
    //cout << symbols << endl;
    SHA1((unsigned char *) lb, length, digest);
    for (int i = 0, match_count = 0; i < SHA_DIGEST_LENGTH; i++) {
      if (hash[i] == digest[i]) match_count++;
      if (match_count == SHA_DIGEST_LENGTH) cout << "matched password: " << lb << endl;
    }
    PassQuantity++;
  } while (next_permutation(lb, lb + length) && (strcmp(lb, ub) != 0));

  //cout << endl << "total passwords of " << rank << " process: " << PassQuantity << endl;
  delete[] digest;
}


int main(int argc, char *argv[])
{
  int commsize, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc != 4) {
    cout << "Launch format: ./[program_name] [character set] [password length] [hash]" << endl;
    return 1;
  }

  int length = atoi(argv[2]);
  long int total_passwords = factorial(length);

  if (total_passwords < commsize) {
    if (rank == 0) cout << "The number of processes must be more than the number of passwords" << endl;
    return 1;
  }

  char *buf = new char[SHA_DIGEST_LENGTH * 2];
  strcpy(buf, argv[3]);
  int hash[SHA_DIGEST_LENGTH];

  char hash_part[2];
  for (int i = 0, j = 0; j < SHA_DIGEST_LENGTH * 2; i++, j += 2) {
    hash_part[0] = buf[j];
    hash_part[1] = buf[j + 1];
    hash[i] = strtol(hash_part, NULL, 16);
  }

  int *perm_lb = new int[length];
  int *perm_ub = new int[length];
  char *set_sorted = new char[length];
  char *lb_psw = new char[length];
  char *ub_psw = new char[length];
  lb_psw[length] = { 0 };
  ub_psw[length] = { 0 };


  strcpy(set_sorted, argv[1]);
  sort(set_sorted, set_sorted + length);

  long int items_per_proc = total_passwords / commsize;
  long int lb = (rank > 0) ? rank * items_per_proc : 0;
  long int ub = (rank + 1) * items_per_proc;
  ithPermutation(perm_lb, length, lb);
  ithPermutation(perm_ub, length, ub);

  for (int i = 0; i < length; i++)
    lb_psw[i] = set_sorted[perm_lb[i]];

  for (int i = 0; i < length; i++)
    ub_psw[i] = set_sorted[perm_ub[i]];

  double t = MPI_Wtime();
  PasswGenerate_parallel(lb_psw, ub_psw, length, hash);
  t = MPI_Wtime() - t;

  double time_global;
  MPI_Reduce(&t, &time_global, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    cout << "commsize: " << commsize << endl;
    cout << "time: " << time_global << endl;
  }

  delete [] buf;
  delete [] perm_lb;
  delete [] perm_ub;
  delete [] set_sorted;
  delete [] lb_psw;
  delete [] ub_psw;

  MPI_Finalize();
  return 0;
}
