#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "deque.h"

#include "tomasulo_sim.h"
#include "fetch.h"
#include "dispatch.h"
#include "schedule.h"
#include "execute.h"
#include "common.h"
#include "logger.h"

struct cdb_set CDB_SET;

/* Writes results */
static void
state_update(struct int_register *reg_file)
{
	struct cdb *cdb;
	struct int_register *reg;
	for (int i = 0; i < CDB_SET.count; ++i) {
		cdb = &CDB_SET.cdbs[i];
		if (cdb->reg_num < 0) {
			cdb->busy = false;
			continue;
		}

		if (cdb->busy) {
			cdb->busy = false;
			reg = &reg_file[cdb->reg_num];
			if (cdb->tag == reg->tag) {
				reg->val = cdb->val;
				reg->ready = true;
			}
		}
	}
}

/* Runs the actual tomasulo pipeline*/
int
tomasulo_sim(const struct options * const opt)
{
	int ret = 0;
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
	CDB_SET.count = opt->cdb_count;
	CDB_SET.cdbs = cdbs;

	disp_init();
	sched_init();
	exe_init(opt->fu0_count, opt->fu1_count, opt->fu2_count);

	int clock = 0;
	do {
		vlog("\n----- Cycle %d -----\n", clock);
		state_update(reg_file);
		execute();
		schedule();
		dispatch(reg_file);
		instruction_fetch(opt->trace_file, opt->fetch_rate);
		clock++;
	} while (((opt->max_cycles && clock < opt->max_cycles)) &&
		 (!disp_queue_is_empty() ||
		  !sched_queue_is_empty())
		 );

	if (opt->max_cycles && clock >= opt->max_cycles) {
		vlog("Error: Max number of cycles reached\n");
		ret = 1;
	}
	vlog("Simulation complete.\n");

	disp_destroy();
	sched_destroy();
	exe_destroy();
	return ret;
}
