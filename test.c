#include "stdio.h"
#include "stdlib.h"

int main() {
  malloc((size_t) 1024 * 1024 * 1024);
  getchar();
  return 0;
}