#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"



struct semaphore
{
	int value;
	queue L; // list of processes
};



sem_t sem_create(size_t count)
{
	sem_t count = malloc(sizeof(*count));
	count -> size = 0;

	//initializing the semaphore
	if (count)
	{
		return *count;
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


