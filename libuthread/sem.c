#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"

struct semaphore 
{
  int value;
  queue_t List; // list of thread IDs
};


sem_t sem_create(size_t count)
{
  sem_t sem  = malloc(sizeof(sem_t));
  sem->value = count;
  sem->List  = queue_create();

  //initializing the semaphore
  if (sem)
  {
    //If Allocated Return pointer to semaphore
    return sem;
  }

  return NULL; //when initializing fails
}

int sem_destroy(sem_t sem)
{
  //Error_Checking
  if(sem == NULL){
    return(-1);
  }//semaphore is NULL 
  else if(queue_length(sem->List)){
    return(-1);
  }//Or threads are still being blocked
    
  
  queue_destroy(sem->List);
  free(sem);
  return 0;
}

int sem_down(sem_t sem)
{
	enter_critical_section();
  	//Error_Checking(UnAllocated Object)
  	if(sem == NULL)
    		return(-1);
  	else if(sem->value > 0){
    		sem->value -= 1;
    		exit_critical_section();
	}
	
  	return 0;
	
}
int sem_up(sem_t sem)
{
	
	if(queue_length(sem->List)){
		enter_critical_section();
    	}//QUEUE_NOT_EMPTY
  	else{
    		sem->value += 1;
		exit_critical_section();
  	}//increment
    		
	
  return 0;
}


