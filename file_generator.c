#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define NUM_COUNT 21

#define ERR_HANDLE { \
  fprintf(stderr, "Line %d: ", __LINE__); \
  perror(""); \
  exit(1); }

int main() {
  FILE *file = fopen("numbers.bin", "wb");
  if (!file) ERR_HANDLE;

  srand(time(0));
  for (int i = 0; i < NUM_COUNT; i++) {
    int num = rand() / 10000000;
    printf("%d ", num);
    fwrite(&num, sizeof(i), 1, file);
  }

  fclose(file);
  return 0;
}
