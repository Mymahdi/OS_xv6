#include "types.h"   // معمولاً برای انواع پایه xv6
#include "defs.h"    // برای cprintf, sleep, wakeup و ...
#include "rwlock.h" 

// (In the same .c file as the rwlock struct instance)
void
kernel_start_read(void)
{
  acquire(&rwl.lk);
  // Wait if there's an active writer OR if any writers are waiting (writer preference)
  while (rwl.write_count > 0 || rwl.waiting_writers > 0) {
    rwl.waiting_readers++;
    sleep(rwl.chan_read, &rwl.lk); // Atomically releases rwl.lk, sleeps, reacquires on wakeup
    rwl.waiting_readers--;
  }
  rwl.read_count++;
  release(&rwl.lk);
}

void
kernel_end_read(void)
{
  acquire(&rwl.lk);
  rwl.read_count--;
  // If this was the last reader and writers are waiting, wake up one writer.
  if (rwl.read_count == 0 && rwl.waiting_writers > 0) {
    wakeup(rwl.chan_write);
  }
  release(&rwl.lk);
}

void
kernel_start_write(void)
{
  acquire(&rwl.lk);
  // Wait if there are active readers OR an active writer
  while (rwl.read_count > 0 || rwl.write_count > 0) {
    rwl.waiting_writers++;
    sleep(rwl.chan_write, &rwl.lk);
    rwl.waiting_writers--;
  }
  rwl.write_count++; // Mark one writer as active
  release(&rwl.lk);
}

void
kernel_end_write(void)
{
  acquire(&rwl.lk);
  rwl.write_count--; // Writer is no longer active
  // Prioritize waking up a waiting writer
  if (rwl.waiting_writers > 0) {
    wakeup(rwl.chan_write);
  } else if (rwl.waiting_readers > 0) {
    // If no writers waiting, wake up ALL waiting readers.
    // xv6 wakeup() wakes all processes sleeping on the channel.
    // Each reader's kernel_start_read() will re-check conditions.
    wakeup(rwl.chan_read);
  }
  release(&rwl.lk);
}