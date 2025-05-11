#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int pid = getpid();  // Get current process PID

  printf(1, "Before set_deadline: my pid is %d\n", pid);

  // Try changing scheduling class and level
  int result = set_deadline(10);  // Example: move to edf

  if (result == 0) {
    printf(1, "set_deadline succeeded.\n");
  } else {
    printf(1, "set_deadline failed.\n");
  }

  exit();
}
