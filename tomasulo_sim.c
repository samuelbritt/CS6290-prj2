#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <libgen.h>
#include <string.h>

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

/* Print information at each step */
static int verbose;

struct int_register {
	bool ready;
	int tag;
	int val;
};

struct cdb {
	int tag;
	int val;
	int reg_num;
	bool busy;
};

struct reservation_station {
	int fu_type;
	int dest_reg_num;
	int dest_reg_tag;
	struct int_register *src[SRC_REGISTER_COUNT];
};

struct instruction {
	void *addr;
	int fu_type;
	int dest_reg_num;
	int src1_reg_num;
	int src2_reg_num;
};

enum FU_TYPES {
	FU0,
	FU1,
	FU2,
	FU_TYPE_COUNT
};

struct func_unit {
	bool busy;
	int latency;
};

struct options {
	int fu0_count;
	int fu1_count;
	int fu2_count;
	int cdb_count;
	int fetch_rate;
	FILE *trace_file;
};

/* Fetches N instructions and puts them in the dispatch queueu */
static void instruction_fetch()
{
	exit(1);	/* TODO */
}

/* Dispatches instrcutions to the scheduler */
static void dispatch()
{
	exit(1);	/* TODO */
}

/* Schedules instructions to be run */
static void schedule()
{
	exit(1);	/* TODO */
}

/* Executes an instruction */
static void execute()
{
	exit(1);	/* TODO */
}

/* Writes results */
static void state_update()
{
	exit(1);	/* TODO */
}

/* Runs the actual tomosulo pipeline*/
static void tomasulo_sim()
{
	int clock = 0;
	while (1) {
		state_update();
		execute();
		schedule();
		dispatch();
		instruction_fetch();
		clock++;
	}
}

/* Prints usage information. Does not exit */
static void print_usage(FILE *fp, char *program_name)
{
	fprintf(fp, "\nUsage:  %s [OPTIONS] <fetch_rate> <trace_file>\n",
		program_name);
	fprintf(fp,
		"\n"
		"REQUIRED ARGS:\n"
		"    <fetch_rate>      Number of instructions to fetch per cycle\n"
		"    <trace_file>      File containing instructions to simulate\n"
		"\n"
		"OPTIONS:\n"
		"    -h --help         Display help and exit\n"
		"    -v --verbose      Print simulation progress to stdout\n"
		"    -c <n> --cdb=<n>  Run with <n> common data buses\n"
		"    --k0=<n>          Run with <n> instances of FU type 0\n"
		"    --k1=<n>          Run with <n> instances of FU type 1\n"
		"    --k2=<n>          Run with <n> instances of FU type 2\n"
		"\n");
}

static void process_args(int argc, char *const argv[],
			 struct options *opt)
{
	char *program_name = basename(strdup(argv[0]));

	char *short_opts = "0:1:2:c:vh";
	const struct option long_opts[] = {
		/* {name, has_arg, flag, val} */
		{"k0", required_argument, NULL, '0'},
		{"k1", required_argument, NULL, '1'},
		{"k2", required_argument, NULL, '2'},
		{"cdb", required_argument, NULL, 'c'},
		{"verbose", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	int option_index = 0;
	int c = 0;
	while ((c = getopt_long(argc, argv, short_opts, long_opts,
				&option_index)) != -1) {
		switch (c) {
			case '0':
				opt->fu0_count = atoi(optarg);
				break;
			case '1':
				opt->fu1_count = atoi(optarg);
				break;
			case '2':
				opt->fu2_count = atoi(optarg);
				break;
			case 'c':
				opt->cdb_count = atoi(optarg);
				break;
			case 'v':
				verbose = true;
				break;
			case 0:
				/* flags set by getopt_long() */
				break;
			case 'h':
				print_usage(stdout, program_name);
				exit(EXIT_SUCCESS);
				break;
			case '?':
				print_usage(stderr, program_name);
				exit(EXIT_FAILURE);
				break;
			default:
				abort();
		}
	}

	if ((argc - optind) != 2) {
		print_usage(stderr, program_name);
		exit(EXIT_FAILURE);
	}

	opt->fetch_rate = atoi(argv[optind++]);
	opt->trace_file = fopen(argv[optind++], "r");

	if (!(opt->fu0_count > 0 &&
	      opt->fu1_count > 0 &&
	      opt->fu2_count > 0 &&
	      opt->cdb_count > 0 &&
	      opt->fetch_rate > 0 &&
	      opt->trace_file != NULL)) {
		fprintf(stderr, "Error: invalid argument\n");
		print_usage(stderr, program_name);
		exit(EXIT_FAILURE);
	}

	if (verbose)
		printf("Running in verbose mode\n");
}

int main(int argc, char * const argv[])
{
	struct reservation_station *sched_queue;
	struct int_register reg_file[ARCH_REGISTER_COUNT];

	struct options opt  = {
		.fu0_count = DEFAULT_FU0_COUNT,
		.fu1_count = DEFAULT_FU1_COUNT,
		.fu2_count = DEFAULT_FU2_COUNT,
		.cdb_count = DEFAULT_CDB_COUNT,
		.fetch_rate = DEFAULT_FETCH_RATE,
		.trace_file = NULL,
	};
	process_args(argc, argv, &opt);

	struct cdb cdbs[opt.cdb_count];
	struct func_unit fu0[opt.fu0_count];
	struct func_unit fu1[opt.fu1_count];
	struct func_unit fu2[opt.fu2_count];
	struct func_unit *fus[] = {fu0, fu1, fu2};
	/* TODO set latencies. Set each fu type individually? Have a table
	 * that maps fu type to latency? Have separate structs for each fu
	 * type? */

	struct instruction inst;
	while (!feof(opt.trace_file)) {
		fscanf(opt.trace_file, "%p %d %d %d %d",
		       &inst.addr, &inst.fu_type, &inst.dest_reg_num,
		       &inst.src1_reg_num, &inst.src2_reg_num);
	}

	return 0;
}
