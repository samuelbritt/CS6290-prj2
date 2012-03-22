/*
 * Module to group together functions related to the schedule stage of the
 * pipeline.
 */

#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include "deque.h"

struct fu_set;
struct cdb;

/* Schedules instructions to be run */
void
schedule(struct cdb *cdbs, int cdb_count, struct fu_set *fus[]);

/* Manipulates the internal scheduling queue */
void sched_init(); /* Must be called before any other function */
void sched_destroy();
struct reservation_station * sched_add_rs();
void sched_delete_rs(struct reservation_station *rs);
bool sched_queue_is_empty();

#endif /* end of include guard: SCHEDULE_H_ */

