#include "state_update.h"
#include "schedule.h"
#include "execute.h"
#include "logger.h"
#include "common.h"

#include "deque.h"

static struct {
	deque_t *queue;
	int cdb_total_count;
	int cdb_use_count;
} su_state;

static int
update_sched_queue(void *completed_rs_)
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
	for (int i = 0; i < su_state.cdb_total_count; ++i) {
		struct reservation_station *rs = exe_retire_inst();
		if (!rs)
			return;
		vlog_inst(rs->fu_type, &rs->dest, rs->src, "SU");
		update_reg_file(rs, reg_file);
		update_sched_queue(rs);
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
