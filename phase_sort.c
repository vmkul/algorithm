#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define NUM_COUNT 100

#define ERR_HANDLE { \
  fprintf(stderr, "Line %d: ", __LINE__); \
  perror(""); \
  exit(1); }

int check_sorted(char *path) {
  FILE *file = fopen(path, "rb");
  if (!file) ERR_HANDLE;

  int current;
  int next;
  fread(&current, sizeof(int), 1, file);
  while (fread(&next, sizeof(int), 1, file) > 0) {
    if (current > next) {
      fclose(file);
      return 0;
    }
  }
  fclose(file);
  return 1;
}

int main() {
  FILE *file = fopen("numbers.bin", "wb");
  if (!file) ERR_HANDLE;

  srand(time(0));
  for (int i = 0; i < NUM_COUNT; i++) {
    int num = rand();
    fwrite(&num, sizeof(i), 1, file);
  }
  
  fclose(file);

  file = fopen("numbers.bin", "rb");
  if (!file) ERR_HANDLE;

  int number;
  while (fread(&number, sizeof(number), 1, file) > 0) {
    printf("%d ", number);
  }
  
  puts("");
  fclose(file);
  
  int result = check_sorted("numbers.bin");
  puts(result ? "Sorted" : "Not sorted");
  return 0;
}