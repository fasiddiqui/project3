//Author: Alex Whelan
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/errno.h>//FOR ERRNO VARIABLES
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

#include "queue.h"
#include "thread.h"
#include "tps.h"

//struct for sigsev?????
static void segv_handler(int sig, siginfo_t *si, void *context);

//pointer to Page Structure 
typedef struct page *PAGE;

/*
 * A Helper function to determine if operation is bound
 * Checks for all possible input
 * Returns: Integer {0,1} -> TRUE | FALSE
 */
int tps_bound(size_t offset, size_t length);

/*
 * A Helper function to find empty index in Page Table
 * Returns: Integer index
 */
int find_empty(void);

/*
 * A Helper function to iterator over the Page Table
 * Returns True or False
 */
int is_Allocated(pthread_t tid);

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
/*
 * Signal_Handler Function
 * Returns: UNKNOWN
 */

/*
 * EXIT_CRITICAL SECTION 
 * RETURNS: Signal
 */
int exit_critical(int sig)
{
  exit_critical_section();
  return (sig);
}//returns_safe

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
#define TRUE  1
#define FALSE 0

//PROTECT contains flags that control what kind of access is permitted
#define PFLAGS (PROT_READ | PROT_WRITE)

//FLAGS contains flags that control the nature of the map
#define MFLAGS (MAP_ANON | MAP_PRIVATE)

//  GLOBAL VARIABLES  //
static int Init = 0;
static PAGE table[PAGE_SIZE];

int tps_init(int segv)
{
  enter_critical_section();

  Init = 1;
  segv = 0;

  if (segv != 0)
  {
    //install a page fault handler to recognize TPS protection errors
    //call segv_handler()
    fprintf(stderr, "TPS protection error!\n");
  }
  else if (Init)
  {
    //SET VARIABLES HERE       
    int i;
    for (i = 0; i < PAGE_SIZE; i++)
    {
      table[i] = (PAGE)malloc(sizeof (PAGE));
      table[i]->id = (pthread_t)malloc(sizeof (pthread_t));
      table[i]->id = (pthread_t) (-1);
      table[i]->address = NULL;
      table[i]->valid = -1;
    }//initialize static array of pages

  }//initialize_TPS 
  else
  {
    exit_critical(-1);
  }//TPS API has already been initialized, or failure initialization.

  exit_critical_section();
  return 0; //success
}//tps_init()

int tps_create(void)
{
  enter_critical_section();
  //Pull Current Threads Id
  pthread_t cID;
  cID = pthread_self();

  if (is_Allocated(cID))
  {
    fprintf(stderr, "\n***ERROR*** Current thread has a TPS\n");
    exit_critical(-1);
  }

  //find_first_empty else SIG_SEV
  int index = find_empty();

  if (index == -1)
  {
    fprintf(stderr, "\n***ERROR*** OUT OF MEMORY -> RESIZE\n");
    exit_critical(-1);
  }

  //Allocated MAP to Index
  table[index]->id = cID;//memcpy or memset as a requirement???
  table[index]->address = (PAGE) mmap(NULL, TPS_SIZE, PFLAGS, MFLAGS, -1, 0);
  table[index]->valid = 1;

  if (table[index] == MAP_FAILED)
  {
    fprintf(stderr, "\n***ERROR*** Memory allocation\n");
    exit_critical(-1);
  }

  exit_critical_section();
  //TPS area was successfully created
  return 0;
}//tps_create()

int tps_destroy(void)
{
  enter_critical_section();
  //Pull Current Threads Id
  pthread_t cID;
  cID = pthread_self();

  if (!is_Allocated(cID))
  {
    fprintf(stderr, "\n***ERROR*** Current thread doesn't have a TPS\n");
    exit_critical(-1);
  }

  //Free pointer
  PAGE fpointer = tps_find(cID);

  fpointer->valid = -1;
  fpointer->id = (pthread_t) (-1);
  munmap(fpointer->address, TPS_SIZE);

  //maybe wait on this 
  free(fpointer);

  //TPS area was successfully destroyed.
  exit_critical_section();
  return 0;
}

int tps_read(size_t offset, size_t length, char *buffer)
{
  enter_critical_section();

  // CRITICAL SECTION
  pthread_t cID = pthread_self();

  if (!is_Allocated(cID))
  {
    fprintf(stderr, "\n***ERROR*** Current thread doesn't have a TPS\n");
    exit_critical(-1);
  }

  PAGE page_pointer = tps_find(cID);

  if (!tps_bound(offset, length))
  {
    fprintf(stderr, "\n***ERROR*** reading operation is out of bound\n");
    exit_critical(-1);
  }

  if (page_pointer)
  {

    if (page_pointer->address == NULL)
    {
      fprintf(stderr, "\n***SIGSEV*** NULL pointer exception\n");
      exit_critical(-1);
    }
  }//custom_error_checking

  //memcpy( &dst[dstIdx], &src[srcIdx], number of bytes to copy);

  if (memcpy(buffer, page_pointer->address + offset, length - offset)
      == MAP_FAILED)
  {
    fprintf(stderr, "\n***ERROR*** READ FAILED\n");
    exit_critical(-1);
  }

  exit_critical_section();
  return 0;
}

int tps_write(size_t offset, size_t length, char *buffer)
{

  enter_critical_section();

  if (buffer == NULL)
  {
    fprintf(stderr, "\n***FAILURE*** SEGMENTATION FAULT \n");
    exit_critical(-1);
  }

  // CRITICAL SECTION
  pthread_t cID = pthread_self();

  /*
   * If the current threads TPS shares a memory page with another threads TPS,
   * this should trigger a copy-on-write operation before the actual write occurs.
   */

  if (!is_Allocated(cID))
  {
    fprintf(stderr, "\n***ERROR*** Current thread doesn't have a TPS\n");
    exit_critical(-1);
  }

  PAGE page_pointer = tps_find(cID);

  if (!tps_bound(offset, length))
  {
    fprintf(stderr, "\n***ERROR*** writing operation is out of bound\n");
    exit_critical(-1);
  }

  if (page_pointer)
  {

    if (page_pointer->address == NULL)
    {
      fprintf(stderr, "\n***SIGSEV*** NULL pointer exception\n");
      exit_critical(-1);
    }
  }//custom_error_checking

  //memcpy( &dst[dstIdx], &src[srcIdx], number of bytes to copy);

  if (memcpy(page_pointer->address + offset, buffer, length - offset)
          == MAP_FAILED)
  {
    fprintf(stderr, "\n***ERROR*** Write FAILED\n");
    exit_critical(-1);
  }

  exit_critical_section();
  return 0;
}//tps_write()

int tps_clone(pthread_t tid)
{
  enter_critical_section();
  
  //calling thread[i.e. C++(this)]
  pthread_t call_id = pthread_self();
  PAGE newPage = pmap();

  if (!is_Allocated(tid))
  {
    fprintf(stderr, "\n***ERROR*** Current thread doesn't have a TPS\n");
    exit_critical(-1);
  }
 
  //find requesting tid
  PAGE reqPage = tps_find(tid);

  //copy contents 
  if (memcpy(newPage, reqPage, TPS_SIZE) == MAP_FAILED)
  {
    fprintf(stderr, "\n***ERROR*** CLONE FAILED\n");
    exit_critical(-1);
  }
  
  //memset or memcopy id since that is my tps_find identifying technique
  newPage->id = call_id;
  newPage->valid = 1;
  int index = find_empty();
  
  if(index > 0)
    table[index] = newPage;
  else{
    printf("\nindex less than zero\n");
  }

  //Insert into New Page into Empty Table Index
  //***
  //Clone-on-Write
  //PHASE 2.3

  exit_critical_section();
  return 0;
}

PAGE pmap(void)
{

  PAGE newPage = (PAGE) mmap(NULL, TPS_SIZE, PFLAGS, MFLAGS, -1, 0);

  if (newPage == MAP_FAILED)
  {
    fprintf(stderr, "\n***ERROR*** PMAP FAILED\n");
    exit_critical((int)MAP_FAILED);
    printf("\n\n\nTHIS SHOULD NOT GET PRINTED\n\n\n");
  }

  //Allocated MAP Anonymously
  newPage->id = (pthread_t) (-1);
  newPage->address = (PAGE) mmap(NULL, TPS_SIZE, PFLAGS, MFLAGS, -1, 0);
  newPage->valid = -1;

  return newPage;

}//create a mapping and return pointer to it.

PAGE tps_find(pthread_t tid)
{
  int index;

  for (index = 0; index < PAGE_SIZE; index++)
  {
    if (table[index]->id == tid)//might need to do *tid since pointer
      return table[index];
  }//TPS_UNALLOCATED

  return NULL;

}

int is_Allocated(pthread_t tid)
{
  int index;

  for (index = 0; index < PAGE_SIZE; index++)
  {
    if (table[index]->id == tid)//might need to do *tid since pointer
      if (table[index]->address != NULL && table[index]->valid != -1)
        return TRUE;
  }//TPS_UNALLOCATED

  return FALSE;
}

int find_empty()
{
  int index;
  int sig_sev = -1;

  for (index = 0; index < PAGE_SIZE; index++)
  {
    if (table[index]->valid == -1)
      if (table[index]->address == NULL)
        return (index);
  }//find_empty_location

  return sig_sev;
}//find_empty()

int tps_bound(size_t offset, size_t length)
{

  if ((length + offset) > TPS_SIZE)
  {
    return FALSE;
  }
  if ((length + offset) < 0)
  {
    return FALSE;
  }
  if ((length - offset) > TPS_SIZE)
  {
    return FALSE;
  }
  if ((length - offset) < 0)
  {
    return FALSE;
  }

  return TRUE;
}//tps_bound()

static void segv_handler(int sig, siginfo_t *si, void *context)
{
  //Get the address of the beginning of the page where SIGSEV occurred
  void *p_fault = (void*) ((uintptr_t) si->si_addr & ~(TPS_SIZE - 1));

  //iterate through all the TPS areas and find if p_fault matches 
  //<CODE> -> 

  if (TRUE/*there is a match */)
  {

    fprintf(stderr, "TPS protection error!\n");
  }

  /* In any case, restore the default signal handlers????*/
  signal(SIGSEGV, SIG_DFL);
  signal(SIGBUS, SIG_DFL);
  //Transmit the signal again in order to cause program to crash 

  raise(sig);
}

/*
 * APPROVED_BY: Ernesto "Che" Guevara
 */