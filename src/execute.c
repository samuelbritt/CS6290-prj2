/*
 * Module to group together functions related to the execution stage of the
 * pipeline and the functional units.
 */

#include "execute.h"
#include "schedule.h"
#include "state_update.h"
#include "logger.h"
#include "common.h"

#include "deque.h"

#include <stdio.h>
#include <stdlib.h>

/* A singular FU of a given type, with its pipeline */
struct func_unit {
	int latency;
	int type;
	bool waiting_for_retirement;
	struct reservation_station **pipeline;
};

/* Groups together all FUs of a particular type */
struct fu_set {
	int count;
	struct func_unit *fus;
};

/* Global array of all FUs */
static struct fu_set *all_fus[FU_TYPE_COUNT];

/* Global queue of fu's containing instructions waiting for retirement in
 * their final pipeline stage. Sorted by the waiting instruction's tag. */
static deque_t *waiting_fus;

/* Maps FU_TYPE to its latency */
static int fu_latencies[] = { 1, 2, 3 };

/* returns a pointer to the last rs in the pipeline */
static struct reservation_station **
last_pipeline_slot(struct func_unit *fu)
{
	return &fu->pipeline[fu->latency - 1];
}

/* Removes a completed instruction from the pipeline and returns it. If no
 * instructions are ready for retirement, returns NULL. */
struct reservation_station *
exe_retire_inst()
{
	struct func_unit *fu = deque_delete_first(waiting_fus);
	if (!fu)
		return NULL;

	fu->waiting_for_retirement = false;
	struct reservation_station **last_slot = last_pipeline_slot(fu);
	struct reservation_station *last_rs = *last_slot;
	*last_slot = NULL;
	return last_rs;
}

/* prints out the state of the FU, if in verbose mode */
static void
log_fu_state(struct func_unit *fu, int fu_number)
{
	struct reservation_station *rs;
	char *fmt = "EX(%d):%d";
	char stage[512];
	for (int i = fu->latency - 1; i >= 0; --i) {
		rs = fu->pipeline[i];
		if (rs) {
			sprintf(stage, fmt, i, fu_number);
			vlog_inst(rs->fu_type, &rs->dest, rs->src, stage);
		}
	}
}

/* comparison function for deque_insert_sorted. Sorts based on the tag of
 * the final reservation station in the fu pipeline */
static int
fu_compare(void *fu_a, void *fu_b)
{
	int tag_a = (*last_pipeline_slot((struct func_unit *) fu_a))->dest.tag;
	int tag_b = (*last_pipeline_slot((struct func_unit *) fu_b))->dest.tag;
	return tag_a - tag_b;
}

static void
update_waiting_fus(struct func_unit *fu)
{
	if (!fu->waiting_for_retirement && *last_pipeline_slot(fu) != NULL) {
		fu->waiting_for_retirement = true;
		deque_insert_sorted(waiting_fus, fu, fu_compare);
	}
}

/*  Pulls a new instruction from the scheduling queue if necessary to fill
 *  the pipeline */
static void
fill_fu_from_sched(struct func_unit *fu)
{
	if (fu->pipeline[0] == NULL)
		fu->pipeline[0] = sched_wakeup(fu->type);
}

/* pulls pipeline[i-1] into pipeline[i] if possible. */
static void
pull_from_prev(struct reservation_station **pipeline, int i)
{
	if (pipeline[i] != NULL)
		return;
	pipeline[i] = pipeline[i - 1];
	pipeline[i - 1] = NULL;
}

/* moves all instructions down the FU pipeline. */
static void
update_fu_pipeline(struct func_unit *fu)
{
	for (int i = fu->latency - 1; i > 0; --i)
		pull_from_prev(fu->pipeline, i);
}

static void
execute_fu(struct func_unit *fu)
{
	update_fu_pipeline(fu);
	fill_fu_from_sched(fu);
	update_waiting_fus(fu);
}

static void
execute_fu_set(struct fu_set *set)
{
	for (int i = 0; i < set->count; ++i) {
		execute_fu(&set->fus[i]);
		log_fu_state(&set->fus[i], i);
	}
}

/* Execution pipeline stage. Updates all FUs */
void
execute()
{
	for (int i = 0; i < FU_TYPE_COUNT; ++i) {
		execute_fu_set(all_fus[i]);
	}
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
		set->fus[i].type = fu_type;
		set->fus[i].waiting_for_retirement = false;
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
	waiting_fus = deque_create();
}

void
exe_destroy()
{
	for (int i = 0; i < FU_TYPE_COUNT; ++i)
		free(all_fus[i]);
	deque_destroy(waiting_fus);
}
