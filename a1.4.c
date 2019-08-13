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
#include <sys/sysinfo.h>
#include <unistd.h>

#define SIZE 2

struct block {
  int size;
  int *first;
};

pthread_mutex_t mutex;
int numActiveThreads;
int MAX_NUM_CORES;

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

    pthread_t thread_left;

    pthread_mutex_lock(&mutex);

    if (numActiveThreads < MAX_NUM_CORES) {
      pthread_mutex_unlock(&mutex);

      // Call merge_sort() on a new thread as well

      pthread_mutex_lock(&mutex);
      numActiveThreads++;
      pthread_mutex_unlock(&mutex);

      // Create/set attributes of the 'left' thread that will be created
      pthread_attr_t thread_left_attr;

      if (pthread_attr_init(&thread_left_attr) != 0) {
        fprintf(stderr, "ERROR: Failed to initialize new left thread attributes\n");
        exit(EXIT_FAILURE);
      }

      // Set the stack size of the left thread
      size_t thread_left_stacksize = 1024 * 1024 * 1024;  // 1000MB

      if (pthread_attr_setstacksize(&thread_left_attr, thread_left_stacksize) != 0) {
        fprintf(stderr, "ERROR: Failed to increase stack size of left tread\n");
        exit(EXIT_FAILURE);
      }

      // Create the left thread and perform merge_sort of left_block on it
      if (pthread_create(&thread_left, &thread_left_attr, merge_sort, &left_block) != 0) {
        fprintf(stderr, "ERROR: Failed to create left thread\n");
        exit(EXIT_FAILURE);
      }

      // merge_sort the right_block on the current thread
      merge_sort(&right_block);

      // Wait for the left thread to finish
      if (pthread_join(thread_left, NULL) != 0) {
        fprintf(stderr, "ERROR: Failed to join left thread\n");
        exit(EXIT_FAILURE);
      }

      pthread_mutex_lock(&mutex);
      numActiveThreads--;
      pthread_mutex_unlock(&mutex);

    } else {
      pthread_mutex_unlock(&mutex);

      // Call merge_sort() on the current thread only
      merge_sort(&left_block);
      merge_sort(&right_block);
    }

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
  const rlim_t desiredStackSize = 1024 * 1024 * 1024;  // 500MB
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

/* Find the max number of cores available to use */
bool check4Cores() {
  // Return true if there's at least 4 available, false otherwise
  return (MAX_NUM_CORES = get_nprocs()) >= 4;
}

int main(int argc, char *argv[]) {
  // Set the stack size of the original thread first
  increaseStackSize();

  // Check if there's at least 4 cores on the machine
  if (!check4Cores()) {
    fprintf(stderr, "ERROR: Need at least 4 cores (you have %d)\n", MAX_NUM_CORES);
    exit(EXIT_FAILURE);
  }

  printf("A maximum of %d cores will be used\n", MAX_NUM_CORES);

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

  // Initialize the mutex
  if (pthread_mutex_init(&mutex, NULL) != 0) {
    fprintf(stderr, "ERROR: Failed to initialize the mutex\n");
    exit(EXIT_FAILURE);
  }

  numActiveThreads = 1;

  printf("starting---\n");
  merge_sort(&start_block);
  printf("---ending\n");

  printf(is_sorted(data, size) ? "sorted\n" : "not sorted\n");
  exit(EXIT_SUCCESS);
}