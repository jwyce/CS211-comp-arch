#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  int data;
  struct node* next;
} node;

// creates a new node initializing data and next field
node* create(int data, node* next) {
  node* new_node = (node*) malloc(sizeof(node));
  if (new_node == NULL) {
    printf("error creating node\n");
  }
  new_node->data = data;
  new_node->next = next;
  return new_node;
}

// inserts a node into sorted position in list (allows no duplicates)
node* insert(node* head, int data) {
  if (head == NULL || head->data >= data) {
    node* new_node = create(data, head);
    head = new_node;
  } else {
    node* ptr = head;
    while (ptr->next != NULL && ptr->next->data < data) {
      ptr = ptr->next;
    }
    if (ptr->next == NULL || ptr->next->data != data) {
      node* new_node = create(data, ptr->next);
      ptr->next = new_node;
    }
  }
  return head;
}

// deletes node with specified data
node* delete(node* head, int data) {
  node* ptr = head;
  node* prev = NULL;
  while (ptr != NULL) {
    if (ptr->data == data) break;
    prev = ptr;
    ptr = ptr->next;
  }
  if (ptr == NULL) return head;
  if (prev == NULL) return head->next;
  prev->next = ptr->next;
  return head;
}

// linearly prints the data of each node in list
void traverse(node* head) {
  node* ptr = head;
  while (ptr != NULL) {
    printf("%d-->", ptr->data);
    ptr = ptr->next;
  }
  printf("NULL\n");
}

// print reverse
void printReverse(node* head, int i) {
  if (head == NULL) return;
  printReverse(head->next, i+1);
  printf("%d-->", head->data);
  if (i == 0) printf("NULL\n");
}

// clears linked list and frees memory
void clear(node* head) {
  node *ptr, *temp;
  if (head != NULL) {
    ptr = head->next;
    head->next = NULL;
    while (ptr != NULL) {
      temp = ptr->next;
      clear(ptr);
      ptr = temp;
    }
  }
}

int main(int argc, char **argv) {
  node* head = NULL;
  head = insert(head, 1);
  head = insert(head, 2);
  head = insert(head, 3);
  head = insert(head, 4);
  head = insert(head, 5);
  traverse(head);
  printReverse(head, 0);
  return 0;
}

// int main(int argc, char **argv) {
//   FILE* fp = fopen(argv[1], "r");
//   node* head = NULL;
//   if (argc > 1 && fp != NULL) {
//     char op;
//     int data;
//     while (!feof(fp)) {
//       if (fscanf(fp, "%c\t%d\n", &op, &data) == 2) {
//       // printf("%c %d\n", op, data);
//         switch (op) {
//         case 'i':
//           head = insert(head, data);
//           break;
//         case 'd':
//           head = delete(head, data);
//           break;
//         default:
//           printf("error\n");
//           return 0;
//         }
//       }
//     }
//   } else {
//     printf("error\n");
//     return 0;
//   }
//
//   traverse(head);
//   clear(head);
//   fclose(fp);
//   return 0;
// }
