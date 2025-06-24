#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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

// Add the following code to the end of sysproc.c

// Wrapper for the shm_open helper function in vm.c
int
sys_open_shared_mem(void)
{
  int id;
  // Get the integer argument (the ID) from the user's call
  if(argint(0, &id) < 0)
    return -1;
  // Call the actual implementation in vm.c
  return shm_open(id);
}

// Wrapper for the shm_close helper function in vm.c
int
sys_close_shared_mem(void)
{
  int id;
  // Get the integer argument (the ID) from the user's call
  if(argint(0, &id) < 0)
    return -1;
  // Call the actual implementation in vm.c
  return shm_close(id);
}


// --- Monitor System Calls ---
// The following functions are also simple wrappers for helpers
// that you should define in vm.c

int
sys_monitor_init(void)
{
  int shared_mem_id;
  char* initial_value; // Pointer to user data
  int size_value;

  // Get arguments from the user call
  if(argint(0, &shared_mem_id) < 0 || argptr(1, &initial_value, sizeof(int)) < 0 || argint(2, &size_value) < 0)
    return -1;
  
  // Call a helper function in vm.c (that you need to create)
  return shm_monitor_init(shared_mem_id, (int*)initial_value, size_value);
}

int
sys_monitor_increase_all_elems(void)
{
  int shared_mem_id;
  if(argint(0, &shared_mem_id) < 0)
    return -1;
  
  // Call a helper function in vm.c (that you need to create)
  return shm_monitor_increase(shared_mem_id);
}

int
sys_monitor_close_shared_mem(void)
{
  int shared_mem_id;
  if(argint(0, &shared_mem_id) < 0)
    return -1;
  
  // This can just call the existing shm_close helper
  return shm_close(shared_mem_id);
}

int
sys_monitor_read_shared_mem(void)
{
  int shared_memory_id;
  char* data; // Pointer to user array where data will be copied

  if(argint(0, &shared_memory_id) < 0 || argptr(1, &data, sizeof(int)) < 0)
    return -1;
  
  // Call a helper function in vm.c (that you need to create)
  return shm_monitor_read(shared_memory_id, (int*)data);
}