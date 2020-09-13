#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define SERIES_COUNT 165580141

#define ERR_HANDLE { \
  fprintf(stderr, "Line %d: ", __LINE__); \
  perror(""); \
  exit(1); }

int main() {
  FILE *file = fopen("numbers.bin", "wb");
  if (!file) ERR_HANDLE;

  srand(time(0));
  int series = 0;
  int current = rand();
  while (series != SERIES_COUNT) {
    int next = rand();
    if (current > next)
      series++;
    //printf("%d ", current);
    fwrite(&current, sizeof(current), 1, file);
    current = next;
  }

  puts("");

  fclose(file);
  return 0;
}
