#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "semaphore.h"

#define MAX_QUEUE_SIZE 6

extern int shared_waiting_customers; 
extern struct spinlock waiting_lock;

extern int customer_id_queue[MAX_QUEUE_SIZE];
extern int queue_head;
extern int queue_tail;
extern struct spinlock queue_lock;

int
sys_enqueue_customer_id(void)
{
  int customer_id;
  if(argint(0, &customer_id) < 0) return -1;

  acquire(&queue_lock);
 
  if ((queue_tail + 1) % MAX_QUEUE_SIZE == queue_head) {
   
    release(&queue_lock);
    return -1; 
  }
  customer_id_queue[queue_tail] = customer_id;
  queue_tail = (queue_tail + 1) % MAX_QUEUE_SIZE;
  release(&queue_lock);
  return 0;
}

int
sys_dequeue_customer_id(void)
{
  acquire(&queue_lock);
  
  if (queue_head == queue_tail) {
    
    release(&queue_lock);
    return -1;
  }
  int id = customer_id_queue[queue_head];
  queue_head = (queue_head + 1) % MAX_QUEUE_SIZE;
  release(&queue_lock);
  return id;
}

int sys_get_waiting_count(void) {
  acquire(&waiting_lock);
  int count = shared_waiting_customers;
  release(&waiting_lock);
  return count;
}

int sys_inc_waiting_count(void) {
  acquire(&waiting_lock);
  shared_waiting_customers++;
  int count = shared_waiting_customers; 
  release(&waiting_lock);
  return count;
}

int sys_dec_waiting_count(void) {
  acquire(&waiting_lock);
  shared_waiting_customers--;
  int count = shared_waiting_customers; 
  release(&waiting_lock);
  return count;
}

int sys_sem_init(void) {
  int id, value;
  if (argint(0, &id) < 0)
    return -1;
  if (argint(1, &value) < 0)
    return -1;

  return sem_init(id, value);
}

int sys_sem_wait(void) {
  int id;
  if (argint(0, &id) < 0)
    return -1;

  sem_wait(id);
  return 0;
}

int sys_sem_signal(void) {
  int id;
  if (argint(0, &id) < 0)
    return -1;

  sem_signal(id);
  return 0;
}


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int shared_data = 0;


struct rwlock {
  struct spinlock lock;
  int readers;
  int writers;
  int waiting_writers;
  int waiting_readers;
};

struct rwlock rwlock;

int sys_init_rw_lock(void)
{
  initlock(&rwlock.lock, "rwlock");
  acquire(&rwlock.lock);
  rwlock.readers = 0;
  rwlock.writers = 0;
  rwlock.waiting_writers = 0;
  rwlock.waiting_readers = 0;
  release(&rwlock.lock);
  return 0;
}

int sys_get_rw_pattern(void)
{
  int pattern;
  if (argint(0, &pattern) < 0)
    return -1;

  int found_first_one = 0;

  for (int i = 31; i >= 0; i--) {
    int op = (pattern >> i) & 1;

    if (!found_first_one) {
      if (op == 1) {
       
        found_first_one = 1;
      }
      continue; 
    }

    if (op == 0) {
      // Read
      acquire(&rwlock.lock);
      while (rwlock.writers > 0 || rwlock.waiting_writers > 0) {
        rwlock.waiting_readers++;
        sleep(&rwlock.readers, &rwlock.lock);
        rwlock.waiting_readers--;
      }
      rwlock.readers++;
      release(&rwlock.lock);

      cprintf("Reading shared data: %d\n", shared_data);

      acquire(&rwlock.lock);
      rwlock.readers--;
      if (rwlock.readers == 0 && rwlock.waiting_writers > 0)
        wakeup(&rwlock.writers);
      release(&rwlock.lock);

    } else {
      // Write
      acquire(&rwlock.lock);
      rwlock.waiting_writers++;
      while (rwlock.readers > 0 || rwlock.writers > 0) {
        sleep(&rwlock.writers, &rwlock.lock);
      }
      rwlock.waiting_writers--;
      rwlock.writers++;
      release(&rwlock.lock);

      shared_data++;
      cprintf("Writing to shared data: %d\n", shared_data);

      acquire(&rwlock.lock);
      rwlock.writers--;
      if (rwlock.waiting_writers > 0)
        wakeup(&rwlock.writers);
      else if (rwlock.waiting_readers > 0)
        wakeup(&rwlock.readers);
      release(&rwlock.lock);
    }
  }

  return 0;
}
