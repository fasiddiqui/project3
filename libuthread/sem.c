#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"


struct semaphore
{
	int value;
	queue List; // list of processes
};



sem_t sem_create(size_t count)
{
	sem_t sem = malloc(sizeof(*sem));
	sem->List = malloc(sizeof(queue_t); 
	sem->value = count;


	//initializing the semaphore
	if (sem)
	{
		//returning a pointer if succeeding
		// returns null otherwise 
		return sem;
	}	

	return NULL; //when initializing fails
}

int sem_destroy(sem_t sem)
{
	return 0;
}

int sem_down(sem_t sem)
{
	return 0;
}

int sem_up(sem_t sem)
{
	return 0;
}


