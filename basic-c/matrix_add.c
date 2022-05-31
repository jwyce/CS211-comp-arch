#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  FILE* fp = fopen(argv[1], "r");
  int rows, cols;
  fscanf(fp, "%d\t%d\n", &rows, &cols);
  int matrix[rows][cols];

  // init
  int i, j, k;
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      matrix[i][j] = 0;
    }
  }

  // add
  for (k = 0; k < 2; k++) {
    for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++) {
        int num;
        if (fscanf(fp, "%d", &num) != 1) break;
        matrix[i][j] += num;
      }
    }
  }

  // print
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }
  fclose(fp);
  return 0;
}
