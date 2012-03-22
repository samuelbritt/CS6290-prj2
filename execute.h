/*
 * Module to group together functions related to the execution stage of the
 * pipeline and the functional units.
 */

#ifndef EXECUTE_H_
#define EXECUTE_H_

#include "deque.h"

struct reservation_station;

enum FU_TYPES {
	FU0,
	FU1,
	FU2,
	FU_TYPE_COUNT
};

/* A singular FU of a given type, with its pipeline */
struct func_unit {
	bool busy;
	int latency;
	struct reservation_station **pipeline;
};

/* Groups together all FUs of a particular type */
struct fu_set {
	int count;
	struct func_unit *fus;
};

/* Creates and returns an `fu_set`, which contains the array of `func_unit`s
 * The return val can be freed with free() */
struct fu_set * create_fu_set(int fu_type, int fu_count);

/* Execution pipeline stage. Updates all FUs */
void execute(deque_t *exe_queue, deque_t *sched_queue, struct fu_set *fus[]);

#endif /* end of include guard: EXECUTE_H_ */
