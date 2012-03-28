/*
 * Module to group together functions related to the execution stage of the
 * pipeline and the functional units.
 */

#include <stdio.h>
#include <stdlib.h>

#include "deque.h"
#include "logger.h"
#include "common.h"

#include "execute.h"
#include "schedule.h"
#include "state_update.h"

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

/* Global array of all FUs */
struct fu_set *all_fus[FU_TYPE_COUNT];

/* Maps FU_TYPE to its latency */
int fu_latencies[] = { 1, 2, 3 };

/* Broadcasts and finishes the last instruction in the FU pipeline, if
 * possible */
static void
execute_last_inst(struct func_unit *fu)
{
	int latency = fu->latency;
	struct reservation_station **last_rs = &fu->pipeline[latency - 1];
	if (*last_rs && !retire_inst(*last_rs)) {
		vlog_inst((*last_rs)->dest.tag, "Complete");
		*last_rs = NULL;
	}
}

/* moves the instructions down the FU pipeline */
static void
execute_update_fu_pipeline(struct func_unit *fu)
{
	for (int i = fu->latency - 1; i > 0; --i) {
		if (fu->pipeline[i] != NULL)
			continue;
		fu->pipeline[i] = fu->pipeline[i - 1];
		fu->pipeline[i - 1] = NULL;
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

/* Returns true if the FU is free for the next instruction */
static inline bool
fu_is_free(struct func_unit *fu)
{
	return fu->pipeline[0] == NULL;
}

/* Returns a free FU in `set`, if one exists. Else returns NULL. */
static struct func_unit *
find_free_fu(struct fu_set *set) {
	struct func_unit *fu;
	for (int i = 0; i < set->count; ++i) {
		fu = &set->fus[i];
		if (fu_is_free(fu))
			return fu;
	}
	return NULL;
}

/* Inserts the reservation station into the first pipeline stage of a free
 * FU in `fu_set`. If successful, returns 0. If no free FU is available,
 * returns 1 */
static int
issue_instruction_(struct fu_set *fu_set, struct reservation_station *rs)
{
	struct func_unit *free_fu;
	if ((free_fu = find_free_fu(fu_set))) {
		free_fu->pipeline[0] = rs;
		return 0;
	}
	return 1;
}

/* Inserts the reservation station into the first pipeline stage of a free
 * FU of the type specified by `rs`. If successful, returns 0. If no free FU is
 * available, returns 1 */
int
issue_instruction(struct reservation_station *rs)
{
	return issue_instruction_(all_fus[rs->fu_type], rs);
}

/* Creates and returns an `fu_set`, which contains the array of `func_unit`s
 * The return val can be freed with free() */
static struct fu_set *
create_fu_set(int fu_type, int fu_count)
{
	struct reservation_station **rs_arr;
	int latency = fu_latencies[fu_type];

	 /* Allocate one big chunk of memory and divide it */
	struct fu_set *set = ecalloc(sizeof(*set) +
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

/* Inits all the fus */
void
exe_init(int fu0_count, int fu1_count, int fu2_count)
{
	all_fus[0] = create_fu_set(FU0, fu0_count);
	all_fus[1] = create_fu_set(FU1, fu1_count);
	all_fus[2] = create_fu_set(FU2, fu2_count);
}

void
exe_destroy()
{
	for (int i = 0; i < FU_TYPE_COUNT; ++i)
		free(all_fus[i]);
}

/* Execution pipeline stage. Updates all FUs */
void
execute()
{
	for (int i = 0; i < FU_TYPE_COUNT; ++i) {
		execute_fu_set(all_fus[i]);
	}
}
