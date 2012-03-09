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

static void process_options(int argc, char *const argv[])
{
	int k0 = 0;
	int k1 = 1;
	int k2 = 2;
	int cdb_count = 1;

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
				k0 = atoi(optarg);
				break;
			case '1':
				k1 = atoi(optarg);
				break;
			case '2':
				k2 = atoi(optarg);
				break;
			case 'c':
				cdb_count = atoi(optarg);
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

	if (verbose)
		printf("Running in verbose mode\n");
	printf("ks: %d, %d, %d\n", k0, k1, k2);
	printf("cdbs: %d\n", cdb_count);

	printf("args: ");
	while (optind < argc) {
		printf("%s ", argv[optind++]);
	}
	printf("\n");
}

int main(int argc, char * const argv[])
{
	struct reservation_station *sched_queue;
	struct int_register reg_file[ARCH_REGISTER_COUNT];

	struct cdb cdb_count[DEFAULT_CDB_COUNT];

	struct func_unit fu0[DEFAULT_FU0_COUNT];
	struct func_unit fu1[DEFAULT_FU1_COUNT];
	struct func_unit fu2[DEFAULT_FU2_COUNT];
	struct func_unit *fus[] = {fu0, fu1, fu2};

	process_options(argc, argv);

	printf("Hello world\n");
	return 0;
}
