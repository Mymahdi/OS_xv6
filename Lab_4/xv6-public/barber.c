#include "types.h"
#include "user.h"
#include "semaphore.h"

#define SEM_CUSTOMERS 0
#define SEM_BARBER    1
#define SEM_MUTEX     2

#define CHAIRS 5 

void print_log(char *prefix, int id, char *msg, int count) {
  int t = uptime();
  if (id > 0)
    printf(1, "[%d] %s %d: %s %d\n", t, prefix, id, msg, count);
  else
    printf(1, "[%d] %s: %s %d\n", t, prefix, msg, count);
}

void barber() {
  int customer_id_being_served;

  while (1) {
    sem_wait(SEM_CUSTOMERS);

    customer_id_being_served = dequeue_customer_id();
    int current_waiting = dec_waiting_count();

    print_log("Barber", 0, "starts cutting hair for Customer", customer_id_being_served);
    printf(1, "Barber: Customers waiting = %d\n", current_waiting);

    sem_signal(SEM_BARBER);

    sleep(50);

    print_log("Barber", 0, "finished haircut for Customer", customer_id_being_served);
  }
}

void customer(int id) {
  int current_waiting;

  sem_wait(SEM_MUTEX);

  current_waiting = get_waiting_count();
  if (current_waiting < CHAIRS) {
    current_waiting = inc_waiting_count();
    enqueue_customer_id(id);

    print_log("Customer", id, "sits down, total waiting =", current_waiting);
    sem_signal(SEM_CUSTOMERS);
    sem_signal(SEM_MUTEX);

    sem_wait(SEM_BARBER);
    printf(1, "[%d] Customer %d: getting haircut\n", uptime(), id);
  }
  else {
    printf(1, "[%d] Customer %d: left (no chair)\n", uptime(), id);
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
  }

  for (int i = 1; i <= 10; i++) {
    sleep(10);
    int cid = fork();
    if (cid == 0) {
      customer(i);
    }
  }

  for (int i = 0; i < 10; i++)
    wait();

  kill(barber_pid);
  wait();
  exit();
}