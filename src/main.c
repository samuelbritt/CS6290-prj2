#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <libgen.h>
#include <string.h>

#include "tomasulo_sim.h"
#include "logger.h"

/* Prints usage information. Does not exit */
static void
print_usage(FILE *fp, char *program_name)
{
	fprintf(fp,
		"\n"
		"USAGE:   %s --help\n"
		"         %s [-v] [-m <max_cycles>] <fetch-rate> <k0> <k1> <k2> <cdb-count> <trace_file>\n"
		"         %s [OPTIONS] <trace_file>\n",
		program_name, program_name, program_name);
	fprintf(fp,
		"OPTIONS:\n"
		"    -h --help                  Display this help and exit\n"
		"    -v --verbose               Print simulation progress to stdout\n"
		"    -m <n> --max-cycles=<n>    Run at most <n> cycles of the simulation\n"
		"    -n <n> --fetch-rate=<n>    Fetch <n> instructions per cycle\n"
		"    -c <count> --cdb=<count>   Run with <count> common data buses\n"
		"    --k0=<count>               Run with <count> instances of FU type 0\n"
		"    --k1=<count>               Run with <count> instances of FU type 1\n"
		"    --k2=<count>               Run with <count> instances of FU type 2\n"
		"\n");
}

/* Process arguments passed to the simulation.
 *
 * We accept arguments in one of two ways. The first way is all positional
 * arguments, specifying everything in a specific order. Since this can be
 * difficult to read and remember, the second is by using named optional
 * arguments, and a single positional argument of the input trace. There is no
 * in-between: either all arguments have to be positional, or none of them can
 * be. If the positional form is used, any additional optional arguments will be
 * overridden. */
static void
process_args(int argc, char *const argv[], struct options *opt)
{
	char *arg0 = strdup(argv[0]);
	char *program_name = basename(arg0);

	char *short_opts = "0:1:2:c:n:m:vh";
	const struct option long_opts[] = {
		/* {name, has_arg, flag, val} */
		{"k0", required_argument, NULL, '0'},
		{"k1", required_argument, NULL, '1'},
		{"k2", required_argument, NULL, '2'},
		{"cdb", required_argument, NULL, 'c'},
		{"fetch-rate", required_argument, NULL, 'n'},
		{"max-cycles", required_argument, NULL, 'm'},
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
			case 'n':
				opt->fetch_rate = atoi(optarg);
				break;
			case 'm':
				opt->max_cycles = atoi(optarg);
				break;
			case 'v':
				VERBOSE = true;
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

	/* Accept either a single positional argument, or all positional
	 * arguments */
	int positional_argc = argc - optind;
	switch (positional_argc) {
		case 1:
			opt->trace_filename = argv[optind];
			break;
		case 6:
			opt->fu0_count      = atoi(argv[optind++]);
			opt->fu1_count      = atoi(argv[optind++]);
			opt->fu2_count      = atoi(argv[optind++]);
			opt->cdb_count      = atoi(argv[optind++]);
			opt->trace_filename = argv[optind];
			break;
		default:
			fprintf(stderr, "Error: invalid arguments\n");
			print_usage(stderr, program_name);
			exit(EXIT_FAILURE);
	}
	opt->trace_file = fopen(opt->trace_filename, "r");

	if (opt->fu0_count < 1 ||
	    opt->fu1_count < 1 ||
	    opt->fu2_count < 1 ||
	    opt->cdb_count < 1 ||
	    opt->fetch_rate < 1 ||
	    opt->max_cycles < 0 ||
	    opt->trace_file == NULL) {
		fprintf(stderr, "Error: invalid arguments\n");
		print_usage(stderr, program_name);
		exit(EXIT_FAILURE);
	}

	vlog("Running in verbose mode\n"
	     "Provided options:\n"
	     "    trace-file: %s\n"
	     "    fetch-rate: %d\n"
	     "    cdb-count:  %d\n"
	     "    k0:         %d\n"
	     "    k1:         %d\n"
	     "    k2:         %d\n",
	     opt->trace_filename, opt->fetch_rate, opt->cdb_count,
	     opt->fu0_count, opt->fu1_count, opt->fu2_count);
	if (opt->max_cycles)
		vlog("Will run at most %d cycles.", opt->max_cycles);
	free(arg0);
}

int main(int argc, char * const argv[])
{
	struct options opt  = {
		.fu0_count = DEFAULT_FU0_COUNT,
		.fu1_count = DEFAULT_FU1_COUNT,
		.fu2_count = DEFAULT_FU2_COUNT,
		.cdb_count = DEFAULT_CDB_COUNT,
		.fetch_rate = DEFAULT_FETCH_RATE,
		.trace_file = NULL,
		.max_cycles = 0
	};
	process_args(argc, argv, &opt);
	int ret = tomasulo_sim(&opt);
	fclose(opt.trace_file);
	return ret;
}
