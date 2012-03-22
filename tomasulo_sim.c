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

	disp_init();
	sched_init();
	exe_init(opt->fu0_count, opt->fu1_count, opt->fu2_count);

	int clock = 0;
	do {
		vlog("\n----- Cycle %d -----\n", clock);
		/* state_update(); */
		execute();
		schedule(cdbs, opt->cdb_count);
		dispatch(reg_file);
		instruction_fetch(opt->trace_file, opt->fetch_rate);
		clock++;
	} while (!disp_queue_is_empty() ||
		 !sched_queue_is_empty() ||
		 0
		 );
	vlog("Simulation complete.\n");

	disp_destroy();
	sched_destroy();
	exe_destroy();
}
