#include "types.h"
#include "defs.h"
#include "semaphore.h"

struct semaphore semaphores[MAX_SEMAPHORES];

int sem_init(int id, int value) {
  if (id < 0 || id >= MAX_SEMAPHORES)
    return -1;
  
  struct semaphore *sem = &semaphores[id];
  
  initlock(&sem->lock, "semaphore");
  sem->value = value;
  
  return 0;
}

void sem_wait(int id) {
  if (id < 0 || id >= MAX_SEMAPHORES)
    return;

  struct semaphore *sem = &semaphores[id];

  acquire(&sem->lock);
  while (sem->value <= 0) {
    sleep(sem, &sem->lock);
  }
  sem->value--;
  release(&sem->lock);
}

void sem_signal(int id) {
  if (id < 0 || id >= MAX_SEMAPHORES)
    return;

  struct semaphore *sem = &semaphores[id];

  acquire(&sem->lock);
  sem->value++;
  wakeup(sem);
  release(&sem->lock);
}