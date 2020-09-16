#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "quicksort.h"

#define SERIES_BUFFER_SIZE 1073741824LLU
#define NUMBER_COUNT 26214400

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
    if (next == n) {
      *first = current;
      *second = next - current;
      break;
    }
    int tmp = next;
    next += current;
    current = tmp;
  }
}

int get_closest_fib(int n) {
  int current = 1;
  int next = 1;

  for (;;) {
    if (next >= n) {
      return next;
      break;
    }
    int tmp = next;
    next += current;
    current = tmp;
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

int buffer_sort(File *f) {
  FILE *tmp = fopen("tmp", "wb");
  int *buffer = (int *) malloc(SERIES_BUFFER_SIZE);
  int series_count = 0;
  int read;
  do {
    read = fread(buffer, SERIES_BUFFER_SIZE, 1, f->stream);
    series_count++;
    quickSort(buffer, 0, NUMBER_COUNT - 1);
    fwrite(buffer, NUMBER_COUNT * 4, 1, tmp);
  } while (read);

  fclose(f->stream);
  remove(f->name);

  rename("tmp", f->name);
  f->stream = tmp;
  free(buffer);

  return series_count;
}

int main() {
  File file;
  File A, B, C;

  file.stream = fopen("numbers.bin", "rb");
  A.stream = fopen("A", "wb");
  B.stream = fopen("B", "wb");
  C.stream = fopen("C", "wb");

  file.name = "numbers.bin";
  A.name = "A";
  B.name = "B";
  C.name = "C";

  File_series series_in_file;
  
  if (!file.stream) ERR_HANDLE;

  int current;
  int next;

  clock_t start, end;
  double cpu_time_used;
  start = clock();

  puts("Internal sorting...\n");

  int series_count = buffer_sort(&file);
  int resulting_series_count = get_closest_fib(series_count);

  if (resulting_series_count == 1) resulting_series_count++;
  
  int empty_series = resulting_series_count - series_count;
  freopen(file.name, "ab", file.stream);
  int empty = -2;

  for (int i = 0; i < empty_series; i++) {
    fwrite(&empty, sizeof(int), 1, file.stream);
    empty--;
  }
  
  freopen(file.name, "rb", file.stream);
  get_fibs(resulting_series_count, &series_in_file.A, &series_in_file.B);

  series_in_file.C = 0;

  fread(&current, sizeof(int), 1, file.stream);

  puts("Generating file A\n");

  for (int i = 0; i < series_in_file.A;) {
    fwrite(&current, sizeof(current), 1, A.stream);
    fread(&next, sizeof(int), 1, file.stream);

    if (current > next) i++;
    current = next;
  }
  
  puts("Generating file B\n");

  do {
    fwrite(&current, sizeof(current), 1, B.stream);
  } while (fread(&current, sizeof(int), 1, file.stream));

  puts("Sorting...\n");

  int merge_count = 0;
  while ((series_in_file.A || series_in_file.B)
  && (series_in_file.A || series_in_file.C)
  && (series_in_file.B || series_in_file.C)) {
    merge_count++;
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

  printf("Merge count: %d\n", merge_count);

  freopen("numbers.bin", "wb", file.stream);

  if (series_in_file.A) {
    puts("COPY A");
    freopen("A", "rb", A.stream);
    copy_file(A.stream, file.stream);
  } else if (series_in_file.B) {
    puts("COPY B");
    freopen("B", "rb", B.stream);
    copy_file(B.stream, file.stream);
  } else {
    puts("COPY C");
    freopen("C", "rb", C.stream);
    copy_file(C.stream, file.stream);
  }

  fclose(C.stream);
  fclose(B.stream);
  fclose(A.stream);

  remove(A.name);
  remove(B.name);
  remove(C.name);

  freopen("numbers.bin", "rb", file.stream);
  int result = check_sorted(file.stream);
  puts(result ? "Sorted" : "Not sorted");
  fclose(file.stream);
  
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("Sort completed in: %f seconds\n", cpu_time_used);
  return 0;
}