#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
  int pid = getpid();
  printf(1, "PID=%d before set_deadline\n", pid);

  if (set_deadline(15) < 0) {
    printf(1, "ERROR: set_deadline failed\n");
    exit();
  }
  printf(1, "set_deadline(15) OK\n");
  print_info();

  // switch to RR
  if (change_level(pid, 2, 1) < 0)
    printf(1, "ERROR: change_queue failed\n");
  else {
    printf(1, "change_queue to (2,1) OK\n");
    print_info();
  }

  // busy-work
  for (volatile int i = 0; i < 50000000; i++);
  printf(1, "done busy-loop\n");

  exit();
}
