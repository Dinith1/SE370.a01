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
#define STACK_SIZE 1000 * 1024 * 1024;  // 1000MB

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
  const rlim_t desiredStackSize = STACK_SIZE;
  struct rlimit rl;

  if (getrlimit(RLIMIT_STACK, &rl) != 0) {
    fprintf(stderr, "%s", "Failed to get the current stack size\n");
    exit(EXIT_FAILURE);
  }

  printf("Old stack size = %ldMB\n", rl.rlim_cur / 1000000);

  rl.rlim_cur = desiredStackSize;

  if (setrlimit(RLIMIT_STACK, &rl) != 0) {
    fprintf(stderr, "%s", "Failed to increase the stack size\n");
    exit(EXIT_FAILURE);
  }

  printf("New stack size = %ldMB\n", rl.rlim_cur / 1000000);
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

  int fd[2];

  // Initialize the pipe
  if (pipe(fd) != 0) {
    fprintf(stderr, "Failed to create the pipe\n");
    exit(EXIT_FAILURE);
  }

  pid_t pid;

  if ((pid = fork()) < 0) {
    fprintf(stderr, "Failed to create the second process\n");
    exit(EXIT_FAILURE);

  } else if (pid > 0) {
    /* Parent process */

    // Close pipe writer
    close(fd[1]);

    merge_sort(&left_block);

    // Read the data in the pipe from the child process
    read(fd[0], right_block.first, right_block.size * sizeof(int));

    // Close the pipe reader so only reading allowed
    close(fd[0]);

    // Finally merge sorted blocks from the two threads
    merge(&left_block, &right_block);

    printf("---ending\n");

    printf(is_sorted(data, size) ? "sorted\n" : "not sorted\n");
    exit(EXIT_SUCCESS);
  } else {
    /* Child process */

    // Close pipe reader so only writing allowed
    close(fd[0]);

    merge_sort(&right_block);

    // Write the sorted block to the pipe
    write(fd[1], right_block.first, right_block.size * sizeof(int));

    // Close pipe writer
    close(fd[1]);
  }
}
