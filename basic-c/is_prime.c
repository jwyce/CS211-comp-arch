#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool isPrime(int num) {
  if (num < 2) return false;
  if (num == 2) return true;
  if (num % 2 == 0) return false;

  int i;
  for (i = 3; i*i <= num; i += 2) {
    if (num % i == 0) return false;
  }
  return true;
}

int main(int argc, char **argv) {
  if (argc > 1) {
    int num;
    sscanf(argv[1], "%d", &num);
    if (isPrime(num)) {
      printf("yes\n");
    } else {
      printf("no\n");
    }
  } else {
    printf("error\n");
  }
  return 0;
}
