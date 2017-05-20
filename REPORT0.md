# General Information 
## Class: ECS150  
## Instructor: Joel Porquet 
## Project Name: libuthread.a

## Introduction: The goal of this project is to extend our understanding of
threads, by implementing two independent parts:

1.Semaphores, for efficient thread synchronization by using waiting queues. 2
.Per-thread protected memory regions. Threads will be able to create their own
private storage (a.k.a. TPS or Thread Private Storage), transparently accessible
by an API and protected from other threads.

	
Our project contains the following files: * queue.c * sem.c * tps.c *
Makefile(s) * AUTHORS * REPORT.md

**Design choices:** * I decided to use the queue implementation from phase 2  *
Queue was modified to run get_length in O(1) time via a size variable * TPS I
implemented a static Page Table of size 1024 * Page object contains adress of
memory, valid bit, other variables not yet implemented. * I decided to include
several helper functions some of which do similar operations

**Implementation:** * Since the client app dosen't have access to pointer
objects we mangage it ourselves. * We add a static Page Table to keep track of
thread_IDs. * We index the pages based on their thread_IDs. * ^ requires us to
make by Table Large enough to avoid collisions.

**Page Struct -> address -> maps to anonymous memory pointer** * Simple Page
Table with dynamically resized pages

**Key information** * Semaphores implementation seemed simple but currently
hangs

**Biggest challenges:** * Design Descisions are ambigou

Phase I:  Building upon project II, we used the same queue. We create a
semaphore struct that holds a count, the current thread's ID, and a list that
holds the running threads ID's. In *sem_create()*, we allocate space for the
semaphore, we initialize it, and return a pointer to the allocated semaphore. In
*sem_destroy()*, we check if threads are bieng blocked and if not, then we
dostroy and free sem. As for *sem_down()*, we first enter the critical section,
we check weather sem was properly allocated, we enqueue the current thread and
call *thread_block()*, then we exit the critical section. Lastly in *sem_up()*, 
we first enter the critical section then we check for the proper allocation
of sem. Now, we unblock the blocked thread, we dequeue the thread ID, increment
the thread count, and we exit the critical section.

Phase II:
