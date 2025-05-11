#include "types.h"
#include "stat.h"
#include "user.h"

#define NUM_PROCS 9

int
main(void)
{
  int father = getpid();
  printf(1, "[%d] I'm main\n", father);
  int i;
  for (i = 0; i < NUM_PROCS; i++) {
    int pid = fork();
    if (pid < 0) {
      printf(1, "Fork failed at %d\n", i);
      exit();
    }

    if (pid == 0) {
      // child process
      int me = getpid();
      if (i < 2) {
        // First 3 -> EDF
        set_deadline(20 + i * 10); // deadlines: 20, 30, 40
        printf(1, "[%d] I'm EDF with deadline %d\n", me, 20 + i * 10);
      } else if (i < 6) {
        // Next 3 -> RR
        change_level(me, 2);
        printf(1, "[%d] I'm RR\n", me);
      } else {
        // Last 3 -> FCFS
        change_level(me, 0);
        printf(1, "[%d] I'm FCFS\n", me);
      }
      exit();
    }
    // parent continues
  }
  print_info();
  // Parent waits for all children
  for (i = 0; i < NUM_PROCS; i++) {
    wait();
  }

  printf(1, "All children finished.\n\n");

  // Optional: view scheduling info
  print_info();
  exit();
}
