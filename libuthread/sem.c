#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "thread.h"
#include "sem.h"

struct semaphore 
{
  int value;
  pthread_t tid;
  queue_t List; // list of thread IDs
};

// GLOBAL VARIABLES //

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
  
  if(sem->value > 0)
    sem->value -= 1;
  else{
    tid = pthread_self();   
    queue_enqueue(sem->List, tid);  
    thread_block();
  }
  
  exit_critical_section();
  
  return 0;
}

int sem_up(sem_t sem)
{
  enter_critical_section();
  
  if(queue_length(sem->List)){   
    thread_unblock(tid);//wakeup this thread
    queue_dequeue(sem->List, (void*)tid);//remove thread via its ID   
  }//QUEUE_NOT_EMPTY
  else{
    sem->value += 1;
  }//increment
    
  exit_critical_section();
  return 0;
}


