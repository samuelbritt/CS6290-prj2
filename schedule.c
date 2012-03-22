#include "deque.h"
#include "logger.h"

#include "schedule.h"
#include "execute.h"
#include "common.h"

deque_t *sched_queue;

/* pulls data from cdb into reg if tags match */
static void
pull_if_tags_match(struct int_register *src, struct cdb *cdb)
{
	if (cdb->tag == src->tag) {
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

	if (rs->fired)
		return;
	vlog_inst(rs->dest_reg_tag, "Schedule");
	struct int_register *src;
	bool all_sources_ready = true;
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		src = &rs->src[i];
		pull_matching_tag(src, cdbs, cdb_count);
		all_sources_ready = all_sources_ready && src->ready;
	}
	if (all_sources_ready && !issue_instruction(rs)) {
		vlog_inst(rs->dest_reg_tag, "Fire");
		rs->fired = true;
	}
}

/* Wrapper functions for deque */
struct reservation_station *
sched_add_rs()
{
	struct reservation_station *rs = ecalloc(sizeof(*rs));
	deque_append(sched_queue, rs);
	return rs;
}
void
sched_delete_rs(struct reservation_station *rs)
{
	rs = deque_delete(sched_queue, rs);
	if (rs)
		free(rs);
}
bool
sched_queue_is_empty()
{
	return deque_is_empty(sched_queue);
}
void
sched_init()
{
	sched_queue = deque_create();
}
void
sched_destroy()
{
	deque_destroy(sched_queue);
}

/* Schedules instructions to be run */
void
schedule(struct cdb *cdbs, int cdb_count)
{
	struct sched_inst_arg arg;
	arg.cdb_count = cdb_count;
	arg.cdbs = cdbs;
	deque_foreach(sched_queue, &schedule_inst, &arg);
}

