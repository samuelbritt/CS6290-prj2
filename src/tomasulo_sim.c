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
#include "state_update.h"
#include "common.h"
#include "logger.h"

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

	disp_init();
	sched_init();
	exe_init(opt->fu0_count, opt->fu1_count, opt->fu2_count);
	su_init(opt->cdb_count);

	int clock = 0;
	do {
		vlog("\n----- Cycle %d -----\n", clock);
		state_update(reg_file);
		execute();
		schedule();
		dispatch(reg_file);
		instruction_fetch(opt->trace_file, opt->fetch_rate);
		clock++;
	} while (((!opt->max_cycles || clock < opt->max_cycles)) &&
		 (!disp_queue_is_empty() || !sched_queue_is_empty()));

	if (opt->max_cycles && clock >= opt->max_cycles) {
		vlog("Error: Max number of cycles reached\n");
		ret = 1;
	} else {
		vlog("Simulation complete.\n");
	}

	disp_destroy();
	sched_destroy();
	exe_destroy();
	su_destroy();
	return ret;
}
