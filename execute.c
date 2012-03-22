/*
 * Module to group together functions related to the execution stage of the
 * pipeline and the functional units.
 */

#include <stdio.h>
#include <stdlib.h>

#include "execute.h"
#include "common.h"

/* Maps FU_TYPE to its latency */
int fu_latencies[] = { 1, 2, 3 };

/* Broadcasts and finishes the last instruction in the FU pipeline, if
 * possible */
static void
execute_last_inst(struct func_unit *fu)
{
	int latency = fu->latency;
	if (/* TODO a cdb is not busy && */
	    fu->pipeline[latency - 1] != NULL) {
		/* TODO finish instruction */
		fu->pipeline[latency - 1] = NULL;
	}
}

/* moves the instructions down the FU pipeline */
static void
execute_update_fu_pipeline(struct func_unit *fu)
{
	for (int i = fu->latency - 1; i > 0; --i) {
		if (fu->pipeline[i] == NULL) {
			fu->pipeline[i] = fu->pipeline[i - 1];
			fu->pipeline[i - 1] = NULL;
		}
	}
}

static void
execute_fu(struct func_unit *fu)
{
	execute_last_inst(fu);
	execute_update_fu_pipeline(fu);
}

static void
execute_fu_set(struct fu_set *set)
{
	for (int i = 0; i < set->count; ++i)
		execute_fu(&set->fus[i]);
}

/* Creates and returns an `fu_set`, which contains the array of `func_unit`s
 * The return val can be freed with free() */
struct fu_set *
create_fu_set(int fu_type, int fu_count)
{
	struct reservation_station **rs_arr;
	int latency = fu_latencies[fu_type];

	 /* Allocate one big chunk of memory and divide it */
	struct fu_set *set = emalloc(sizeof(*set) +
				     fu_count * (sizeof(*set->fus) +
						 latency * sizeof(*rs_arr)));
	set->count = fu_count;
	set->fus = (struct func_unit *) (set + 1);
	rs_arr = (struct reservation_station **) (set->fus + fu_count);
	for (int i = 0; i < fu_count; ++i) {
		set->fus[i].pipeline = rs_arr + i * latency;
		set->fus[i].latency = latency;
		set->fus[i].busy = 0;
	}
	return set;
}

/* Execution pipeline stage. Updates all FUs */
void
execute(deque_t *exe_queue, deque_t *sched_queue, struct fu_set *fus[])
{
	for (int i = 0; i < FU_TYPE_COUNT; ++i) {
		execute_fu_set(fus[i]);
	}
	while (!deque_is_empty(exe_queue)) {
		struct reservation_station *rs = deque_delete_first(exe_queue);
		deque_delete(sched_queue, rs);
		free(rs);
	}
}
