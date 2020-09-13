#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"

#define SERIES_COUNT 165580141
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

struct File {
  char *name;
  FILE *stream;
} typedef File;

void copy_file(FILE *src, FILE* dest) {
  int num;
  while (fread(&num, sizeof(int), 1, src)) {
    fwrite(&num, sizeof(int), 1, dest);
  }
}


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

void update_file(int current, File *file) {
  FILE *tmp = fopen("tmp", "wb");
  int num;
  fwrite(&current, sizeof(int), 1, tmp);
  while (fread(&num, sizeof(int), 1, file->stream)) {
    fwrite(&num, sizeof(int), 1, tmp);
  }
  fclose(file->stream);
  remove(file->name);
  rename("tmp", file->name);
  file->stream = tmp;
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
    read = fread(&current, sizeof(int), 1, src); 
} while (last <= current && read);

  if (!read) return -1;
  return current;
}

void merge_series(File *src1, File *src2, File *dest) {
  int current_first;
  int current_second;

  int first_read = fread(&current_first, sizeof(int), 1, src1->stream);  
  int second_read = fread(&current_second, sizeof(int), 1, src2->stream);

  while (first_read && second_read) {
    int minimal = fmin(current_first, current_second);
    fwrite(&minimal, sizeof(int), 1, dest->stream);

    if (minimal == current_first) {
      first_read = fread(&current_first, sizeof(int), 1, src1->stream);
      if (current_first < minimal || !first_read) {
        current_second = write_series(current_second, 
          src2->stream, dest->stream);

        if (current_second == -1) {
          second_read = 0;
          break;
        }
      }
    } else {
      second_read = fread(&current_second, sizeof(int), 1, src2->stream);
      if (current_second < minimal || !second_read) {
        current_first = write_series(current_first, 
          src1->stream, dest->stream);
         if (current_first == -1) {
          first_read = 0;
          break;
        }
      }
    }
  }
  
  if (!first_read) {
    freopen(src1->name, "wb", src1->stream);
    update_file(current_second, src2);
  } else {
    freopen(src2->name, "wb", src2->stream);
    update_file(current_first, src1);
  }
}

int check_sorted(FILE *file) {
  int current;
  int next;
  int read = fread(&current, sizeof(int), 1, file);
  while (fread(&next, sizeof(int), 1, file) > 0) {
    if (current > next) {
      fclose(file);
      return 0;
    }
    current = next;
  }
  if (!read) return 0;
  return 1;
}

int main() {
  FILE *file;
  File A, B, C;

  file = fopen("numbers.bin", "rb");
  A.stream = fopen("A", "wb");
  B.stream = fopen("B", "wb");
  C.stream = fopen("C", "wb");

  A.name = "A";
  B.name = "B";
  C.name = "C";

  File_series series_in_file;
  
  if (!file) ERR_HANDLE;

  int current;
  int next;

  get_fibs(SERIES_COUNT, &series_in_file.A, &series_in_file.B);

  series_in_file.C = 0;

  fread(&current, sizeof(int), 1, file);

  puts("File A");

  for (int i = 0; i < series_in_file.A;) {
    fwrite(&current, sizeof(current), 1, A.stream);
    fread(&next, sizeof(int), 1, file);

    if (current > next) i++;
    current = next;
  }
  
  puts("");
  puts("File B");

  do {
    fwrite(&current, sizeof(current), 1, B.stream);
  } while (fread(&current, sizeof(int), 1, file));

  puts("");
  puts("File C");

  while ((series_in_file.A || series_in_file.B)
  && (series_in_file.A || series_in_file.C)
  && (series_in_file.B || series_in_file.C)) {
    if (!series_in_file.A) {
      freopen(B.name, "rb", B.stream);
      freopen(C.name, "rb", C.stream);
      freopen(A.name, "wb", A.stream);

      update_series(&series_in_file.B, &series_in_file.C, &series_in_file.A);
      merge_series(&B, &C, &A);
    } else if (!series_in_file.B) {
      freopen(B.name, "wb", B.stream);
      freopen(C.name, "rb", C.stream);
      freopen(A.name, "rb", A.stream);
      
      update_series(&series_in_file.A, &series_in_file.C, &series_in_file.B);
      merge_series(&A, &C, &B);
    } else {
      freopen(B.name, "rb", B.stream);
      freopen(C.name, "wb", C.stream);
      freopen(A.name, "rb", A.stream);

      update_series(&series_in_file.A, &series_in_file.B, &series_in_file.C);
      merge_series(&A, &B, &C);
    }
  }

  freopen("numbers.bin", "wb", file);

  if (series_in_file.A) {
    puts("COPY A");
    freopen("A", "rb", A.stream);
    copy_file(A.stream, file);
  } else if (series_in_file.B) {
    puts("COPY B");
    freopen("B", "rb", B.stream);
    copy_file(B.stream, file);
  } else {
    puts("COPY C");
    freopen("C", "rb", C.stream);
    copy_file(C.stream, file);
  }

  fclose(C.stream);
  fclose(B.stream);
  fclose(A.stream);

  freopen("numbers.bin", "rb", file);
  int result = check_sorted(file);
  puts(result ? "Sorted" : "Not sorted");
  fclose(file);
  return 0;
}