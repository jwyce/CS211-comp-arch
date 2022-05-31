#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#define SIZE 1000

bool linear_probe(long long hash_table[], int data) {
  int i = data % SIZE + 1;
  while (i != data % SIZE) {
    if (hash_table[i] == LONG_MAX) {
      hash_table[i] = data;
      return true;
    }
    if (hash_table[i] == data) return false;
    i++;
    i %= SIZE;
  }
}

bool insert(long long hash_table[], int data) {
  if (hash_table[data % SIZE] == LONG_MAX) {
    hash_table[data % SIZE] = data;
    return true;
  } else {
    if (hash_table[data % SIZE] == data) {
      return false;
    } else {
      // linearly probe to resolve collision
      return linear_probe(hash_table, data);
    }
  }
}

bool search(long long hash_table[], int data) {
  if (hash_table[data % SIZE] == data) return true;
  else {
    int i = data % SIZE + 1;
    while (i != data % SIZE) {
      if (hash_table[i] == data) return true;
      i++;
      i %= SIZE;
    }
    return false;
  }
}

void print_table(long long hash_table[]) {
  int i;
  for (i = 0; i < SIZE; i++) {
    if (hash_table[i] != LONG_MAX) {
      printf("%d : %lld\n", i, hash_table[i]);
    }
  }
}

int main(int argc, char **argv) {
  FILE* fp = fopen(argv[1], "r");
  long long hash_table[SIZE] = { [0 ... SIZE-1] = LONG_MAX };

  if (argc > 1 && fp != NULL) {
    char op;
    int data;
    while (!feof(fp)) {
      if (fscanf(fp, "%c\t%d\n", &op, &data) == 2) {
        switch (op) {
        case 'i':
          if (insert(hash_table, data)) printf("inserted\n");
          else printf("duplicate\n");
          break;
          case 's':
            if (search(hash_table, data)) printf("present\n");
            else printf("absent\n");
            break;
          default:
            printf("error\n");
        }
      }
    }
  } else {
    printf("error\n");
    return 0;
  }

  print_table(hash_table);
  fclose(fp);
  return 0;
}
