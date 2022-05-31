#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc > 1) {
    int num;
    sscanf(argv[1], "%d", &num);
    if (num % 2 == 0) {
      printf("even\n");
    } else {
      printf("odd\n");
    }
  } else {
    printf("error\n");
  }
  return 0;
}
