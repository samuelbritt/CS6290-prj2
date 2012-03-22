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

	struct fu_set *fu0 = create_fu_set(FU0, opt->fu0_count);
	struct fu_set *fu1 = create_fu_set(FU1, opt->fu1_count);
	struct fu_set *fu2 = create_fu_set(FU2, opt->fu2_count);
	struct fu_set *fus[] = {fu0, fu1, fu2};

	disp_init();
	sched_init();

	int clock = 0;
	do {
		vlog("\n----- Cycle %d -----\n", clock);
		/* state_update(); */
		execute(fus);
		schedule(cdbs, opt->cdb_count, fus);
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
}
