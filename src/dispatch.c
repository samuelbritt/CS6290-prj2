#include "dispatch.h"
#include "schedule.h"

#include "common.h"
#include "logger.h"
#include "deque.h"

static deque_t *disp_queue;

/* Initializes a reservation station for the given instruction. */
static void
reservation_station_init(struct reservation_station *rs,
			 struct instruction *inst,
			 struct int_register reg_file[])
{
	/* A dummy register that is always ready. Use for instructions that
	 * don't use a register. */
	struct int_register dummy = {
		.index = -1,
		.tag = -1,
		.ready = true,
	};

	rs->fu_type = inst->fu_type;
	rs->dest.index = inst->dest_reg_num;
	rs->dest.tag = inst->id;
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		int reg_index = inst->src_reg_num[i];
		if (reg_index >= 0) {
			rs->src[i] = reg_file[reg_index];
		} else {
			rs->src[i] = dummy;
		}
	}
}

/* dispatches a single instruction into the given reservation station */
static void
dispatch_inst(struct instruction *inst, struct int_register reg_file[],
	      struct reservation_station *rs)
{
	reservation_station_init(rs, inst, reg_file);
	vlog_inst(rs->fu_type, &rs->dest, rs->src, "DISPATCH");
	if (rs->dest.index >= 0) {
		reg_file[rs->dest.index].tag = rs->dest.tag;
		reg_file[rs->dest.index].ready = false;
	}
}

/* Dispatches instructions to the scheduler */
void
dispatch(struct int_register reg_file[])
{
	struct instruction *inst;
	struct reservation_station *rs;
	while (!deque_is_empty(disp_queue)) {
		inst = deque_delete_first(disp_queue);
		rs = sched_add_rs();
		dispatch_inst(inst, reg_file, rs);
		free(inst);
	}
}

/* Wrapper functions for deque */
struct instruction *
disp_add_inst()
{
	struct instruction *inst = ecalloc(sizeof(*inst));
	deque_append(disp_queue, inst);
	return inst;
}

bool
disp_queue_is_empty()
{
	return deque_is_empty(disp_queue);
}

void
disp_destroy()
{
	deque_destroy(disp_queue);
}

void
disp_init()
{
	disp_queue = deque_create();
}
