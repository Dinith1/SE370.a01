# SE370.a01
---
# Step 1
- Determine how large an array can be dealt with by this program before you get a segmentation fault (**Question 2**).
- **a1.1.c** -> Increase amount of stack space so that program can handle at least 100,000,000 numbers.
	- `getrlimit` and `setrlimit`.
- Time/record sorting of 100,000,000 numbers - `./a1.1 100000000`.


# Step 2
- **a1.2.c** -> Use two threads to perform sort.
	- `pthread_create` and `pthread_join`.
- Increase stack size (change `pthread` attributes).
	- `pthread_attr_init` and `pthread_attr_setstacksize`.
- Time/record sorting of 100,000,000 numbers - `./a1.2 100000000`.


# Step 3
- **a1.3.c** -> Use new thread every time you call `merge_sort`.
- Use what is found to answer **Question 4**.


# Step 4
- **a1.4.c** -> Use as many threads as there are cores to perform the sort **and no more**.
	- Need machine with **at least 4** cores.
- **MUST** use some shared state to keep track of how many threads there are currently active.
	- Increment counter each time thread starts (if not at maximum), decrement when it stops.
- Provide mutual exclusion over the thread counter.
	- `pthread_mutex_init`, `pthread_mutex_lock` and `pthread_mutex_unlock`.
- Time/record sorting of 100,000,000 numbers - `./a1.4 100000000`.
- Screenshot the `System Monitor` program showing the `Resources` tab as the program runs.
	- Proves that all cores are being used.


# Step 5
- **a1.5.c** -> Use **spin locks** to protect the shared state.
	- `pthread_spin_init`, `pthread_spin_lock` and `pthread_spin_unlock`.
- Time/record sorting of 100,000,000 numbers - `./a1.5 100000000`.
- Screenshot the `System Monitor` showing the `Resources` tab.


# Step 6
- **a1.6.c** -> Go back to **Step 2** and instead use **two processes** rather than **two threads**.
- Need to communicate between processes.
	- `fork` and `pipe`.
	- Pass sorted values from children back to parent.
- Time/record sorting of 100,000,000 numbers - `./a1.6 100000000`.


# Step 7
- **a1.7.c** -> Repeat **Step 6** except use **as many processes as the machine has cores**.
- Time/record sorting of 100,000,000 numbers - `./a1.7 100000000`.
- Screenshot the `System Monitor` showing the `Resources` tab.


# Step 8
- **a1.8.c** -> Repeat **Step 6** (**two processes**) except, instead of passing info back to parents, **share the memory** sorted in all of the processes.
- Time/record sorting of 100,000,000 numbers - `./a1.8 100000000`.
- Screenshot the `System Monitor` showing the `Resources` tab.


# Step 9
- **a1.9.c** -> Repeat **Step 7** (**as many processes as the machine has cores**) except, instead of passing info back to parents, **share the memory** sorted in all of the processes.
- Time/record sorting of 100,000,000 numbers - `./a1.9 100000000`.
- Screenshot the `System Monitor` showing the `Resources` tab.


# Bonus
- **a1.bonus.c** -> Write quickest version of merge sort that you can based on original code.
- In report include a section describing the program with its timing results.
