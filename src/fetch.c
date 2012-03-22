
#include "deque.h"
#include "logger.h"
#include "common.h"

#include "fetch.h"
#include "dispatch.h"

/* Counter for the number of fetched instructions. Useful for tags */
static int instruction_count = 1;

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
	vlog_inst(inst->id, "Fetch");
	return inst;
}

/* Fetches `fetch_rate` instructions and puts them in the dispatch queue */
void
instruction_fetch(FILE *trace_file, int fetch_rate)
{
	int instructions_fetched = 0;
	while ((instructions_fetched++ < fetch_rate) && !feof(trace_file)) {
		fetch_single_inst(trace_file);
	}
}
