#include "stats.h"

#include "common.h"
#include "schedule.h"

#include <stdlib.h>
#include <math.h>

static int max_clock;
static int array_size;
static int *inst_fired;
static int *inst_retired;
static int *sched_queue_size;

void
stats_init()
{
	array_size = 1024;
	inst_fired = emalloc(array_size * sizeof(*inst_fired));
	inst_retired = emalloc(array_size * sizeof(*inst_retired));
	sched_queue_size = emalloc(array_size * sizeof(*sched_queue_size));
}

void
stats_destroy()
{
	free(inst_fired);
	free(inst_retired);
	free(sched_queue_size);
}

static void
update_arrays(int clock)
{
	max_clock = clock;
	inst_fired[clock] = sched_fired_instruction_counter();
	sched_fired_instruction_counter_clear();

	inst_retired[clock] = sched_retired_instruction_counter();
	sched_retired_instruction_counter_clear();

	sched_queue_size[clock] = sched_queue_current_size();
}

static void
ensure_min_array_size(int min_size)
{
	if (array_size >= min_size)
		return;

	while (array_size < min_size)
		array_size *= 2;

	inst_fired = erealloc(inst_fired, array_size * sizeof(*inst_fired));
	inst_retired = erealloc(inst_retired,
				array_size * sizeof(*inst_retired));
	sched_queue_size = erealloc(sched_queue_size,
				    array_size * sizeof(*sched_queue_size));
}

void
stats_update(int clock)
{
	ensure_min_array_size(clock + 1);
	update_arrays(clock);
}

static int
max(int *arr, int size)
{
#define MAX(a, b) (a) > (b) ? (a) : (b)
	int max = arr[0];
	for (int i = 1; i < size; ++i)
		max = MAX(max, arr[i]);
	return max;
#undef MAX
}

static int
sum(int *arr, int size)
{
	int sum_ = 0;
	for (int i = 0; i < size; ++i)
		sum_ += arr[i];
	return sum_;
}

static float
fsum(float *arr, int size)
{
	float sum_ = 0.0;
	for (int i = 0; i < size; ++i)
		sum_ += arr[i];
	return sum_;
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
favg(float *arr, int size)
{
	if (!size)
		return 0;
	float sum_ = fsum(arr, size);
	return sum_ / (float) size;
}

static float
std_dev(int *arr, int size)
{
	float mean = avg(arr, size);
	float variance_arr[size];
	for (int i = 0; i < size; ++i) {
		float delta = arr[i] - mean;
		variance_arr[i] = delta * delta;
	}
	float variance = favg(variance_arr, size);
	return sqrt(variance);
}

int stats_inst_fetched() {
	return sum(inst_fired, max_clock + 1);
}
float stats_inst_fired_avg() {
	return avg(inst_fired, max_clock + 1);
}
float stats_inst_fired_std_dev() {
	return std_dev(inst_fired, max_clock + 1);
}
float stats_ipc_avg() {
	return avg(inst_retired, max_clock + 1);
}
float stats_ipc_std_dev() {
	return std_dev(inst_retired, max_clock + 1);
}
float stats_sched_queue_size_avg() {
	return avg(sched_queue_size, max_clock + 1);
}
int stats_sched_queue_size_max() {
	return max(sched_queue_size, max_clock + 1);
}
