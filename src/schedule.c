#include "schedule.h"
#include "execute.h"
#include "common.h"
#include "logger.h"

#include "deque.h"

/* Maintain both the official scheduling queue, and a queue to hold
 * instructions coming in from DISP that have yet to be scheduled. Moving
 * instructions from the temporary unscheduled queue to the scheduling queue
 * during the SCHED pipeline stage ensures that all instructions spend at least
 * one cycle in SCHED. */
static deque_t *sched_queue;
static deque_t *unsched_queue;

static void
update_rs_from_cdb(void *rs_, void *cdb_)
{
	struct reservation_station *rs = rs_;
	struct cdb *cdb = cdb_;

	struct int_register *src;
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		src = &rs->src[i];
		if (!src->ready && cdb->tag == src->tag) {
			src->val = cdb->val;
			src->ready = true;
		}
	}
}

/* Broadcasts the cdb to all the reservation stations, updating their source
 * registers if the tags match */
void
sched_broadcast_cdb(struct cdb *cdb)
{
	deque_foreach(sched_queue, update_rs_from_cdb, cdb);
}

/* returns true if all src values are ready for the given rs */
static bool
all_sources_ready(struct reservation_station *rs)
{
	bool ready = true;
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		ready = ready && rs->src[i].ready;
	}
	return ready;
}

/* schedule logic for a single reservation station. Designed to be called
 * from deque_foreach() */
static void
schedule_inst(void *rs_, void *arg)
{
	struct reservation_station *rs = rs_;

	if (rs->fired)
		return;
	vlog_inst(rs->fu_type, &rs->dest, rs->src, "SCHED");
	if (all_sources_ready(rs) && !exe_issue_instruction(rs))
		rs->fired = true;
}

static void
tranfer_unsched_to_sched()
{
	struct reservation_station *unsched_rs;
	while (!deque_is_empty(unsched_queue)) {
		unsched_rs = deque_delete_first(unsched_queue);
		deque_append(sched_queue, unsched_rs);
	}
}

/* Schedules instructions to be run */
void
schedule()
{
	tranfer_unsched_to_sched();
	deque_foreach(sched_queue, &schedule_inst, NULL);
}

/* Wrapper functions for deque */
struct reservation_station *
sched_add_rs()
{
	struct reservation_station *rs = ecalloc(sizeof(*rs));
	deque_append(unsched_queue, rs);
	return rs;
}

void
sched_delete_rs(struct reservation_station *rs)
{
	rs = deque_delete(sched_queue, rs);
	free(rs);
}
bool
sched_queue_is_empty()
{
	return deque_is_empty(sched_queue);
}

void
sched_destroy()
{
	deque_destroy(sched_queue);
}

void
sched_init()
{
	sched_queue = deque_create();
	unsched_queue = deque_create();
}
