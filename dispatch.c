#include "deque.h"
#include "logger.h"

#include "dispatch.h"
#include "common.h"

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
static struct reservation_station *
reservation_station_init(struct reservation_station *rs,
			 struct instruction *inst,
			 struct int_register reg_file[])
{
	rs->fu_type = inst->fu_type;
	rs->dest_reg_index = inst->dest_reg_num;
	rs->dest_reg_tag = inst->id;
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
	return rs;
}

/* dispatch logic for a single instruction. Returns a new
 * reservation_station. Destroy with free() */
static struct reservation_station *
dispatch_inst(struct instruction *inst, struct int_register reg_file[])
{
	vlog_inst(inst->id, "Dispatch");
	struct reservation_station *rs = ecalloc(sizeof(*rs));
	reservation_station_init(rs, inst, reg_file);
	if (rs->dest_reg_index >= 0) {
		reg_file[rs->dest_reg_index].tag = rs->dest_reg_tag;
		reg_file[rs->dest_reg_index].ready = false;
	}
	return rs;
}

/* Dispatches instructions to the scheduler */
void
dispatch(deque_t *dispatch_queue, struct int_register reg_file[],
	 deque_t *sched_queue)
{
	struct instruction *inst;
	struct reservation_station *rs;
	while (!deque_is_empty(dispatch_queue)) {
		inst = deque_delete_first(dispatch_queue);
		rs = dispatch_inst(inst, reg_file);
		deque_append(sched_queue, rs);
		free(inst);
	}
}
