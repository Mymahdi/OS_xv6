#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int pid = getpid();  // Get current process PID

  printf(1, "Before change_level: my pid is %d\n", pid);

  // Try changing scheduling class and level
  int result = change_level(pid, 0, 1);  // Example: move to default class, level 1 (interactive)

  if (result == 0) {
    printf(1, "change_level succeeded.\n");
  } else {
    printf(1, "change_level failed.\n");
  }

  exit();
}
