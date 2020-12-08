#include <algorithm>
#include <cmath>
#include <mpi.h>
#include <openssl/sha.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;


char* itob(int num, char* buffer, int base)
{
    if (!num)
       return 0;

    char res;
    int i, LEN = strlen(buffer);

    if (num % base > 9) res = (char)(num % base - 10) + 'A';
        else res = (char)(num % base) + '0';

    for (i = LEN; i > -1; i--) buffer[i + 1] = buffer[i];
    buffer[0] = res;

    itob(num / base, buffer, base);

    return buffer;
}


void ithPermutation(int *perm, int set_length, int psw_length, int n)
{
  char num[80] = "";
  itob(n, num, set_length);     // Перевод в систему счисления в основание мощности алфавита

  // Запись числа в perm
  for (int i = 0; i < strlen(num); i++) {
    char k = num[i];
    if (num[i] >= 'A' && num[i] <= 'Z')
      perm[i] = strtol(&k, NULL, set_length);
    else
      perm[i] = strtol(&k, NULL, 10);
  }

  // Сдвиг массива на strlen(num) элементов вправо, чтобы можно было записать незначащие нули
  if (strlen(num) != psw_length) {
    for (int i = psw_length, j = strlen(num); j >= 0; i--, j--)
      perm[i] = perm[j];
  }

  // Добавления незначащих нулей, чтобы число было длины psw_length (110 -> 0110)
  for (int i = 0; i < psw_length - strlen(num); i++)
    perm[i] = 0;

  for (int i = 0; i < psw_length; i++)
    perm[i]++;
}


bool NextSet(int *a, int n, int m)
{
  int j = m - 1;

  while (j >= 0 && a[j] == n) j--;
  if (j < 0) return false;

  if (a[j] >= n) j--;
  a[j]++;

  if (j == m - 1) return true;

  for (int k = j + 1; k < m; k++)
    a[k] = 1;

  return true;
}


void PasswGenerate_parallel(int *lb, int *ub, char *symbols, int set_length, int psw_length, int *hash)
{
  int commsize, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  char *psw = new char[psw_length];
  psw[psw_length] = '\0';
  unsigned char *digest = new unsigned char[SHA_DIGEST_LENGTH];
  unsigned long PassQuantity = 0;
  int collisions = 0;
  int collisions_total = 0;
  bool ub_reached = false;

  do {
    int match_count = 0;
    for (int i = 0; i < psw_length; i++)
      if (lb[i] == ub[i]) match_count++;

    if (match_count == psw_length) ub_reached = true;

    for (int i = 0; i < psw_length; i++) psw[i] = symbols[lb[i] - 1];
    SHA1((unsigned char *) psw, psw_length, digest);
    for (int i = 0, match_count = 0; i < SHA_DIGEST_LENGTH; i++) {
      if (hash[i] == digest[i]) match_count++;
      if (match_count == SHA_DIGEST_LENGTH) {
        cout << "matched password: " << psw << endl;
        collisions++;
      }
    }

    PassQuantity++;
  } while (NextSet(lb, set_length, psw_length) && (ub_reached != true));

  //cout << "Passw: " << PassQuantity << endl;
  MPI_Reduce(&collisions, &collisions_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) cout << endl << "total collisions: " << collisions_total << endl;
  delete [] psw;
  delete [] digest;
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

  int set_length = strlen(argv[1]);
  int psw_length = atoi(argv[2]);
  unsigned long total_passwords = pow(set_length, psw_length);


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

  int set_size = set_length + 1;
  char *set_sorted = new char[set_size];
  set_sorted[set_length] = { 0 };
  int perm_lb[psw_length];
  int perm_ub[psw_length];


  strcpy(set_sorted, argv[1]);
  sort(set_sorted, set_sorted + set_length);

  unsigned long items_per_proc = total_passwords / commsize;
  unsigned long lb = (rank > 0) ? rank * items_per_proc : 0;
  unsigned long ub = (rank == commsize - 1) ? (total_passwords - 1) : (lb + items_per_proc - 1);
  ithPermutation(perm_lb, set_length, psw_length, lb);
  ithPermutation(perm_ub, set_length, psw_length, ub);

  PasswGenerate_parallel(perm_lb, perm_ub, set_sorted, set_length, psw_length, hash);
  t = MPI_Wtime() - t;

  double time_global;
  MPI_Reduce(&t, &time_global, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    cout << "commsize: " << commsize << endl;
    cout << "time: " << time_global << endl;
  }

  delete [] buf;
  delete [] set_sorted;

  MPI_Finalize();
  return 0;
}
