#include "fetch.h"
#include "dispatch.h"
#include "common.h"
#include "logger.h"

#include "deque.h"

#include <stdio.h>

/* Counter for the number of fetched instructions. Useful for tags */
static int instruction_count = 0;

/* logs the fetch stage, if verbose is set */
static void
log_fetch(struct instruction *inst)
{
	struct int_register dest;
	dest.index = inst->dest_reg_num;
	dest.tag = -1;

	struct int_register srcs[SRC_REGISTER_COUNT];
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		srcs[i].index = inst->src_reg_num[i];
		srcs[i].tag = -1;
	}

	vlog_inst(inst->fu_type, &dest, srcs, "FETCH");
}

static struct instruction *
fetch_single_inst(FILE *trace_file)
{
	struct instruction *inst = disp_add_inst();
	int n = fscanf(trace_file, "%p %d %d %d %d\n", &inst->addr,
		       &inst->fu_type, &inst->dest_reg_num,
		       &inst->src_reg_num[0], &inst->src_reg_num[1]);
	if (n < 5)
		fail("Invalid instruction read\n");
	inst->id = instruction_count++;
	log_fetch(inst);
	return inst;
}

/* Fetches `fetch_rate` instructions and puts them in the dispatch queue.
 * Returns the number of instructions successfully fetched. */
int
instruction_fetch(FILE *trace_file, int fetch_rate)
{
	int instructions_fetched = 0;
	while ((instructions_fetched < fetch_rate) && !feof(trace_file)) {
		fetch_single_inst(trace_file);
		instructions_fetched++;
	}
	return instructions_fetched;
}
