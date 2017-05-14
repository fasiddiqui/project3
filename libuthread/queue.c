#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

typedef struct node
{
  void *data;
  struct node *next;
} Node;

typedef struct queue
{
  unsigned int size;
  Node *head, *tail;
} Queue;

// A utility function to create a new linked list node.

Node* newNode(void *d)
{
  Node *tmp = (Node*) malloc(sizeof (Node));
  tmp->data = d;
  tmp->next = NULL;
  return tmp;
}// SOURCE: http://quiz.geeksforgeeks.org

queue_t queue_create(void)
{
  queue_t q = malloc(sizeof (*q));
  q->size = 0;

  if (q)
  {
    q->head = q->tail = NULL;
    return q;
  }//initialize

  return NULL; //failure
}

int queue_destroy(queue_t queue)
{
  //Error_Checking
  //Double-free or !isEmpty()
  if (queue == NULL)
    return (-1);
  else if (queue->head)
    return (-1);

  Node *ptr, *tmp;
  tmp = ptr = queue->head;

  for (ptr = queue->head; ptr != NULL; ptr = tmp)
  {
    tmp = ptr->next;
    free(ptr);
  }//free_internal_elements

  free(queue);

  return (0);
}

int queue_enqueue(queue_t queue, void *data)
{
  //Error_Checking
  if (!queue || !data)
    return (-1); //SIGSEV or NULL data

  //Create newNode
  Node *tmp = newNode(data);

  if (queue->tail == NULL)
  {
    queue->head = queue->tail = tmp;
    queue->size++;
    return (0);
  }//IF_EMPTY :- 1st Element

  //newNode Inserted as Last Node
  queue->tail->next = tmp;
  queue->tail = tmp;
  queue->size++;

  return (0); //success
}

int queue_dequeue(queue_t queue, void **data)
{
  //Error_Checking
  //Empty Queue || Corrupt Pointers
  if (queue->head == NULL)
    return (-1);
  else if (!queue || !data)
    return (-1);
  else if(queue->size = 0){
    fprintf(stderr, "Attempting to free empty queue\n");
    return(-1);
  }
  
  //Pop_Top_of_LL+save+Free
  
  *data = queue->head->data;
  
  /*Free_Node_Save_Space (Experimental)
  Node fNode = NULL;
  fNode = queue->head;
  free(fNode); <-THIS line must go last */ 
  
  queue->head = queue->head->next;
  queue->size--;

  if (queue->head == NULL)
    queue->tail = NULL;

  return (0);
}

int queue_delete(queue_t queue, void *data)
{
  //for deleting the node
  Node *tmp, *ptr, *prev;


  Node *itr = queue->head;


  while (itr)
  {

    if (itr->data == data)
      printf("****MATCH FOUND****\n");

    itr = itr->next;


  }

  //Error_Checking
  if (!queue || !data)
  {
    return (-1);
  }
  else if (data)
  {
    tmp = ptr = queue->head;

    //Element @ Head of List
    if (data == ptr->data)
    {
      tmp = ptr->next;
      free(ptr);
      queue->head = tmp;
      return (0); //success
    }

    //Element within bound
    for (ptr = queue->head; ptr->next != NULL; ptr = tmp)
    {
      tmp = ptr->next;

      if (tmp->data == data)
      {
        tmp = ptr->next->next;
        free(ptr->next);
        ptr->next = tmp;
        return (0);
      }
      prev = ptr;
    }//find(index)

    //Element @ Tail
    if (data == queue->tail)
    {
      prev->next = NULL;
      free(queue->tail);
      queue->tail = prev;
      return (0);
    }
  }//Search_Queue_From_head

  return (-1); //NOT_FOUND
}

void print(void *data)
{
  int *a = (int *)data;
  
  printf("%d\n", *a);
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
  Node *cNode = NULL;
  int index = 0;
   
  //Default Print QUEUE Content
  if (func == NULL)
  {
    void(*func_ptr)(void *) = &print;

    for (cNode = queue->head; cNode != NULL; cNode = cNode->next)
    {
      //callback function here
      printf("Data[%d] : ", index++);
      func_ptr(cNode->data);
    }
  }//PRINT BY DEFAULT
  else if (data == NULL)
  {
    for (cNode = queue->head; cNode != NULL; cNode = cNode->next)
    {
      //callback function here
      func(queue, cNode->data, arg);
    }
    
  }//use Callback Function (INC_ITEM)
  else{
    
    int match = (intptr_t)arg;
     
    for (cNode = queue->head; cNode != NULL; cNode = cNode->next)
    {     
      //callback function here
      if(func(queue, cNode->data, arg)){

        *data = (intptr_t *)cNode->data;
        
        if(data)
          return (int)(*data);
      }//FOUND_ITEM          
    }//Iteration       
  }//find_item

  return (0);
}

int queue_length(queue_t queue)
{
  return (queue->size);
}

