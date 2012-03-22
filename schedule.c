#include "deque.h"
#include "logger.h"

#include "schedule.h"
#include "common.h"

/* pulls data from cdb into reg if tags match */
static void
pull_if_tags_match(struct int_register *src, struct cdb *cdb)
{
	if (cdb->tag == src->tag) {
		vlog("Pulling register %d from cdb\n", src->tag);
		src->ready = true;
		src->val = cdb->val;
	}
}

/* Compares reg to all cdbs, and pulls the data in the tags match */
static void
pull_matching_tag(struct int_register *src, struct cdb *cdbs, int cdb_count)
{
	struct cdb *cdb;
	for (int i = 0; i < cdb_count; ++i) {
		cdb = &cdbs[i];
		pull_if_tags_match(src, cdb);
	}
}

/* wrapper argument for schedule_inst() */
struct sched_inst_arg {
	int cdb_count;
	struct cdb *cdbs;
	deque_t *exe_queue;
};

/* schedule logic for a single reservation station. Designed to be called
 * from deque_foreach() */
static void
schedule_inst(void *rs_, void *sched_arg_)
{
	struct reservation_station *rs = rs_;
	struct sched_inst_arg *arg = sched_arg_;
	int cdb_count = arg->cdb_count;
	struct cdb *cdbs = arg->cdbs;
	deque_t *exe_queue = arg->exe_queue;

	vlog("Scheduling instruction %d\n", rs->dest_reg_tag);
	struct int_register *src;
	bool all_sources_ready = true;
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		src = &rs->src[i];
		pull_matching_tag(src, cdbs, cdb_count);
		all_sources_ready = all_sources_ready && src->ready;
	}
	if (all_sources_ready /* TODO && FU is not busy */) {
		vlog("Firing instruction %d\n", rs->dest_reg_tag);
		deque_append(exe_queue, rs);
	}
}

/* Schedules instructions to be run */
void
schedule(deque_t *sched_queue, struct cdb *cdbs, int cdb_count,
	 struct fu_set *fus[], deque_t *exe_queue)
{
	struct sched_inst_arg arg;
	arg.cdb_count = cdb_count;
	arg.cdbs = cdbs;
	arg.exe_queue = exe_queue;
	deque_foreach(sched_queue, &schedule_inst, &arg);
}
