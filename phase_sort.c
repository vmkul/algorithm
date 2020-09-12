#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define NUM_COUNT 10
#define SERIES_BUFFER_SIZE 1024

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
    current = next;
  }
  fclose(file);
  return 1;
}

int main() {
  FILE *file;
  FILE *A, *B, *C;

  file = fopen("numbers.bin", "rb");
  if (!file) ERR_HANDLE;

  int current;
  int next;

  int *buffer = (int *) malloc(SERIES_BUFFER_SIZE);
  int element_count = 1;
  int index = 0;

  fread(&current, sizeof(int), 1, file);

  while (fread(&next, sizeof(int), 1, file) > 0) {
    element_count++;
    buffer[index++] = current;

    if (current > next || element_count == NUM_COUNT) {
      puts("Series: ");
      for (int i = 0; i < index; i++) {
        printf("%d ", buffer[i]);
      }
      puts("");

      if (element_count == NUM_COUNT) {
        if (next > current) {
          printf("%d\n", next);
        } else {
          printf("Series: \n%d\n", next);
        }
      }

      index = 0;
    }
    current = next;
  }

  fclose(file);

  int result = check_sorted("numbers.bin");
  puts(result ? "Sorted" : "Not sorted");
  return 0;
}