#include "deque.h"
#include "logger.h"
#include "common.h"

#include "dispatch.h"
#include "schedule.h"

deque_t *disp_queue;

/* Inits a reservation station source slot given the state of the corresponding
 * register in the reg_file */
static void
rs_src_init(struct int_register *rs_src, struct int_register *reg_file_reg)
{
	if (reg_file_reg->ready) {
		rs_src->val = reg_file_reg->val;
		rs_src->ready = true;
	} else {
		rs_src->tag = reg_file_reg->tag;
		rs_src->ready = false;
	}
}

/* Initializes a reservation station for the given instruction. */
static void
reservation_station_init(struct reservation_station *rs,
			 struct instruction *inst,
			 struct int_register reg_file[])
{
	rs->fu_type = inst->fu_type;
	rs->dest.index = inst->dest_reg_num;
	rs->dest.tag = inst->id;
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		int reg_index = inst->src_reg_num[i];
		if (reg_index >= 0) {
			rs_src_init(&rs->src[i], &reg_file[reg_index]);
		} else {
			/* the instruction doesn't use this register, so it
			 * is ready regardless of its value */
			rs->src[i].ready = true;
		}
	}
}

/* dispatches a single instruction into the given reservation station */
static void
dispatch_inst(struct instruction *inst, struct int_register reg_file[],
	      struct reservation_station *rs)
{
	vlog_inst(inst->id, "Dispatch");
	reservation_station_init(rs, inst, reg_file);
	if (rs->dest.index >= 0) {
		reg_file[rs->dest.index].tag = rs->dest.tag;
		reg_file[rs->dest.index].ready = false;
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
disp_init()
{
	disp_queue = deque_create();
}
void
disp_destroy()
{
	deque_destroy(disp_queue);
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
