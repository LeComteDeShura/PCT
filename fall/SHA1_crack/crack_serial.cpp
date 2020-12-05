#include <algorithm>
#include <openssl/sha.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

using namespace std;


void PasswGenerate(char *symbols, int length, int *hash)
{
  sort(symbols, symbols + length);
  unsigned char *digest = new unsigned char[SHA_DIGEST_LENGTH];
  long int PassQuantity = 0;

  do {
    //cout << symbols << endl;
    SHA1((unsigned char *) symbols, length, digest);
    for (int i = 0, match_count = 0; i < SHA_DIGEST_LENGTH; i++) {
      if (hash[i] == digest[i]) match_count++;
      if (match_count == SHA_DIGEST_LENGTH) cout << "matched password: " << symbols << endl;
    }
    PassQuantity++;
  } while (next_permutation(symbols, symbols + length));

  cout << endl << "total passwords: " << PassQuantity << endl;
  delete[] digest;
}


int main(int argc, char *argv[])
{
  if (argc != 3) {
    cout << "Launch format: ./[program_name] [character set] [password length]" << endl;
    return 1;
  }

  double t = clock();
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

  char *symbols = new char[length];
  strcpy(symbols, argv[1]);

  PasswGenerate(symbols, length, hash);

  t = (clock() - t) / CLOCKS_PER_SEC;
  cout << "time: " << t << endl;

  delete [] buf;
  delete [] symbols;
  return 0;
}
