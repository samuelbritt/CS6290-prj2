#ifndef STATS_H_
#define STATS_H_

void stats_init();
void stats_update(int clock);
void stats_destroy();

int stats_inst_fetched();
float stats_inst_fired_avg();
float stats_inst_fired_std_dev();
float stats_ipc_avg();
float stats_ipc_std_dev();
float stats_sched_queue_size_avg();
int stats_sched_queue_size_max();

#endif /* end of include guard: STATS_H_ */
