#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define NUMBER_COUNT 26214400

int main() {
  FILE *file = fopen("numbers.bin", "wb");

  srand(time(0));

  for (int i = 0; i < NUMBER_COUNT; i++) {
    int number = rand();
    fwrite(&number, sizeof(number), 1, file);
  }

  fclose(file);
  return 0;
}
