#include "deque.h"
#include "logger.h"
#include "common.h"

#include "state_update.h"
#include "schedule.h"

struct {
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
	if (tag_a > tag_b)
		return 1;
	if (tag_a < tag_b)
		return -1;
	return 0;
}

/* Attempts to retire the instruction and return 0. If it can't (because all the
 * CDBs are busy), returns 1 */
int
retire_inst(struct reservation_station *rs)
{
	if (su_state.cdb_use_count < su_state.cdb_total_count) {
		deque_insert_sorted(su_state.queue, rs, compare_func);
		su_state.cdb_use_count++;
		return 0;
	}
	return 1;
}

void su_init(int cdb_count)
{
	su_state.queue = deque_create();
	su_state.cdb_total_count = cdb_count;
	su_state.cdb_use_count = 0;
}

void su_destroy()
{
	deque_destroy(su_state.queue);
}

static void
update_reg_file(void *rs_, void *reg_file_)
{
	struct reservation_station *rs = rs_;
	struct int_register *reg_file = reg_file_;

	vlog_inst(rs->dest.tag, "State Update");
	if (rs->dest.index < 0)
		return;
	struct int_register *reg = &reg_file[rs->dest.index];
	if (rs->dest.tag == reg->tag) {
		vlog_inst(rs->dest.tag, "Update Dest Register");
		reg->ready = true;
	}
}

static void
update_sched_queue(void *completed_rs_, void *arg)
{
	struct reservation_station *completed_rs = completed_rs_;

	struct cdb cdb;
	cdb.tag = completed_rs->dest.tag;
	cdb.reg_num = completed_rs->dest.index;
	cdb.val = 0;
	sched_broadcast_cdb(&cdb);
	sched_delete_rs(completed_rs);
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

