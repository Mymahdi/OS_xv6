#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "syscall.h"

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

int sys_make_user_syscall(void) {
  int user_id;
  char *password;

  if (argint(0, &user_id) < 0 || argstr(1, &password) < 0)
    return -1;

  for (int i = 0; i < MAX_USERS; i++) {
    if (users[i].in_use && users[i].id == user_id)
      return -1;
  }

  for (int i = 0; i < MAX_USERS; i++) {
    if (!users[i].in_use) {
      users[i].id = user_id;
      safestrcpy(users[i].password, password, MAX_PASSWORD_LEN);
      users[i].in_use = 1;
      return 0;
    }
  }
  return -1;
}

int sys_login_syscall(void) {
  int user_id;
  char *password;

  if (argint(0, &user_id) < 0 || argstr(1, &password) < 0)
    return -1;

  for (int i = 0; i < MAX_USERS; i++) {
    if (users[i].in_use && users[i].id == user_id &&
        strncmp(users[i].password, password, MAX_PASSWORD_LEN) == 0) {
      current_user_id = user_id;
      return 0;
    }
  }
  return -1;
}

int sys_logout_syscall(void) {
  if (current_user_id != -1) {
    current_user_id = -1;
    return 0;
  }
  return -1;
}


int sys_get_logs_syscall(void) {
  if (current_user_id != -1) {
    for (int i = 0; i < MAX_USERS; i++) {
      if (logs[i].user_id == current_user_id) {
        for (int j = 0; j < logs[i].count; j++) {
          cprintf("Syscall %d\n", logs[i].syscalls[j]);
        }
        return 0;
      }
    }
  } else {
    for (int i = 0; i < MAX_USERS; i++) {
      if (logs[i].count > 0) {
        cprintf("User %d:\n", logs[i].user_id);
        for (int j = 0; j < logs[i].count; j++) {
          cprintf("  Syscall %d\n", logs[i].syscalls[j]);
        }
      }
    }
  }
  return 0;
}


int is_palindrome(int n) {
  int original = n, rev = 0;
  while (n > 0) {
    rev = rev * 10 + n % 10;
    n /= 10;
  }
  return original == rev;
}


int sys_next_palindrome(void) {
  int num;

  if (argint(0, &num) < 0)
    return -1;

  int next = num + 1;
  while (!is_palindrome(next))
    next++;

  cprintf("Next palindrome of %d is %d\n", num, next);
  return 0;
}


