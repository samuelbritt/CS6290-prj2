#include "stats.h"

#include "common.h"
#include "schedule.h"

#include <stdlib.h>

static int max_clock;
static int array_size = 1024;
static int *cum_inst_fired;

void stats_init()
{
	cum_inst_fired = emalloc(array_size * sizeof(*cum_inst_fired));
}

void stats_destroy() {
	free(cum_inst_fired);
}

static void
resize_arrays()
{
	array_size *= 2;
	cum_inst_fired = realloc(cum_inst_fired, array_size);
}

static void
update_arrays(int clock)
{
	if (array_size < clock) {
		resize_arrays();
	}
	cum_inst_fired[clock] = sched_get_fired_instruction_count();
}

static void
init_arrays()
{
	cum_inst_fired[0] = sched_get_fired_instruction_count();
}

void stats_update(int clock) {
	max_clock = clock;
	if (clock == 0) {
		init_arrays();
	} else {
		update_arrays(clock);
	}
}

static int
sum(int *arr, int size)
{
	return arr[size-1];
}

static float
avg(int *arr, int size)
{
	if (!size)
		return 0;
	int sum_ = sum(arr, size);
	return sum_ / (float) size;
}

static float
std_dev(int *arr, int size)
{
	float mean = avg(arr, size);
	int variance[size];
	for (int i = 0; i < size; ++i) {
		float delta = x - mean;
		variance[i] = 
	}

}


int stats_inst_fetched() {
	// TODO change this to something better
	return sum(cum_inst_fired, max_clock + 1);
}
float stats_inst_fired_avg() {
	return avg(cum_inst_fired, max_clock + 1);
}
float stats_inst_fired_std_dev();
float stats_ipc_avg();
float stats_ipc_std_dev();
float stats_sched_queue_size_avg();
int stats_sched_queue_size_max();
