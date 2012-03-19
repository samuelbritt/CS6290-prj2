#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "tomasulo_sim.h"
#include "common.h"
#include "deque.h"

/* Print information at each step */
int verbose = 0;

/* Counter for the number of fetched instructions. Useful for tags */
static int instruction_count = 1;

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
static void
vlog(const char *format, ...)
{
	if (!verbose)
		return;
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
}

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

/* pulls data from cdb into reg if tags match */
static void
pull_if_tags_match(struct int_register *src, struct cdb *cdb)
{
	if (cdb->tag == src->tag) {
		vlog("Pulling register %d from cdb\n", src->tag);
		src->ready = true;
		src->val = cdb->val;
	}
}

/* Compares reg to all cdbs, and pulls the data in the tags match */
static void
pull_matching_tag(struct int_register *src, struct cdb *cdbs, int cdb_count)
{
	struct cdb *cdb;
	for (int i = 0; i < cdb_count; ++i) {
		cdb = &cdbs[i];
		pull_if_tags_match(src, cdb);
	}
}

/* wrapper argument for schedule_inst() */
struct sched_inst_arg {
	int cdb_count;
	struct cdb *cdbs;
	deque_t *exe_queue;
};

/* schedule logic for a single reservation station. Designed to be called
 * from deque_foreach() */
static void
schedule_inst(void *rs_, void *sched_arg_)
{
	struct reservation_station *rs = rs_;
	struct sched_inst_arg *arg = sched_arg_;
	int cdb_count = arg->cdb_count;
	struct cdb *cdbs = arg->cdbs;
	deque_t *exe_queue = arg->exe_queue;

	vlog("Scheduling instruction %d\n", rs->dest_reg_tag);
	struct int_register *src;
	bool all_sources_ready = true;
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		src = &rs->src[i];
		pull_matching_tag(src, cdbs, cdb_count);
		all_sources_ready = all_sources_ready && src->ready;
	}
	if (all_sources_ready /* TODO && FU is not busy */) {
		vlog("Firing instruction %d\n", rs->dest_reg_tag);
		deque_append(exe_queue, rs);
	}
}

/* Schedules instructions to be run */
static void
schedule(deque_t *sched_queue, struct cdb *cdbs, int cdb_count,
	 deque_t *exe_queue)
{
	struct sched_inst_arg arg;
	arg.cdb_count = cdb_count;
	arg.cdbs = cdbs;
	arg.exe_queue = exe_queue;
	deque_foreach(sched_queue, &schedule_inst, &arg);
}

/* Executes an instruction */
static void
execute(deque_t *exe_queue, deque_t *sched_queue)
{
	while (!deque_is_empty(exe_queue)) {
		struct reservation_station *rs = deque_delete_first(exe_queue);
		deque_delete(sched_queue, rs);
		free(rs);
	}

}

/* Writes results */
static void
state_update()
{
	exit(1);	/* TODO */
}

/* wraps free() so you can pass into deque_foreach() */
static void
free_wrap(void *data, void *null)
{
	free(data);
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

	struct func_unit fu0[opt->fu0_count];
	memset(fu0, 0, sizeof(fu0));
	struct func_unit fu1[opt->fu1_count];
	memset(fu1, 0, sizeof(fu1));
	struct func_unit fu2[opt->fu2_count];
	memset(fu2, 0, sizeof(fu2));

	struct func_unit *fus[] = {fu0, fu1, fu2};
	/* TODO set latencies. Set each fu type individually? Have a table
	 * that maps fu type to latency? Have separate structs for each fu
	 * type? */

	deque_t *dispatch_queue = deque_create();
	deque_t *sched_queue = deque_create();
	deque_t *exe_queue = deque_create();

	int clock = 0;
	do {
		/* state_update(); */
		execute(exe_queue, sched_queue);
		schedule(sched_queue, cdbs, opt->cdb_count, exe_queue);
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
