#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "semaphore.h"
#include "rwlock.h"

int
sys_sem_init(void)
{
  int id, value;
  if (argint(0, &id) < 0)
    return -1;
  if (argint(1, &value) < 0)
    return -1;

  return sem_init(id, value);
}

int
sys_sem_wait(void)
{
  int id;
  if (argint(0, &id) < 0)
    return -1;

  sem_wait(id);
  return 0;
}

int
sys_sem_signal(void)
{
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

int
sys_init_rw_lock(void)
{
  initlock(&rwl.lk, "rwlock"); // Initialize the spinlock
  rwl.read_count = 0;
  rwl.write_count = 0;
  rwl.waiting_readers = 0;
  rwl.waiting_writers = 0;
  // Using simple non-zero pointers for channels.
  // These just need to be unique identifiers for sleep/wakeup.
  rwl.chan_read = (void*)1;
  rwl.chan_write = (void*)2;
  shared_resource_val = 0; // Initialize shared data
  return 0;
}

int
sys_get_rw_pattern(void)
{
  int pattern;
  // argint fetches the integer argument from the user stack
  if (argint(0, &pattern) < 0) {
    return -1; // Error fetching argument
  }

  if (pattern <= 0) { // Pattern must be positive
    return -1;
  }

  // Find the position of the most significant bit (MSB)
  int msb_pos = -1;
  for (int i = 31; i >= 0; i--) { // Assuming 32-bit int
    if ((pattern >> i) & 1) {
      msb_pos = i;
      break;
    }
  }

  if (msb_pos == -1) { // Should not happen if pattern > 0
    return -1;
  }

  // Iterate from the bit *after* the MSB '1' (left) down to bit 0 (right)
  // These are bits at positions msb_pos-1, msb_pos-2, ..., 0
  for (int i = msb_pos - 1; i >= 0; i--) {
    int operation_bit = (pattern >> i) & 1; // Get the operation bit

    if (operation_bit == 0) { // Read operation [cite: 45]
      kernel_start_read();
      // Simulate reading: print value, maybe delay
      cprintf("PID %d: Start Read. Shared_Val=%d. (ActiveR:%d, ActiveW:%d, WaitR:%d, WaitW:%d)\n",
              myproc()->pid, shared_resource_val, rwl.read_count, rwl.write_count, rwl.waiting_readers, rwl.waiting_writers);
      // for(volatile int d=0; d<100000; d++); // Optional delay to simulate work
      // yield(); // Optional: encourage context switches for testing
      cprintf("PID %d: End Read. Shared_Val=%d.\n", myproc()->pid, shared_resource_val);
      kernel_end_read();
    } else { // Write operation [cite: 45]
      kernel_start_write();
      shared_resource_val++; // Increment shared data [cite: 48]
      cprintf("PID %d: Start Write. Shared_Val Updated To %d. (ActiveR:%d, ActiveW:%d, WaitR:%d, WaitW:%d)\n",
              myproc()->pid, shared_resource_val, rwl.read_count, rwl.write_count, rwl.waiting_readers, rwl.waiting_writers);
      // for(volatile int d=0; d<100000; d++); // Optional delay
      // yield();
      cprintf("PID %d: End Write. Shared_Val=%d.\n", myproc()->pid, shared_resource_val);
      kernel_end_write();
    }
  }
  return 0; // Success
}
