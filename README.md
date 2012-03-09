CS 6290 Project 2
================
Tomasulo Algorithm
------------------------------------
### Sam Britt

To build:

 - Run `make` to build optimized executables in the project root.
 - Run `make debug` to build non-optimized executables with debugging
   symbols in the project root.
 - Run `make clean` to remove all compiled files and generated
   dependency files.


SYNOPSIS:

        tomasulo_sim [OPTIONS] N trace_file

DESCRIPTION:
  The program takes two required arguments. `N` is the fetch rate;
  that is, the number of instructions that can be put in the dispatch
  queue per cycle. `trace_file` is a space-delimited file containing
  the instruction address (in hex), the functional unit type (0, 1, or
  2), the destination register, and the first and second source
  registers.  If any register is not used, it should be specified as
  -1. For example,

        ab120024 0  1 2 3
        ab120028 1  4 1 3
        ab12002c 2 -1 4 7

OPTIONS:
  The following options are available:

