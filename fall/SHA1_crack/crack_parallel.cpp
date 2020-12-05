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
  int collisions = 0;
  int collisions_total = 0;

  do {
    //cout << symbols << endl;
    SHA1((unsigned char *) lb, length, digest);
    for (int i = 0, match_count = 0; i < SHA_DIGEST_LENGTH; i++) {
      if (hash[i] == digest[i]) match_count++;
      if (match_count == SHA_DIGEST_LENGTH) {
        cout << "matched password: " << lb << endl;
        collisions++;
      }
    }
    PassQuantity++;
  } while (next_permutation(lb, lb + length) && (strcmp(lb, ub) != 0));

  MPI_Reduce(&collisions, &collisions_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) cout << endl << "total collisions: " << collisions_total << endl;
  delete[] digest;
}


int main(int argc, char *argv[])
{
  int commsize, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double t = MPI_Wtime();

  if (argc != 4) {
    cout << "Launch format: ./[program_name] [character set] [password length] [hash]" << endl;
    return 1;
  }

  int length = atoi(argv[2]);
  long int total_passwords = factorial(length);

  if (total_passwords < commsize) {
    if (rank == 0) cout << "The number of processes must be not less than the number of passwords" << endl;
    return 1;
  }

  int buf_size = SHA_DIGEST_LENGTH * 2 + 1;
  char *buf = new char[buf_size];
  strcpy(buf, argv[3]);
  int hash[SHA_DIGEST_LENGTH];

  char hash_part[2];
  for (int i = 0, j = 0; j < SHA_DIGEST_LENGTH * 2; i++, j += 2) {
    hash_part[0] = buf[j];
    hash_part[1] = buf[j + 1];
    hash[i] = strtol(hash_part, NULL, 16);
  }

  int passw_size = length + 1;
  char *set_sorted = new char[passw_size];
  char *lb_psw = new char[passw_size];
  char *ub_psw = new char[passw_size];
  set_sorted[length] = { 0 };
  lb_psw[length] = { 0 };
  ub_psw[length] = { 0 };
  int perm_lb[length];
  int perm_ub[length];


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

  PasswGenerate_parallel(lb_psw, ub_psw, length, hash);
  t = MPI_Wtime() - t;

  double time_global;
  MPI_Reduce(&t, &time_global, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    cout << "commsize: " << commsize << endl;
    cout << "time: " << time_global << endl;
  }

  delete [] buf;
  delete [] set_sorted;
  delete [] lb_psw;
  delete [] ub_psw;

  MPI_Finalize();
  return 0;
}
