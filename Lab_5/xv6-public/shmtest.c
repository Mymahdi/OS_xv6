#include "types.h"
#include "stat.h"
#include "user.h"

#define ADD 1000

// Test without monitor 
void test_without_monitor() {
  printf(1, "--- Running test without monitor ---\n");
  int shm_id = 1;
  int *shared_var;
  
  // open_shared_mem returns a virtual address
  shared_var = (int*)open_shared_mem(shm_id);
  if((uint)shared_var < 0){
    printf(1, "open_shared_mem failed\n");
    exit();
  }
  
  *shared_var = 0;

  int pid = fork();

  if (pid == 0) { // Child process
    for (int i = 0; i < ADD; i++) {
      (*shared_var)++;
    }
    close_shared_mem(shm_id);
    exit();
  } else { // Parent process
    for (int i = 0; i < ADD; i++) {
      (*shared_var)++;
    }
    wait();
    printf(1, "Final value (without monitor): %d\n", *shared_var);
    close_shared_mem(shm_id);
  }
}

// Test with monitor 
void test_with_monitor() {
  printf(1, "--- Running test with monitor ---\n");
  int shm_id = 2; // Use a different ID
  int initial_value = 0;

  monitor_init(shm_id, &initial_value, sizeof(int));
  
  int pid = fork();

  if (pid == 0) { // Child
    for (int i = 0; i < ADD; i++) {
      monitor_increase_all_elems(shm_id);
    }
    monitor_close_shared_mem(shm_id);
    exit();
  } else { // Parent
    for (int i = 0; i < ADD; i++) {
      monitor_increase_all_elems(shm_id);
    }
    wait();
    
    int final_value;
    monitor_read_shared_mem(shm_id, &final_value);
    printf(1, "Final value (with monitor): %d\n", final_value);
    
    monitor_close_shared_mem(shm_id);
  }
}

int main(void) {
  test_without_monitor();
  printf(1, "\n");
  test_with_monitor();
  exit();
}