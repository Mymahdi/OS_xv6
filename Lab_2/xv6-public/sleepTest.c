

#include "types.h"
#include "stat.h"

#include "user.h"


int main(int argc, char *argv[])

{
  if(argc != 2){
    printf(1, "Usage: sleep_test <ticks>\n");
    exit();
  }

  int ticks = atoi(argv[1]);
  int start = uptime();

  set_sleep_syscall(ticks);

  int end = uptime();

  printf(1, "Sleept for %d ticks (measured: %d ticks)\n", ticks, end - start);   //few ticks extra is normal due to process scheduling
  exit();
}


