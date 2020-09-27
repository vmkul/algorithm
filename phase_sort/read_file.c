#include "stdio.h"
#include "stdlib.h"

int main(int argc, char *argv[]) {
  FILE *f = fopen(argv[1], "rb");

  int num;
  while (fread(&num, sizeof(int), 1, f)) {
    printf("%d ", num);
  }

  puts("");

  return 0;
}