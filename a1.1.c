/*
    The Merge Sort to use for Operating Systems Assignment 1 2019
    written by Robert Sheehan

    Modified by: Dinith Wannigama
    UPI: dwan609

    By submitting a program you are claiming that you and only you have made
    adjustments and additions to this code.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

#define SIZE 2

struct block {
  int size;
  int *first;
};

// void print_block_data(struct block *blk) {
//     printf("size: %d address: %p\n", blk->size, blk->first);
// }

/* Combine the two halves back together. */
void merge(struct block *left, struct block *right) {
  int combined[left->size + right->size];
  int dest = 0, l = 0, r = 0;

  while (l < left->size && r < right->size) {
    if (left->first[l] < right->first[r])
      combined[dest++] = left->first[l++];
    else
      combined[dest++] = right->first[r++];
  }

  while (l < left->size) combined[dest++] = left->first[l++];
  while (r < right->size) combined[dest++] = right->first[r++];

  memmove(left->first, combined, (left->size + right->size) * sizeof(int));
}

/* Merge sort the data. */
void merge_sort(struct block *my_data) {
  // print_block_data(my_data);
  if (my_data->size > 1) {
    struct block left_block;
    struct block right_block;

    left_block.size = my_data->size / 2;
    left_block.first = my_data->first;

    right_block.size = left_block.size + (my_data->size % 2);
    right_block.first = my_data->first + left_block.size;

    merge_sort(&left_block);
    merge_sort(&right_block);

    merge(&left_block, &right_block);
  }
}

/* Check to see if the data is sorted. */
bool is_sorted(int data[], int size) {
  bool sorted = true;

  for (int i = 0; i < size - 1; i++) {
    if (data[i] > data[i + 1]) sorted = false;
  }

  return sorted;
}

int main(int argc, char *argv[]) {
  long size;

  if (argc < 2) {
    size = SIZE;
  } else {
    size = atol(argv[1]);
  }

  struct block start_block;
  int data[size];

  start_block.size = size;
  start_block.first = data;

  for (int i = 0; i < size; i++) {
    data[i] = rand();
  }

  const rlim_t desiredStackSize = 64L * 1024L * 1024L;  // min stack size = 64MB
  struct rlimit rl;
  /*
  struct rlimit {
    rlim_t rlim_curr; // Soft limit
    rlim_t rlim_max; // hard limit (celing for rlim_cur)
  }
   */

  int result = getrlimit(RLIMIT_STACK, &rl);
  printf("rlimit = %ld\n", rl.rlim_cur);

  rl.rlim_max = desiredStackSize;
  result = setrlimit(RLIMIT_STACK, &rl);

  if (!result) {
    printf("Updated stack size\n");
    result = getrlimit(RLIMIT_STACK, &rl);
    printf("rlimit = %ld\n", rl.rlim_cur);
  }

  printf("starting---\n");
  clock_t begin = clock();  // Start timing

  merge_sort(&start_block);

  clock_t end = clock();  // End timing
  double time = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("---ending\n");

  printf("Time taken: %.9f\n", time);

  printf(is_sorted(data, size) ? "sorted\n" : "not sorted\n");
  exit(EXIT_SUCCESS);
}