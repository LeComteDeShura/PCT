#include <algorithm>
//#include <mpi.h>
#include <openssl/sha.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;


void ithPermutation(int *perm, const int n, int i)
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



int main(int argc, char *argv[])
{
  // int commsize, rank;
  // MPI_Init(&argc, &argv);
  // MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  // MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc != 3) {
    cout << "Launch format: ./[program_name] [character set] [password length]" << endl;
    return 1;
  }

  int length = atoi(argv[2]);

  char *buf = new char[SHA_DIGEST_LENGTH * 2];
  int hash[SHA_DIGEST_LENGTH];
  cout << "hash: ";
  cin >> buf;

  char hash_part[2];
  for (int i = 0, j = 0; j < SHA_DIGEST_LENGTH * 2; i++, j += 2) {
    hash_part[0] = buf[j];
    hash_part[1] = buf[j + 1];
    hash[i] = strtol(hash_part, NULL, 16);
  }

  int *perm = new int[length];
  char *symbols = new char[length];
  char *set_sorted = new char[length];
  char *ub = new char[length];
  strcpy(symbols, argv[1]);
  strcpy(set_sorted, argv[1]);

  ithPermutation(perm, length, 698);
  sort(set_sorted, set_sorted + length);
  for (int i = 0; i < length; i++)
    ub[i] = set_sorted[perm[i]];

  cout << "ub: " << ub << endl;

  // for (int i = 0; i < length; i++)
  //   cout << perm[i];
  //
  // cout << endl;

  double t = clock();

  //PasswGenerate(symbols, length, hash);

  t = (clock() - t) / CLOCKS_PER_SEC;
  cout << "time: " << t << endl;

  delete [] set_sorted;
  delete [] perm;
  delete [] buf;
  delete [] symbols;
  return 0;
}
