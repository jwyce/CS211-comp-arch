#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#define SIZE 1000
int count;

long long mod(long long a, int b) {
  long long m = a % b;
  if (m < 0) {
    m = (b < 0) ? m - b : m + b;
  }
  return m;
}

typedef struct node {
  long long data;
  struct node* next;
} node;

// creates a new node initializing data and next field
node* create(long long data, node* next) {
  node* new_node = (node*) malloc(sizeof(node));
  if (new_node == NULL) {
    printf("error creating node\n");
  }
  new_node->data = data;
  new_node->next = next;
  return new_node;
}

// inserts a node into list (allows no duplicates)
node* insert(node* head, long long data) {
  node* ptr = head;
  bool duplicate = false;
  if (head == NULL) {
    node* new_node = create(data, head);
    head = new_node;
    count++;
    return head;
  }

  while (ptr->next != NULL) {
    if (ptr->data == data) duplicate = true;
    ptr = ptr->next;
  }

  if (ptr->data != data && !duplicate) {
    node* new_node = create(data, ptr->next);
    ptr->next = new_node;
    count++;
  }

  return head;
}

// linearly prints the data of each node in list
void traverse(node* head) {
  node* ptr = head;
  while (ptr != NULL) {
    printf("%lx\t", ptr->data);
    ptr = ptr->next;
  }
  printf("\n");
}

void put(node** hash_table, long long data) {
  hash_table[mod(data, SIZE)] = insert(hash_table[mod(data, SIZE)], data);
}

void print_table(node** hash_table) {
  int i;
  for (i = 0; i < SIZE; i++) {
    if (hash_table[i] != NULL) {
      printf("%d : ", i);
      traverse(hash_table[i]);
    }
  }
}

int main(int argc, char **argv) {
  FILE* fp = fopen(argv[1], "r");
  node* hash_table[SIZE] = { [0 ... SIZE-1] = NULL };
  count = 0;

  if (argc > 1 && fp != NULL) {
    while (!feof(fp)) {
      long long data;
      if (fscanf(fp, "%lx\n", &data) == 1) {
        // insert into hash_table
        put(hash_table, data);
      }
    }
  } else {
    printf("error\n");
    return 0;
  }

  fclose(fp);
  // print_table(hash_table);
  printf("%d\n", count);
  return 0;
}
