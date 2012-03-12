#ifndef TOMASULO_SIM_H_
#define TOMASULO_SIM_H_

/* Max number of source registers for any instruction */
#define SRC_REGISTER_COUNT 2

/* Number of architectural registers */
#define ARCH_REGISTER_COUNT 128

/* Number of instructions to fetch each cycle */
#define DEFAULT_FETCH_RATE 1

/* Number of CDBs */
#define DEFAULT_CDB_COUNT 1

/* Number of FUs of type 0, 1, 2 */
#define DEFAULT_FU0_COUNT 1
#define DEFAULT_FU1_COUNT 1
#define DEFAULT_FU2_COUNT 1

struct options {
	int fu0_count;
	int fu1_count;
	int fu2_count;
	int cdb_count;
	int fetch_rate;
	FILE *trace_file;
};

void tomasulo_sim(struct options *opt);

#endif /* end of include guard: TOMASULO_SIM_H_ */
