/*
    The Merge Sort to use for Operating Systems Assignment 1 2019
    written by Robert Sheehan

    Modified by: Dinith Wannigama
    UPI: dwan609

    By submitting a program you are claiming that you and only you have made
    adjustments and additions to this code.
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
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
void *merge_sort(void *my_data) {
  // print_block_data(my_data);

  struct block *my_data_cast = (struct block *)my_data;

  if (my_data_cast->size > 1) {
    struct block left_block;
    struct block right_block;

    left_block.size = my_data_cast->size / 2;
    left_block.first = my_data_cast->first;

    right_block.size = left_block.size + (my_data_cast->size % 2);
    right_block.first = my_data_cast->first + left_block.size;

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

/* Increase the stack size */
void increaseStackSize() {
  const rlim_t desiredStackSize = 512 * 1024 * 1024;  // 500MB
  struct rlimit rl;

  int result = getrlimit(RLIMIT_STACK, &rl);

  if (result != 0) {
    fprintf(stderr, "%s", "Failed to get the current stack size\n");
    exit(EXIT_FAILURE);
  }

  printf("Old stack size = %ldMB\n", rl.rlim_cur / 1000000);

  rl.rlim_cur = desiredStackSize;
  result = setrlimit(RLIMIT_STACK, &rl);

  if (result != 0) {
    fprintf(stderr, "%s", "Failed to increase the stack size\n");
    exit(EXIT_FAILURE);
  }

  printf("New stack size = %ldMB\n", rl.rlim_cur / 1000000);
}

/* Call merge_sort() on the specified input on a new thread */
pthread_t merge_sort_new_thread(struct block *data) {
  // Create/set attributes of the new thread that will be created
  pthread_attr_t thread_attr;

  if (pthread_attr_init(&thread_attr) != 0) {
    fprintf(stderr, "ERROR: Failed to initialize new thread attributes\n");
    exit(EXIT_FAILURE);
  }

  // Set the stack size of the new thread
  size_t thread_stacksize = 512 * 1024 * 1024;  // 500MB

  if (pthread_attr_setstacksize(&thread_attr, thread_stacksize)) {
    fprintf(stderr, "ERROR: Failed to increase stack size of new tread\n");
    exit(EXIT_FAILURE);
  }

  pthread_t thread;

  // Create a new thread and perform merge_sort of left_block on it
  if (pthread_create(&thread, &thread_attr, merge_sort, &data)) {
    fprintf(stderr, "ERROR: Failed to create new thread\n");
    exit(EXIT_FAILURE);
  }

  return thread;
}

int main(int argc, char *argv[]) {
  // Set the stack size of the original thread first
  increaseStackSize();

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

  // Split data into two blocks for processing on two threads
  struct block left_block;
  struct block right_block;

  left_block.size = start_block.size / 2;
  left_block.first = start_block.first;

  right_block.size = left_block.size + (start_block.size % 2);
  right_block.first = start_block.first + left_block.size;

  printf("starting---\n");

  pthread_t thread2 = merge_sort_new_thread(&left_block);

  // Perform merge_sort of the right_block on the original thread
  merge_sort(&right_block);

  // Wait for the second thread to finish
  if (pthread_join(thread2, NULL)) {
    fprintf(stderr, "ERROR: Failed to join thread\n");
    exit(EXIT_FAILURE);
  }

  // Finally merge sorted blocks from the two threads
  merge(&left_block, &right_block);

  printf("---ending\n");

  printf(is_sorted(data, size) ? "sorted\n" : "not sorted\n");
  exit(EXIT_SUCCESS);
}