# project3 for ECS150 : User-level thread library (part2)

## Project name: libuthread.a

## Our project contains the following files:
* queue.c
* sem.c
* tps.c
* Makefile(s)
* AUTHORS
* REPORT.md

**Design choices:**
* I decided to use the queue implementation from phase 2 
* Queue was modified to run get_length in O(1) time via a size variable
* TPS I implemented a static Page Table of size 1024
* Page object contains adress of memory, valid bit, other variables not yet implemented.
* I decided to include several helper functions some of which do similar operations
* Resize could be implemented with capacity variable
* When capacity exceeds thershold open up more array indiced and update Page_SIZE(dynamic)
* Amortized Big-O(n) should also beat queue's iterate
* Free's indecied (random) actually performs well

**Implementation:**
* Since the client app dosen't have access to pointer objects I mangage it myself
* I add a static Page Table to keep track of thread_IDs
* I index the pages based on their thread_IDs
* ^ requires me to make by Table Large enough to avoid collisions

**Page Struct -> address -> maps to anonymous memory pointer**
* Simple Page Table with dynamically resized pages

**Key information**
* Semaphores implementation seemed simple but currently hangs

**Biggest challenges:**
* Design Descisions are ambigous 
