#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include "tomasulo_sim.h"
#include "common.h"
#include "deque.h"

/* Print information at each step */
int verbose = 0;

/* Counter for the number of fetched instructions. Useful for tags */
static int instruction_count = 0;

struct int_register {
	bool ready;
	int tag;
	int val;
};

struct cdb {
	int tag;
	int val;
	int reg_num;
	bool busy;
};

struct reservation_station {
	int fu_type;
	int dest_reg_index;  /* index into reg_file */
	int dest_reg_tag;
	struct int_register src[SRC_REGISTER_COUNT];
};

struct instruction {
	int id;
	void *addr;
	int fu_type;
	int dest_reg_num;
	int src_reg_num[SRC_REGISTER_COUNT];
};

enum FU_TYPES {
	FU0,
	FU1,
	FU2,
	FU_TYPE_COUNT
};

struct func_unit {
	bool busy;
	int latency;
};

/* Logs to stdout if `verbose` is set */
static void vlog(const char *format, ...)
{
	if (!verbose)
		return;
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
}

/* Fetches `fetch_rate` instructions and puts them in the dispatch queue */
static void instruction_fetch(FILE *trace_file, int fetch_rate,
			      deque_t *dispatch_queue)
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

/* alloc's and initializes a reservation station for the given instruction.
 * Destroy with free() */
static struct reservation_station *
reservation_station_create(struct instruction *i)
{
	struct reservation_station *rs = ecalloc(sizeof(*rs));
	rs->fu_type = i->fu_type;
	rs->dest_reg_index = i->dest_reg_num;
	return rs;
}

/* dispatch logic for a single instruction */
static struct reservation_station *
dispatch_inst(struct instruction *inst, struct int_register reg_file[])
{
	struct reservation_station *rs = reservation_station_create(inst);
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		int reg_index = inst->src_reg_num[i];
		if (reg_file[i].ready) {
			rs->src[i].val = reg_file[reg_index].val;
			rs->src[i].ready = true;
		} else {
			rs->src[i].tag = reg_file[reg_index].tag;
			rs->src[i].ready = false;
		}
	}
	int tag = inst->id;
	rs->dest_reg_tag = tag;
	reg_file[inst->dest_reg_num].tag = tag;
	reg_file[inst->dest_reg_num].ready = false;
	return rs;
}

/* Dispatches instructions to the scheduler */
static void dispatch(deque_t *dispatch_queue, struct int_register reg_file[],
		     deque_t *sched_queue)
{
	while (!deque_is_empty(dispatch_queue)) {
		struct instruction *inst = deque_delete_first(dispatch_queue);
		vlog("Dispatching");
		struct reservation_station *rs = dispatch_inst(inst, reg_file);
		deque_append(sched_queue, rs);
		free(inst);
	}
}

/* Schedules instructions to be run */
static void schedule()
{
	exit(1);	/* TODO */
}

/* Executes an instruction */
static void execute()
{
	exit(1);	/* TODO */
}

/* Writes results */
static void state_update()
{
	exit(1);	/* TODO */
}

/* Runs the actual tomasulo pipeline*/
void tomasulo_sim(struct options *opt)
{
	struct int_register reg_file[ARCH_REGISTER_COUNT];
	struct cdb cdbs[opt->cdb_count];
	struct func_unit fu0[opt->fu0_count];
	struct func_unit fu1[opt->fu1_count];
	struct func_unit fu2[opt->fu2_count];
	struct func_unit *fus[] = {fu0, fu1, fu2};
	/* TODO set latencies. Set each fu type individually? Have a table
	 * that maps fu type to latency? Have separate structs for each fu
	 * type? */

	deque_t *dispatch_queue = deque_create();
	deque_t *sched_queue = deque_create();

	int clock = 0;
	do {
		/* state_update(); */
		/* execute(); */
		/* schedule(); */
		dispatch(dispatch_queue, reg_file, sched_queue);
		instruction_fetch(opt->trace_file, opt->fetch_rate,
				  dispatch_queue);
		clock++;
	} while (!(deque_is_empty(dispatch_queue) &&
		   deque_is_empty(sched_queue)));
	vlog("Simulation complete.\n");
}

