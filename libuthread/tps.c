//Author: Alex Whelan

#include "queue.h"
#include "thread.h"
#include "tps.h"


//struct for sigsev



typedef struct page *PAGE;
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
static void segv_handler(int sig, siginfo_t *si, void *context);



// DATA STRUCTURES //

struct page {
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
#define MFLAGS (MAP_ANON | MAP_SHARED)


//  GLOBAL VARIABLES  //
static int Init = 0;
static PAGE table[PAGE_SIZE];

int tps_init(int segv) {
    Init = 1;

    if (segv != 0) {
        fprintf(stderr, "TPS protection error!\n");

    }//install a page fault handler to recognize TPS protection errors
    else if (Init) {
        //SET VARIABLES HERE       
        int i;

        for (i = 0; i < PAGE_SIZE; i++) {
            table[i] = malloc(sizeof (PAGE));
            table[i]->id = malloc(sizeof (pthread_t));
            table[i]->id = (pthread_t) (-1);
            table[i]->address = NULL;
            table[i]->valid = -1;
        }//initialize static array of pages

    }//initialize_TPS 
    else {
        return (-1);
    }//TPS API has already been initialized, or failure initialization.

    return 0; //success
}//tps_init()

int tps_create(void) {
    //Pull Current Threads Id
    pthread_t cID = NULL;
    cID = pthread_self();

    if (is_Allocated(cID)) {
        fprintf(stderr, "\n***ERROR*** Current thread has a TPS\n");
        return (-1);
    }

    //find_first_empty else SIG_SEV
    int index = find_empty();

    if (index == -1) {
        fprintf(stderr, "\n***ERROR*** PAGE MEMORY EMPTY\n");
        return (-1);
    }

    //Allocated MAP to Index
    table[index]->id = cID;
    table[index]->address = (PAGE) mmap(NULL, TPS_SIZE, PFLAGS, MFLAGS, -1, 0);
    table[index]->valid = 1;

    if (table[index] == MAP_FAILED) {
        fprintf(stderr, "\n***ERROR*** Memory allocation\n");
        return (-1);
    }

    //TPS area was successfully created
    return 0;
}//tps_create()

int tps_destroy(void) {

    //Pull Current Threads Id
    pthread_t cID = NULL;
    cID = pthread_self();

    if (!is_Allocated(cID)) {
        fprintf(stderr, "\n***ERROR*** Current thread doesn't have a TPS\n");
        return (-1);
    }

    //TPS area was successfully destroyed.
    table[(int) cID]->valid = 0;
    table[(int) cID]->id = NULL;
    munmap(table[(int) cID]->address, TPS_SIZE);

    /*  
     *******
     *******
     *******
     ******* 
     *******     FREEEEEEEEEEEEE 
     */
    //free(table[index]);

    return 0;
}

int tps_read(size_t offset, size_t length, char *buffer) {

    enter_critical_section();

    // CRITICAL SECTION
    pthread_t cID = pthread_self();

    if (!is_Allocated(cID)) {
        fprintf(stderr, "\n***ERROR*** Current thread doesn't have a TPS\n");
        return (-1);
    }

    PAGE cpy_pointer = tps_find(cID);

    if (cpy_pointer == NULL) {
        fprintf(stderr, "\n***ERROR*** reading operation is out of bound\n");
        return (-1);
    }

    //memcpy( &dst[dstIdx], &src[srcIdx], numElementsToCopy * sizeof(Element));

    memcpy(buffer, cpy_pointer + offset, length - offset);

    exit_critical_section();
    return 0;
}

int tps_write(size_t offset, size_t length, char *buffer) {

    enter_critical_section();

    // CRITICAL SECTION
    pthread_t cID = pthread_self();  
    
    /*
     * If the current threads TPS shares a memory page with another threads TPS,
     * this should trigger a copy-on-write operation before the actual write occurs.
     */
    
    if (!is_Allocated(cID)) {
        fprintf(stderr, "\n***ERROR*** Current thread doesn't have a TPS\n");
        return (-1);
    }

    PAGE cpy_pointer = tps_find(cID);

    if (cpy_pointer == NULL) {
        fprintf(stderr, "\n***ERROR*** writing operation is out of bound\n");
        return (-1);
    }

    //memcpy( &dst[dstIdx], &src[srcIdx], numElementsToCopy * sizeof(Element));

    memcpy(cpy_pointer + offset, buffer, length - offset);
    

    exit_critical_section();
    return 0;
}

int tps_clone(pthread_t tid) {
    //calling thread
    pthread_t call_id = pthread_self();
    PAGE newPage = pmap();

    //find requesting tid + current tid
    PAGE reqPage = tps_find(tid);
    PAGE cPage = tps_find(call_id);

    if (reqPage == NULL) {
        fprintf(stderr, "\n***ERROR*** PAGE NOT FOUND\n");
        return (-1);
    }//thread @tid doesn't have a TPS
    else if (cPage != NULL) {
        if (cPage->valid == 1) {
            fprintf(stderr, "\n***ERROR*** Current thread already has a TPS\n");
            return (-1);
        }
    }//Check_CurrentThread

    //copy contents 
    memcpy(newPage, reqPage, TPS_SIZE);

    if (newPage == MAP_FAILED) {
        fprintf(stderr, "\n***ERROR*** Copy FAILED\n");
        return (-1);
    }

    //Insert into New Page into Empty Table Index
    //***
    //***
    //***
    //***
    //PHASE 2.3

    return 0;
}

PAGE pmap(void) {

    PAGE newPage = (PAGE) mmap(NULL, TPS_SIZE, PFLAGS, MFLAGS, -1, 0);

    if (newPage == MAP_FAILED) {
        fprintf(stderr, "\n***ERROR*** Memory allocation\n");
        return NULL;
    }

    //Allocated MAP Anonymously
    newPage->id = (pthread_t) (-1);
    newPage->address = (PAGE) mmap(NULL, TPS_SIZE, PFLAGS, MFLAGS, -1, 0);
    newPage->valid = -1;

    return newPage;

}//create a mapping and return pointer to it.

PAGE tps_find(pthread_t tid) {
    int index;

    for (index = 0; index < PAGE_SIZE; index++) {
        if (table[index]->id == tid)//might need to do *tid since pointer
            return table[index];
    }//TPS_UNALLOCATED

    return NULL;

}

int is_Allocated(pthread_t tid) {
    int index;

    for (index = 0; index < PAGE_SIZE; index++) {
        if (table[index]->id == tid)//might need to do *tid since pointer
            if (table[index]->address != NULL && table[index]->valid != -1)
                return TRUE;
    }//TPS_UNALLOCATED

    return FALSE;
}

int find_empty() {

    int index;
    int sig_sev = -1;

    for (index = 0; index < PAGE_SIZE; index++) {
        if (table[index]->valid == -1)
            if (table[index]->address == NULL)
                return (index);
    }//find_empty_location

    return sig_sev;
}//find_empty()


static void segv_handler(int sig, siginfo_t *si, void *context){
    
    //Get the address of the beginning of the page where SIGSEV occurred
    
    void *p_fault = (void*)((uintptr_t)si->si_addr & ~(TPS_SIZE - 1));
    
    //iterate through all the TPS areas and find if p_fault mathces 
    
    //<CODE> -> 
    
    if(1/*there is a match */){
        
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