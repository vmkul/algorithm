#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"

#define SERIES_COUNT 5
#define SERIES_BUFFER_SIZE 1024

#define ERR_HANDLE { \
  fprintf(stderr, "Line %d: ", __LINE__); \
  perror(""); \
  exit(1); }

struct File_series {
  int A;
  int B;
  int C;
} typedef File_series;

void get_fibs(int n, int *first, int *second) {
  int current = 1;
  int next = 1;

  for (;;) {
    int tmp = next;
    next += current;
    current = tmp;
    if (next == n) {
      *first = current;
      *second = next - current;
      break;
    }
  }
}

void update_series(int *src1, int *src2, int* dest) {
  int min = fmin(*src1, *src2);
  *dest = min;
  if (min == *src1) {
    *src2 -= *src1;
    *src1 = 0;
  } else {
    *src1 -= *src2;
    *src2 = 0;
  }
}

int write_series(int current, FILE *src, FILE *dest) {
  int last;
  int read;
  do {
    last = current;
    fwrite(&current, sizeof(int), 1, dest);
    printf("%d ", current);
    read = fread(&current, sizeof(int), 1, src); 
} while (last <= current);
  return current;
}

void merge_series(FILE *src1, FILE *src2, FILE *dest) {
  int current_first;
  int current_second;

  int first_read = fread(&current_first, sizeof(int), 1, src1);  
  int second_read = fread(&current_second, sizeof(int), 1, src2);

  while (first_read && second_read) {
    int minimal = fmin(current_first, current_second);
    fwrite(&minimal, sizeof(int), 1, dest);
    printf("%d ", minimal);

    if (minimal == current_first) {
      first_read = fread(&current_first, sizeof(int), 1, src1);
      if (current_first < minimal) {
        current_second = write_series(current_second, src2, dest);
      }
    } else {
      second_read = fread(&current_second, sizeof(int), 1, src2);
      if (current_second < minimal) {
        current_first = write_series(current_first, src1, dest);
      }
    }
  }

  int last_num;
  if (!first_read) {
    last_num = write_series(current_second, src2, dest);
  } else {
    last_num = write_series(current_first, src1, dest);
  }
  puts("");
  printf("%d", last_num);
}

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
  A = fopen("A", "wb");
  B = fopen("B", "wb");
  C = fopen("C", "wb");

  File_series series_in_file;
  
  if (!file) ERR_HANDLE;

  int current;
  int next;

  get_fibs(SERIES_COUNT, &series_in_file.A, &series_in_file.B);

  series_in_file.C = 0;

  fread(&current, sizeof(int), 1, file);

  puts("File A");

  for (int i = 0; i < series_in_file.A;) {
    fwrite(&current, sizeof(current), 1, A);
    printf("%d ", current);
    fread(&next, sizeof(int), 1, file);

    if (current > next) i++;
    current = next;
  }
  
  puts("");
  puts("File B");

  do {
    fwrite(&current, sizeof(current), 1, B);
    printf("%d ", current);
  } while (fread(&current, sizeof(int), 1, file));

  puts("");

  freopen("A", "rb", A);
  freopen("B", "rb", B);

  puts("File C");

  while ((series_in_file.A || series_in_file.B)
  && (series_in_file.A || series_in_file.C)
  && (series_in_file.B || series_in_file.C)) {
    if (!series_in_file.A) {
      update_series(&series_in_file.B, &series_in_file.C, &series_in_file.A);
      merge_series(B, C, A);
    } else if (!series_in_file.B) {
      update_series(&series_in_file.A, &series_in_file.C, &series_in_file.B);
      merge_series(A, C, B);
    } else {
      update_series(&series_in_file.A, &series_in_file.B, &series_in_file.C);
      merge_series(A, B, C);
    }
    break;
  }

  fclose(file);
  fclose(C);
  fclose(B);
  fclose(A);
  int result = check_sorted("numbers.bin");
  puts(result ? "Sorted" : "Not sorted");
  return 0;
}