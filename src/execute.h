/*
 * Module to group together functions related to the execution stage of the
 * pipeline and the functional units.
 */

#ifndef EXECUTE_H_
#define EXECUTE_H_

#include "deque.h"

struct reservation_station;

enum FU_TYPES {
	FU0,
	FU1,
	FU2,
	FU_TYPE_COUNT
};

/* Creates and initializes all the FUs */
void exe_init(int fu0_count, int fu1_count, int fu2_count);
/* Frees memory used by the FUs */
void exe_destroy();

/* Removes a completed instruction from the pipeline and returns it. If no
 * instructions are ready for retirement, returns NULL. */
struct reservation_station *exe_retire_inst();

/* Execution pipeline stage. Updates all FUs */
void execute();

#endif /* end of include guard: EXECUTE_H_ */
