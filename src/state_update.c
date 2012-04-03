#include "state_update.h"
#include "schedule.h"
#include "execute.h"
#include "logger.h"
#include "common.h"

#include "deque.h"

static void
update_sched_queue(struct reservation_station *completed_rs)
{
	struct cdb cdb;
	cdb.tag = completed_rs->dest.tag;
	cdb.reg_num = completed_rs->dest.index;
	cdb.val = 0;
	sched_broadcast_cdb(&cdb);
	sched_delete_rs(completed_rs);
}

static void
update_reg_file(struct reservation_station *rs, struct int_register *reg_file)
{
	if (rs->dest.index >= 0) {
		struct int_register *reg = &reg_file[rs->dest.index];
		if (rs->dest.tag == reg->tag)
			reg->ready = true;
	}
}

void
state_update(int cdb_count, struct int_register *reg_file)
{
	for (int i = 0; i < cdb_count; ++i) {
		struct reservation_station *rs = exe_retire_inst();
		if (!rs)
			return;
		vlog_inst(rs->fu_type, &rs->dest, rs->src, "SU");
		update_reg_file(rs, reg_file);
		update_sched_queue(rs);
	}
}
