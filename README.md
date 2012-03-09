CS 6290 Project 2
================
Tomasulo Algorithm
------------------------------------
### Sam Britt

####To build:

 - Run `make` to build optimized executables in the project root.
 - Run `make debug` to build non-optimized executables with debugging
   symbols in the project root.
 - Run `make clean` to remove all compiled files and generated
   dependency files.


####SYNOPSIS:

        tomasulo_sim [OPTIONS] fetch_rate trace_file

####DESCRIPTION:
  The program takes two required arguments. `fetch rate` is the number
  of instructions that can be put in the dispatch queue per cycle.
  `trace_file` is a space-delimited file containing the instruction
  address (in hex), the functional unit type (0, 1, or 2), the
  destination register, and the first and second source registers.  If
  any register is not used, it should be specified as -1. For example,

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

  - `--k0=<value>`
  - `--k1=<value>`
  - `--k2=<value>`
      Set the number of functional units for functional unit type 0,
      1, or 2. If the option is not given, defaults to 1.

  - `--cdb=<value>` or `-c=<value>`
      Set the number of common data busses for the simulation.  If the
      option is not given, defaults to 1.

