// filepath: /home/ali/Documents/University/OS/OS_xv6/Lab_4/xv6-public/rwlock.h
#ifndef XV6_RWLOCK_H
#define XV6_RWLOCK_H

#include "spinlock.h" // برای استفاده از struct spinlock در تعریف struct rwlock

// تعریف ساختار rwlock
struct rwlock {
  struct spinlock lk;
  int read_count;
  int write_count;
  int waiting_readers;
  int waiting_writers;
  void *chan_read;
  void *chan_write;
};

// اعلان متغیرهای سراسری به صورت extern
extern struct rwlock rwl;
extern int shared_resource_val;


#endif // XV6_RWLOCK_H