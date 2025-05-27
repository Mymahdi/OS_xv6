#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "spinlock.h"
#define MAX_SEMAPHORES  10

struct semaphore {
  int value;             
  struct spinlock lock;  
};

extern struct semaphore semaphores[MAX_SEMAPHORES];


int sem_init(int id, int value);
void sem_wait(int id);
void sem_signal(int id);

#endif 