#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum cache_type {DM, FA, SA};
enum cache_policy {FIFO, LRU, NRU};
bool VIEW = true;

FILE* fp;
int cache_size;
int block_size;
int num_sets;
int n_way;
int entries;
enum cache_type type;
enum cache_policy policy;

int mem_reads;
int mem_writes;
int cache_hits;
int cache_misses;

typedef struct node {
  unsigned long tag;
  int r_policy;
  struct node* next;
} node;

node* create(unsigned long tag, int rp, node* next) {
  node* new_node = (node*) malloc(sizeof(node));
  if (new_node == NULL) {
    printf("error creating node\n");
  }
  new_node->tag = tag;
  new_node->r_policy = rp;
  new_node->next = next;
  return new_node;
}

node* addFront(node* head, unsigned long tag, int rp) {
  head = create(tag, rp, head);
  return head;
}

node* addLast(node* head, unsigned long tag, int rp) {
  node* ptr = head;
  if (head == NULL) {
    head = create(tag, rp, head);
  } else {
    while (ptr->next != NULL) {
      ptr = ptr->next;
    }
    ptr->next = create(tag, rp, NULL);
  }
  return head;
}

node* deleteLast(node* head) {
  node* ptr = head;
  node* prev = NULL;
  while (ptr->next != NULL) {
    prev = ptr;
    ptr = ptr->next;
  }
  free(prev->next);
  prev->next = NULL;
  return head;
}

node* delete(node* head, unsigned long tag) {
  node* ptr = head;
  node* prev = NULL;
  while (ptr != NULL) {
    if (ptr->tag == tag) break;
    prev = ptr;
    ptr = ptr->next;
  }
  if (ptr == NULL) return head;
  if (prev == NULL) return head->next;
  prev->next = ptr->next;
  return head;
}

bool inputFormat(int numArgs, char **args) {
  if (numArgs != 6) {
    printf("incorrect number of input args\n");
    return false;
  }

  cache_size = atoi(args[1]);
  if (cache_size == 0 || (cache_size & (cache_size-1)) != 0) {
    printf("cache size not a power of 2\n");
    return false;
  }

  if (strcmp(args[3], "fifo") == 0) {
    policy = FIFO;
  } else if (strcmp(args[3], "nru") == 0) {
    policy = NRU;
  } else if (strcmp(args[3], "lru") == 0) {
    policy = LRU;
  } else {
    printf("invalid replacement policy\n");
    return false;
  }

  block_size = atoi(args[4]);
  if (block_size == 0 || (block_size & (block_size-1)) != 0) {
    printf("block size not a power of 2\n");
    return false;
  }

  n_way = 1;
  num_sets = cache_size / block_size;
  char substr[7];
  strcpy(substr, args[2]);
  substr[6] = '\0';
  if (strcmp(args[2], "direct") == 0 ) {
    type = DM;
  } else if (strcmp(args[2], "assoc") == 0) {
    type = FA;
  } else if (strcmp(substr, "assoc:") == 0) {
    char num_ways[4];
    int i;
    for (i = 6; i < strlen(args[2]); i++) {
      num_ways[i-6] = args[2][i];
    }

    type = SA;
    n_way = atoi(num_ways);
    if (n_way == 0 || (n_way & (n_way-1)) != 0) {
      printf("number of ways not a power of 2\n");
      return false;
    } else {
      num_sets = cache_size / block_size / n_way;
    }
  } else {
    printf("invalid cache type\n");
    return false;
  }

  fp = fopen(args[5], "r");
  if (fp == NULL) {
    printf("invalid filename\n");
    return false;
  }

  return true;
}

int logb2(int x) {
  int n = 1, i = 0;
  if (x < 1) return -1;

  while (x != n) {
    n *= 2;
    i++;
  }

  return i;
}

void direct_mapped_access(unsigned long *cache, unsigned long address, char op) {
  address = address >> logb2(block_size);
  int s_bits = logb2(num_sets);
  int mask = num_sets-1;
  unsigned long index = address & mask;
  unsigned long tag = address >> s_bits;

  if (cache[index] == tag) {
    cache_hits++;
    if (op == 'W') mem_writes++;
  } else {
    cache_misses++;
    mem_reads++;
    cache[index] = tag;
    if (op == 'W') mem_writes++;
  }
}

void replace_FIFO(node **cache, int *size, unsigned long tag, int set) {
  unsigned long low_tag;
  unsigned int low = -1, high = 0;
  int n_elements = n_way;
  if (num_sets == 1) n_elements = entries;
  node *ptr = cache[set];
  while (ptr != NULL) {
    if (low > ptr->r_policy) {
      low = ptr->r_policy;
      low_tag = ptr->tag;
    }
    if (high < ptr->r_policy) high = ptr->r_policy;
    ptr = ptr->next;
  }

  if (size[set] < n_elements) {
    cache[set] = addFront(cache[set], tag, high+1);
    size[set]++;
  } else {
    cache[set] = delete(cache[set], low_tag);
    cache[set] = addFront(cache[set], tag, 0);
    ptr = cache[set];
    while (ptr != NULL) {
      ptr->r_policy--;
      if (ptr->tag == tag) ptr->r_policy = n_elements;
      ptr = ptr->next;
    }
  }
}

void update_LRU(node **cache, int *size, int set, unsigned long tag) {
  cache[set] = delete(cache[set], tag);
  cache[set] = addFront(cache[set], tag, 0);
}

void replace_LRU(node **cache, int *size, int set, unsigned long tag) {
  int n_elements = n_way;
  if (num_sets == 1) n_elements = entries;
  if (size[set] < n_elements) {
    cache[set] = addFront(cache[set], tag, 0);
    size[set]++;
  } else {
    cache[set] = deleteLast(cache[set]);
    cache[set] = addFront(cache[set], tag, 0);
  }
}

void replace_NRU(node **cache, int *size, int set, unsigned long tag) {
  int n_elements = n_way;
  if (num_sets == 1) n_elements = entries;
  if (size[set] < n_elements) {
    cache[set] = addLast(cache[set], tag, 0);
    size[set]++;
  } else {
    node *ptr = cache[set];
    bool found = false;
    while (ptr != NULL) {
      if (ptr->r_policy == 1) {
        found = true;
        break;
      }
      ptr = ptr->next;
    }

    if (found) {
      ptr->tag = tag;
      ptr->r_policy = 0;
    } else {
      node *ptr = cache[set];
      while (ptr != NULL) {
        ptr->r_policy = 1;
        ptr = ptr->next;
      }
      replace_NRU(cache, size, set, tag);
    }
  }
}

void assoc_access(node **cache, int *size, unsigned long address, char op) {
  address = address >> logb2(block_size);
  int s_bits = logb2(num_sets);
  int mask = num_sets-1;
  unsigned long index = address & mask;
  unsigned long tag = address >> s_bits;
  if (num_sets == 1) {
    tag = address;
    index = 0;
  }

  bool found = false;
  node *ptr = cache[index];
  while (ptr != NULL) {
    if (ptr->tag == tag) {
      found = true;
      break;
    }
    ptr = ptr->next;
  }

  if (found) {
    cache_hits++;
    if (policy == LRU) update_LRU(cache, size, index, tag);
    if (policy == NRU) ptr->r_policy = 0;
    if (op == 'W') mem_writes++;
  } else {
    cache_misses++;
    mem_reads++;
    if (op == 'W') mem_writes++;

    switch (policy) {
    case FIFO:
      replace_FIFO(cache, size, tag, index);
      break;
    case LRU:
      replace_LRU(cache, size, index, tag);
      break;
    case NRU:
      replace_NRU(cache, size, index, tag);
      break;
    }
  }
}

void view_d_cache(unsigned long *cache) {
  printf("CACHE VIEW:\n");
  int i;
  for (i = 0; i < entries; i++) {
    printf("line: %2d\ttag: %lx\n", i, cache[i]);
  }
}

void view_a_cache(node **cache) {
  printf("CACHE VIEW:\n");
  int i;
  for (i = 0; i < num_sets; i++) {
    printf("Set %d:\n", i);
    node *ptr = cache[i];
    int j = 0;
    while (ptr != NULL) {
      printf("\tline: %2d\ttag: %8lx\tr_policy: %d\n", j, ptr->tag, ptr->r_policy);
      j++;
      ptr = ptr->next;
    }
  }
}

int main(int argc, char **argv) {
  if (inputFormat(argc, argv)) {
    entries = num_sets*n_way;
    unsigned long *d_cache = (unsigned long*)malloc(entries*sizeof(unsigned long));
    int i;
    for (i = 0; i < entries; i++) {
      d_cache[i] = 0;    // tag
    }

    mem_reads = 0;
    mem_writes = 0;
    cache_hits = 0;
    cache_misses = 0;

    if (n_way == 1 && type != DM) num_sets = 1;
    node **a_cache = (node**)malloc(num_sets*sizeof(node*));
    int *size = (int*)malloc(num_sets*sizeof(int));
    for (i = 0; i < num_sets; i++) {
      a_cache[i] = NULL;
      size[i] = 0;
    }

    char op;
    unsigned long address;
    while (fscanf(fp, "%*x: %c %lx\n", &op, &address) == 2) {
      switch (type) {
        case DM:
          direct_mapped_access(d_cache, address, op);
          break;
        case SA:
        case FA:
          assoc_access(a_cache, size, address, op);
          break;
      }
    }

    if (VIEW) {
      if (type == DM) view_d_cache(d_cache);
      else view_a_cache(a_cache);
    }
  }

  printf("Memory reads: %d\n", mem_reads);
  printf("Memory writes: %d\n", mem_writes);
  printf("Cache hits: %d\n", cache_hits);
  printf("Cache misses: %d\n", cache_misses);
  fclose(fp);
  return 0;
}
