#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "queue.h"
#include "thread.h"
#include "tps.h"

typedef struct page *PAGE;

/*
 * A Helper function to iterator over the Page Table
 * Methods based on option parameter
 */
int tps_iterate(pthread_t tid, int option);

/*
 * A Helper function to find Page by thread_ID
 * Returns pointer to Page
 */
PAGE tps_find(pthread_t tid);


/*
 * A Helper function to create empty Page
 * Returns pointer to new Page
 */
PAGE pmap(void);


// DATA STRUCTURES //

struct page
{
  //member variables
  pthread_t id;
  int valid;
  void *address; //address maps to anonymous memory pointer

}; //alias -> PAGE

// MACROS  //
#define PAGE_SIZE 1024

//PROTECT contains flags that control what kind of access is permitted
#define PFLAGS (PROT_READ | PROT_WRITE)

//FLAGS contains flags that control the nature of the map
#define MFLAGS (MAP_ANON | MAP_SHARED)


//  GLOBAL VARIABLES  //
static int Init = 0;
static PAGE table[PAGE_SIZE];

int tps_init(int segv)
{
  Init = 1;

  if (segv != 0)
  {
    fprintf(stderr, "TPS protection error!\n");

  }//install a page fault handler to recognize TPS protection errors
  else if (Init)
  {
    //SET VARIABLES HERE       
    int i;

    for (i = 0; i < PAGE_SIZE; i++)
    {

      table[i] = malloc(sizeof (PAGE));
      table[i]->id = malloc(sizeof (pthread_t));
      table[i]->valid = 0;
    }//initialize static array of pages

  }//initialize_TPS 
  else
  {
    return (-1);
  }//TPS API has already been initialized, or failure initialization.

  return 0; //success
}//tps_init()

int tps_create(void)
{
  //Pull Current Threads Id
  pthread_t cID = NULL;
  cID = pthread_self();

  if (tps_iterate(cID, 1))
  {
    fprintf(stderr, "\n***ERROR*** Current thread has a TPS\n");
    return (-1);
  }

  //Allocated MAP to Index
  table[1]->id = cID;
  table[1]->address = (PAGE) mmap(NULL, TPS_SIZE, PFLAGS, MFLAGS, -1, 0);
  table[1]->valid = 1;

  if (table[1] == MAP_FAILED)
  {
    fprintf(stderr, "\n***ERROR*** Memory allocation\n");
    return (-1);
  }

  //TPS area was successfully created
  return 0;
}//tps_create()

int tps_destroy(void)
{

  //Pull Current Threads Id
  pthread_t cID = NULL;
  cID = pthread_self();

  if (tps_iterate(cID, 0))
  {
    fprintf(stderr, "\n***ERROR*** Current thread doesn't have a TPS\n");
    return (-1);
  }

  //TPS area was successfully destroyed.
  table[(int) cID]->valid = 0;
  table[(int) cID]->id = NULL;
  munmap(table[(int) cID]->address, TPS_SIZE);
  return 0;
}

int tps_read(size_t offset, size_t length, char *buffer)
{

  enter_critical_section();


  // CRITICAL SECTION
  // WHICH PHASE AND HOW


  exit_critical_section();
  return 0;
}

int tps_write(size_t offset, size_t length, char *buffer)
{

  enter_critical_section();


  // CRITICAL SECTION
  // WHICH PHASE AND HOW

  exit_critical_section();
  return 0;
}

int tps_iterate(pthread_t tid, int option)
{

  int index;

  //TESTS FOR DATA VALID/INVALID
  if (option == 0)
  {
    for (index = 0; index < PAGE_SIZE; index++)
    {
      if (table[index]->id = tid)
        if (table[index]->valid == 0)
          return (-1);
    }//TPS_UNALLOCATED
  }//TEST_DESTROY_TPS
  else if (option == 1)
  {
    for (index = 0; index < PAGE_SIZE; index++)
    {
      if (table[index]->id = tid)
        if (table[index]->valid == 1)
          return (-1);
    }//TPS_ALLOCATED

  }//TEST_CREATE_TPS

  return (0);
}//tps_iterate()

PAGE tps_find(pthread_t tid)
{
  int index;

  for (index = 0; index < PAGE_SIZE; index++)
  {
    if (table[index]->id = tid)//might need to do *tid since pointer
      return table[index];
  }//TPS_UNALLOCATED

  return NULL;

}

int tps_clone(pthread_t tid)
{

  //calling thread
  pthread_t call_id = pthread_self();
  PAGE newPage = pmap();


  //find requesting tid + current tid
  PAGE reqPage = tps_find(tid);
  PAGE cPage = tps_find(call_id);

  if (reqPage == NULL)
  {
    fprintf(stderr, "\n***ERROR*** PAGE NOT FOUND\n");
    return (-1);
  }//thread @tid doesn't have a TPS
  else if (cPage != NULL)
  {
    if (cPage->valid == 1)
    {
      fprintf(stderr, "\n***ERROR*** Current thread already has a TPS\n");
      return (-1);
    }
  }//Check_CurrentThread


  //copy contents 
  memcpy(newPage, reqPage, sizeof (PAGE));
  
  if (newPage == MAP_FAILED)
  {
    fprintf(stderr, "\n***ERROR*** Copy FAILED\n");
    return(-1);
  }

  //Insert into New Page into Empty Table Index
  //PHASE 2.3

  return 0;
}

PAGE pmap(void)
{

  PAGE newPage = (PAGE) mmap(NULL, TPS_SIZE, PFLAGS, MFLAGS, -1, 0);

  if (newPage == MAP_FAILED)
  {
    fprintf(stderr, "\n***ERROR*** Memory allocation\n");
    return NULL;
  }

  //Allocated MAP Anonymously
  newPage->id = (pthread_t) (-1);
  newPage->address = (PAGE) mmap(NULL, TPS_SIZE, PFLAGS, MFLAGS, -1, 0);
  newPage->valid = -1;

  return newPage;

}//create a mapping and return pointer to it.