#include "types.h"
#include "user.h"
#include "semaphore.h"

#define SEM_CUSTOMERS 0
#define SEM_BARBER    1
#define SEM_MUTEX     2 // This mutex will still be useful for protecting local logic,
                        // but the *shared* waiting count is now protected by kernel spinlock.

#define CHAIRS 6

// Function to print log (remains the same)
void print_log(char *prefix, int id, char *msg, int count) {
  int t = uptime();
  if (id > 0)
    printf(1, "[%d] %s %d: %s %d\n", t, prefix, id, msg, count);
  else
    printf(1, "[%d] %s: %s %d\n", t, prefix, msg, count);
}

void barber() {
  while (1) {
    sem_wait(SEM_CUSTOMERS); // wait for a customer (customer_waiting semaphore)

    // No need for a separate mutex here to access 'waiting' as it's kernel-managed
    int current_waiting = dec_waiting_count(); // Decrement and get the new count from kernel
    print_log("Barber", 0, "starts cutting hair, customers waiting =", current_waiting);

    sem_signal(SEM_BARBER); // Signal to the customer that barber is ready for them

    sleep(50); // simulate haircut

    print_log("Barber", 0, "finished haircut", 0);
  }
}

void customer(int id) {
  // Use a local variable to store the count for checking.
  // The actual increment/decrement uses system calls.
  int current_waiting;

  // No need for sem_wait(SEM_MUTEX) here if the kernel syscalls are atomic.
  // The 'CHAIRS' limit needs careful handling.
  // We need to check the number of waiting customers *before* deciding to enter.

  // Re-thinking the customer logic:
  // 1. Acquire the MUTEX_SEM (if needed for other local logic or print statements)
  // 2. Check if chairs are available via get_waiting_count().
  // 3. If chairs available, increment waiting_count, then signal CUSTOMERS_SEM.
  // 4. Release MUTEX_SEM.
  // 5. Wait for BARBER_SEM.
  // 6. Else (no chairs), release MUTEX_SEM and leave.

  sem_wait(SEM_MUTEX); // Protect the check and increment logic (user-level part)

  current_waiting = get_waiting_count(); // Get current count from kernel
  if (current_waiting < CHAIRS) {
    current_waiting = inc_waiting_count(); // Increment and get new count from kernel
    print_log("Customer", id, "sits down, total waiting =", current_waiting);
    sem_signal(SEM_CUSTOMERS); // Notify barber there's a customer
    sem_signal(SEM_MUTEX);      // Release mutex before blocking on barber
    sem_wait(SEM_BARBER);      // Wait for barber to be ready
    print_log("Customer", id, "getting haircut", 0);
  } else {
    print_log("Customer", id, "left (no chair)", 0);
    sem_signal(SEM_MUTEX); // Release mutex if leaving
  }
  exit();
}

int main() {
  // Initialize semaphores (this part remains largely the same)
  sem_init(SEM_CUSTOMERS, 0);
  sem_init(SEM_BARBER, 0);
  sem_init(SEM_MUTEX, 1); // Mutex is still useful for protecting user-level checks/prints

  int barber_pid = fork();
  if (barber_pid == 0) {
    barber();
    // barber() loop is infinite, so no exit() here directly unless loop breaks
  }

  for (int i = 1; i <= 10; i++) {
    sleep(10); // stagger customer arrival
    int cid = fork();
    if (cid == 0) {
      customer(i);
      // customer() calls exit() internally
    }
  }

  for (int i = 0; i < 10; i++)
    wait(); // Wait for all customer processes to finish

  kill(barber_pid); // Kill the barber process
  wait(); // Wait for the barber process to exit
  exit();
}