/*
 * Module to group together functions related to the schedule stage of the
 * pipeline.
 */

#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include "deque.h"

struct cdb;

/* Schedules instructions to be run */
void schedule();

/* Wakes up an instruction eligible for execution on the given fu type and
 * returns it. If there is no such instruction, returns NULL */
struct reservation_station * sched_wakeup(int fu_type);

/* Broadcasts the cdb to all the reservation stations, updating their source
 * registers if the tags match */
void sched_broadcast_cdb(struct cdb *cdb);

/* Keep running counters of instructions fired ard and instructions retired.
 * These counters can be reset to 0 using the respective *_clear() functions. */
int sched_fired_instruction_counter();
int sched_retired_instruction_counter();
void sched_fired_instruction_counter_clear();
void sched_retired_instruction_counter_clear();

/* Manipulates the internal scheduling queue */
void sched_init(); /* Must be called before any other function */
void sched_destroy();
struct reservation_station * sched_add_rs();
void sched_delete_rs(struct reservation_station *rs);
bool sched_queue_is_empty();
int sched_queue_current_size();

#endif /* end of include guard: SCHEDULE_H_ */
