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
schedule(deque_t *sched_queue, struct cdb *cdbs, int cdb_count,
	 struct fu_set *fus[]);

#endif /* end of include guard: SCHEDULE_H_ */

