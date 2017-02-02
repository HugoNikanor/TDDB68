#ifndef DEVICES_TIMER_H
#define DEVICES_TIMER_H

#include <round.h>
#include <stdint.h>
#include <list.h>

/* Number of timer interrupts per second. */
#define TIMER_FREQ 100

void timer_init (void);
void timer_calibrate (void);

int64_t timer_ticks (void);
int64_t timer_elapsed (int64_t);

bool sleeping_node_compare(const struct list_elem *a, const struct list_elem *b, void *aux);
void timer_sleep (int64_t ticks);
void timer_msleep (int64_t milliseconds);
void timer_usleep (int64_t microseconds);
void timer_nsleep (int64_t nanoseconds);

void timer_print_stats (void);

/* wait_queue object */
struct sleeping_node{
  int64_t wakeup_tick;
  struct thread *sleeping_thread; 
  struct list_elem elem; 
};

#endif /* devices/timer.h */
