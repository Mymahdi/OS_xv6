#include "types.h"
#include "user.h"
#include "semaphore.h"

#define SEM_CUSTOMERS 0
#define SEM_BARBER    1
#define SEM_MUTEX     2

#define CHAIRS 6

int waiting = 0;

void print_log(char *prefix, int id, char *msg, int count) {
  int t = uptime();
  if (id > 0)
    printf(1, "[%d] %s %d: %s %d\n", t, prefix, id, msg, count);
  else
    printf(1, "[%d] %s: %s %d\n", t, prefix, msg, count);
}

void barber() {
  while (1) {
    sem_wait(SEM_CUSTOMERS); // wait for a customer

    sem_wait(SEM_MUTEX);
    waiting--;
    int current_waiting = waiting;
    print_log("Barber", 0, "starts cutting hair, customers waiting =", current_waiting);
    sem_signal(SEM_BARBER);
    sem_signal(SEM_MUTEX);

    sleep(50); // simulate haircut

    print_log("Barber", 0, "finished haircut", 0);
  }
}

void customer(int id) {
  sem_wait(SEM_MUTEX);
  if (waiting < CHAIRS) {
    waiting++;
    int current_waiting = waiting;
    print_log("Customer", id, "sits down, total waiting =", current_waiting);
    sem_signal(SEM_CUSTOMERS); // notify barber
    sem_signal(SEM_MUTEX);
    sem_wait(SEM_BARBER);      // wait for barber
    print_log("Customer", id, "getting haircut", 0);
  } else {
    print_log("Customer", id, "left (no chair)", 0);
    sem_signal(SEM_MUTEX);
  }
  exit();
}

int main() {
  sem_init(SEM_CUSTOMERS, 0);
  sem_init(SEM_BARBER, 0);
  sem_init(SEM_MUTEX, 1);

  int barber_pid = fork();
  if (barber_pid == 0) {
    barber();
    exit();
  }

  for (int i = 1; i <= 10; i++) {
    sleep(10); // stagger customer arrival
    int cid = fork();
    if (cid == 0) {
      customer(i);
    }
  }

  for (int i = 0; i < 10; i++)
    wait();

  kill(barber_pid);
  exit();
}
