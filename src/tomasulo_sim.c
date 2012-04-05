#include "tomasulo_sim.h"

#include "fetch.h"
#include "dispatch.h"
#include "schedule.h"
#include "execute.h"
#include "state_update.h"
#include "logger.h"
#include "stats.h"
#include "common.h"

#include "deque.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Runs the actual tomasulo pipeline*/
int
tomasulo_sim(const struct options * const opt)
{
	int ret = 0;
	struct int_register reg_file[ARCH_REGISTER_COUNT];
	for (int i = 0; i < ARCH_REGISTER_COUNT; ++i) {
		reg_file[i].ready = true;
		reg_file[i].tag = -1;
		reg_file[i].val = i;
		reg_file[i].index = i;
	}

	vlog_init();
	disp_init();
	sched_init();
	exe_init(opt->fu0_count, opt->fu1_count, opt->fu2_count);
	stats_init();

	int clock = 0;
	do {
		vlog("Cycle %d\n", clock);
		state_update(opt->cdb_count, reg_file);
		execute();
		schedule();
		dispatch(reg_file);
		instruction_fetch(opt->trace_file, opt->fetch_rate);
		stats_update(clock++);
		vlog_flush();
		vlog("\n");
	} while (((!opt->max_cycles || clock < opt->max_cycles)) &&
		 (!disp_queue_is_empty() || !sched_queue_is_empty()));

	if (opt->max_cycles && clock >= opt->max_cycles) {
		fprintf(stderr, "Error: Max number of cycles reached\n");
		ret = 1;
	} else {
		printf("---- Sim Finished ----\n");
	}

	printf("Trace Filename: %s\n", opt->trace_filename);
	printf("Fetch Rate: %d\n", opt->fetch_rate);
	printf("Result Buses: %d\n", opt->cdb_count);
	printf("k0_FUs: %d\n", opt->fu0_count);
	printf("k1_FUs: %d\n", opt->fu1_count);
	printf("k2_FUs: %d\n", opt->fu2_count);
	printf("\n");
	printf("Cycles: %d\n", clock);
	printf("Instructions: %d\n", stats_inst_fetched());
	printf("Insn Fired (avg): %g\n", stats_inst_fired_avg());
	printf("Insn Fired (sdev): %g\n", stats_inst_fired_std_dev());
	printf("IPC (avg): %g\n", stats_ipc_avg());
	printf("IPC (sdev): %g\n", stats_ipc_std_dev());
	printf("SchedQ Size (avg): %g\n", stats_sched_queue_size_avg());
	printf("SchedQ Size (max): %d\n", stats_sched_queue_size_max());

	vlog_destroy();
	disp_destroy();
	sched_destroy();
	exe_destroy();
	stats_destroy();
	return ret;
}
