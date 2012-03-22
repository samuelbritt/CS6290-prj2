#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "deque.h"

#include "tomasulo_sim.h"
#include "schedule.h"
#include "execute.h"
#include "common.h"
#include "logger.h"

/* Counter for the number of fetched instructions. Useful for tags */
static int instruction_count = 1;

struct instruction {
	int id;
	void *addr;
	int fu_type;
	int dest_reg_num;
	int src_reg_num[SRC_REGISTER_COUNT];
};

/* Fetches `fetch_rate` instructions and puts them in the dispatch queue */
static void
instruction_fetch(FILE *trace_file, int fetch_rate, deque_t *dispatch_queue)
{
	int instructions_fetched = 0;
	while ((instructions_fetched++ < fetch_rate) && !feof(trace_file)) {
		struct instruction *inst = emalloc(sizeof(*inst));
		int n = fscanf(trace_file, "%p %d %d %d %d\n", &inst->addr,
			       &inst->fu_type, &inst->dest_reg_num,
			       &inst->src_reg_num[0], &inst->src_reg_num[1]);
		if (n < 5)
			fail("Invalid instruction read\n");
		inst->id = instruction_count++;
		vlog("Adding instruction %d (%p %1d %2d %2d %2d) to dispatch queue.\n",
		     inst->id, inst->addr, inst->fu_type, inst->dest_reg_num,
		     inst->src_reg_num[0], inst->src_reg_num[1]);
		deque_append(dispatch_queue, inst);
	}
}

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
	vlog("Dispatching instruction %d\n", inst->id);
	struct reservation_station *rs = ecalloc(sizeof(*rs));
	reservation_station_init(rs, inst, reg_file);
	if (rs->dest_reg_index >= 0) {
		reg_file[rs->dest_reg_index].tag = rs->dest_reg_tag;
		reg_file[rs->dest_reg_index].ready = false;
	}
	return rs;
}

/* Dispatches instructions to the scheduler */
static void
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

/* Writes results */
static void
state_update()
{
	exit(1);	/* TODO */
}

/* Runs the actual tomasulo pipeline*/
void
tomasulo_sim(const struct options * const opt)
{
	struct int_register reg_file[ARCH_REGISTER_COUNT];
	for (int i = 0; i < ARCH_REGISTER_COUNT; ++i) {
		reg_file[i].ready = true;
		reg_file[i].tag = 0;
		reg_file[i].val = i;
	}

	struct cdb cdbs[opt->cdb_count];
	memset(cdbs, 0, sizeof(cdbs));
	for (int i = 0; i < opt->cdb_count; ++i) {
		cdbs[i].tag = -1;
	}

	struct fu_set *fu0 = create_fu_set(FU0, opt->fu0_count);
	struct fu_set *fu1 = create_fu_set(FU1, opt->fu1_count);
	struct fu_set *fu2 = create_fu_set(FU2, opt->fu2_count);
	struct fu_set *fus[] = {fu0, fu1, fu2};

	deque_t *dispatch_queue = deque_create();
	deque_t *sched_queue = deque_create();
	deque_t *exe_queue = deque_create();

	int clock = 0;
	do {
		/* state_update(); */
		execute(exe_queue, sched_queue, fus);
		schedule(sched_queue, cdbs, opt->cdb_count, fus, exe_queue);
		dispatch(dispatch_queue, reg_file, sched_queue);
		instruction_fetch(opt->trace_file, opt->fetch_rate,
				  dispatch_queue);
		clock++;
	} while (!deque_is_empty(dispatch_queue) ||
		 !deque_is_empty(sched_queue) ||
		 !deque_is_empty(exe_queue) ||
		 0
		 );
	vlog("Simulation complete.\n");

	deque_destroy(dispatch_queue);
	deque_destroy(sched_queue);
	deque_destroy(exe_queue);
}
