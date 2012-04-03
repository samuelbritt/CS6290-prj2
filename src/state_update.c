#include "state_update.h"
#include "schedule.h"
#include "logger.h"
#include "common.h"

#include "deque.h"

static struct {
	deque_t *queue;
	int cdb_total_count;
	int cdb_use_count;
} su_state;

/* Comparison function for sorted deque insert. Sorts based on dest tags */
static int
compare_func(void *a, void *b)
{
	int tag_a = ((struct reservation_station *) a)->dest.tag;
	int tag_b = ((struct reservation_station *) b)->dest.tag;
	return tag_a - tag_b;
}

/* Attempts to retire the instruction and return 0. If it can't (because all the
 * CDBs are busy), returns 1 */
int
su_retire_inst(struct reservation_station *rs)
{
	if (su_state.cdb_use_count < su_state.cdb_total_count) {
		deque_insert_sorted(su_state.queue, rs, compare_func);
		su_state.cdb_use_count++;
		return 0;
	}
	return 1;
}

static int
update_sched_queue(void *completed_rs_, void *arg)
{
	struct reservation_station *completed_rs = completed_rs_;

	struct cdb cdb;
	cdb.tag = completed_rs->dest.tag;
	cdb.reg_num = completed_rs->dest.index;
	cdb.val = 0;
	sched_broadcast_cdb(&cdb);
	sched_delete_rs(completed_rs);
	return DEQUE_CONTINUE;
}

static int
update_reg_file(void *rs_, void *reg_file_)
{
	struct reservation_station *rs = rs_;
	struct int_register *reg_file = reg_file_;

	vlog_inst(rs->fu_type, &rs->dest, rs->src, "SU");
	if (rs->dest.index >= 0) {
		struct int_register *reg = &reg_file[rs->dest.index];
		if (rs->dest.tag == reg->tag)
			reg->ready = true;
	}
	return DEQUE_CONTINUE;
}

void
state_update(struct int_register *reg_file)
{
	deque_foreach(su_state.queue, update_reg_file, reg_file);
	deque_foreach(su_state.queue, update_sched_queue, NULL);
	while (!deque_is_empty(su_state.queue)) {
		deque_delete_first(su_state.queue);
		su_state.cdb_use_count--;
	}
}

void su_destroy()
{
	deque_destroy(su_state.queue);
}

void su_init(int cdb_count)
{
	su_state.queue = deque_create();
	su_state.cdb_total_count = cdb_count;
	su_state.cdb_use_count = 0;
}
