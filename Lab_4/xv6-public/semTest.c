#include "types.h"
#include "stat.h"
#include "user.h"
#include "semaphore.h"

int
main(int argc, char *argv[])
{
    int sem_id = 0;

    printf(1, "Starting semaphore test\n");

    if (sem_init(sem_id, 1) < 0) {
        printf(1, "sem_init failed\n");
        exit();
    }

    printf(1, "Semaphore initialized\n");

    printf(1, "Calling sem_wait (P)...\n");
    sem_wait(sem_id);
    printf(1, "Inside critical section\n");

    sleep(10);

    printf(1, "Calling sem_signal (V)...\n");
    sem_signal(sem_id);

    printf(1, "Semaphore test finished\n");

    exit();
}
