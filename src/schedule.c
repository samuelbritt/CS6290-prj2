#include "schedule.h"
#include "common.h"
#include "logger.h"

#include "deque.h"

static deque_t *sched_queue;

static int
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
	return DEQUE_CONTINUE;
}

/* Broadcasts the cdb to all the reservation stations, updating their source
 * registers if the tags match */
void
sched_broadcast_cdb(struct cdb *cdb)
{
	deque_foreach(sched_queue, update_rs_from_cdb, cdb);
}

static bool
all_sources_ready(struct reservation_station *rs)
{
	bool ready = true;
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		ready = ready && rs->src[i].ready;
	}
	return ready;
}

/* Struct used as both input and output argument for find_eligible_rs(). */
struct find_executable_rs_arg {
	int fu_type;
	struct reservation_station *executable_rs;
};

/* If the reservation station `rs_` is eligible for execution on the fu_type
 * specified in `arg_`, which is of type struct find_executable_rs_arg, returns
 * the reservation station in `arg_->eligible_rs`. Designed to be called from
 * deque_foreach(). */
static int
find_executable_rs(void *rs_, void *arg_)
{
	struct reservation_station *rs = rs_;
	struct find_executable_rs_arg *arg = arg_;
	if (!rs->fired && rs->ready_to_fire && rs->fu_type == arg->fu_type) {
		rs->fired = true;
		arg->executable_rs = rs;
		return DEQUE_STOP;
	}
	return DEQUE_CONTINUE;
}

/* Wakes up an instruction eligible for execution on the given fu type and
 * returns it. If there is no such instruction, returns NULL */
struct reservation_station *
sched_wakeup(int fu_type)
{
	struct find_executable_rs_arg arg;
	arg.fu_type = fu_type;
	arg.executable_rs = NULL;
	deque_foreach(sched_queue, find_executable_rs, &arg);
	return arg.executable_rs;
}

/* schedule logic for a single reservation station. Designed to be called
 * from deque_foreach() */
static int
sched_inst(void *rs_, void *arg)
{
	struct reservation_station *rs = rs_;
	if (!rs->fired) {
		vlog_inst(rs->fu_type, &rs->dest, rs->src, "SCHED");
		rs->ready_to_fire = all_sources_ready(rs);
	}
	return DEQUE_CONTINUE;
}

/* Schedules instructions to be run */
void
schedule()
{
	/* tranfer_unsched_to_sched(); */
	deque_foreach(sched_queue, &sched_inst, NULL);
}

/* Wrapper functions for deque */
struct reservation_station *
sched_add_rs()
{
	struct reservation_station *rs = ecalloc(sizeof(*rs));
	rs->ready_to_fire = false;
	deque_append(sched_queue, rs);
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
}
