#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tomasulo_sim.h"
#include "common.h"
#include "deque.h"

/* Print information at each step */
int verbose = 0;

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
	int dest_reg_num;
	int dest_reg_tag;
	struct int_register *src[SRC_REGISTER_COUNT];
};

struct instruction {
	void *addr;
	int fu_type;
	int dest_reg_num;
	int src1_reg_num;
	int src2_reg_num;
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

/* Fetches N instructions and puts them in the dispatch queue */
static void instruction_fetch(FILE *trace_file, int fetch_rate)
{
	int instructions_fetched = 0;
	while ((instructions_fetched++ < fetch_rate) && !feof(trace_file)) {
		struct instruction *inst = emalloc(sizeof(*inst));
		fscanf(trace_file, "%p %d %d %d %d",
		       &inst->addr, &inst->fu_type, &inst->dest_reg_num,
		       &inst->src1_reg_num, &inst->src2_reg_num);
		/* TODO add inst to dispatch Q */
	}
}

/* Dispatches instrcutions to the scheduler */
static void dispatch()
{
	exit(1);	/* TODO */
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

	struct reservation_station *sched_queue;
	struct int_register reg_file[ARCH_REGISTER_COUNT];

	struct cdb cdbs[opt->cdb_count];
	struct func_unit fu0[opt->fu0_count];
	struct func_unit fu1[opt->fu1_count];
	struct func_unit fu2[opt->fu2_count];
	struct func_unit *fus[] = {fu0, fu1, fu2};
	/* TODO set latencies. Set each fu type individually? Have a table
	 * that maps fu type to latency? Have separate structs for each fu
	 * type? */

	int clock = 0;
	while (1) {
		state_update();
		execute();
		schedule();
		dispatch();
		instruction_fetch(opt->trace_file, opt->fetch_rate);
		clock++;
	}
}

