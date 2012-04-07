CS 6290 Project 2
================
Tomasulo Algorithm
------------------------------------
### Sam Britt

####The Tour
  All source files for the Tomasulo simulation program are in the
  `src/` directory. Details on how to build and run the program are
  the sections that follow.

  The `trace/` directory contains all the trace files provided
  by the TA, used for both validation and optimization.

  The `tests/` directory contains some unit tests, as well as the
  scripts and sample output for the validation runs. See the file
  `tests/validation.py` to see how to run the validation script.
  Output from my validation is in the project root, however. The file
  `validation_run` contains the statistics output for all the
  validation simulations; it was obtained by running

      ./tests/validation.py --quiet > validation_run

  from the project root. The file `validation_run.patch` is a series
  of diffs between my validation output and the output provided by the
  TA, showing that they only differ due to rounding error in the
  floating point values. It was obtained by running

      ./tests/validation.py --diff > validation_run.patch

  from the project root.

  The optimization script and results are in the project root, along
  with a simple script to count the percentage of instructions using
  each type of functional unit for a trace file.

  The `report/` directory contains the sources for the report. The
  project root contains a symlink to the compiled PDF.

####To build:

 - Run `make` to build optimized executables in the project root.
 - Run `make debug` to build non-optimized executables with debugging
   symbols in the project root.
 - Run `make test` to run the unit tests and validation script.
 - Run `make clean` to remove all compiled files and generated
   dependency files.

####SYNOPSIS:

    tomalulo_sim [-v] [-m <max-cycles]
            <fetch-rate> <k0> <k1> <k2> <cdb-count> <trace_file>

    tomalulo_sim [-v] [-m <max-cycles]
            [-n <fetch-rate>] [--k0=<count>] [--k1=<count>]
            [--k2=<count>] [-c <count>] <trace_file>

    tomalulo_sim --help

####DESCRIPTION:
  The program can be run in one of two forms. In the first form, all
  simulation parameters are specified as position arguments. Their
  meaning is the same as the corresponding option described below.
  In the second form, the parameters are provided as named optional
  arguments. There is no in-between: either all arguments have to be
  positional, or none of them can be. If the positional form is used,
  any additional optional arguments will be overridden.

  In either form, `<trace_file>` is the final positional argument. It
  must be the path to a space-delimited file where each line contains
  instruction address (in hex), the functional unit type (0, 1, or
  2), the destination register, and the first and second source
  registers. If any register is not used, it should be specified as
  -1. For example,

        ab120024 0  1 2 3
        ab120028 1  4 1 3
        ab12002c 2 -1 4 7

####OPTIONS:
  The following options are available:

  - `-h`, `--help`

      Print help message and exit.

  - `-v`, `--verbose`

      Verbose mode: print the progress of the simulation; e.g. print
      notifications when an instruction is fetched, dispatched, etc.

  - `-m <value>`, `--max-cycles=<value>`

      Limit the number of cycles of the simulation to at most <value>.
      This is useful in debugging to prevent infinite loops.

  - `-n <value>`, `--fetch-rate=<value>`

      Set the fetch rate for the simulation; i.e., the number of
      instructions fetched per cycle. Defaults to 1.

  - `--k0=<value>`

    `--k1=<value>`

    `--k2=<value>`

      Set the number of functional units for functional unit type 0,
      1, or 2. Defaults to 1.

  - `-c <value>`, `--cdb=<value>`

      Set the number of common data buses for the simulation.
      Defaults to 1.
