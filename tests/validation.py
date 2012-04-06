#!/usr/bin/env python
# encoding: utf-8

#
# Validation Script
#
# Runs one or more simulations and can compare with the validation files
# provided by the TA.
#
# When run with no arguments, runs all the validation traces and outputs to
# stdout:
#
#       ./validation.py
#
# When run with a single validation file input, will only run that simulation:
#
#       ./validation.py gcc1.txt
#
# Options:
#   -h, --help
#           Print a help message and exit.
#
#   -d, --diff
#           Instead of printing the simulation output to stdout, run a diff from
#           sample output provided by the TA to the simulation output, and print
#           to stdout.
#
#   -q, --quiet
#           Turn off all verbosity from the simulations (only print statistics
#           results). If this option is not provided, verbose mode is turned on
#           in the simulation only if it is turned on for the sample output
#           provided by the TA. Note that, if both --quiet and --diff are
#           specified, a large diff could be produced (because the sample files
#           are verbose but the simulation output is not).

import os
import sys
import subprocess
import argparse

# Usage: tomalulo_sim [-v] <fetch-rate> <k0> <k1> <k2> <cdb-count> <trace_file>
validation_runs = {
    "barnes1.txt": {'opts': (4, 2, 2, 2, 2), 'verbose':  False, 'trace': "barnes.txt"},
    "barnes2.txt": {'opts': (4, 3, 3, 3, 4), 'verbose':  False, 'trace': "barnes.txt"},
    "gcc1.txt"   : {'opts': (8, 3, 3, 3, 2), 'verbose':  False, 'trace': "gcc.txt"},
    "gcc2.txt"   : {'opts': (8, 3, 3, 3, 8), 'verbose':  False, 'trace': "gcc.txt"},
    "ocean1.txt" : {'opts': (4, 2, 2, 2, 4), 'verbose':  False, 'trace': "ocean.txt"},
    "perl1.txt"  : {'opts': (1, 1, 1, 1, 1), 'verbose':  False, 'trace': "perl.txt"},
    "test1_1.txt": {'opts': (4, 2, 2, 2, 2), 'verbose':  True,  'trace': "test1.txt"},
    "test1_2.txt": {'opts': (4, 2, 2, 2, 1), 'verbose':  True,  'trace': "test1.txt"},
    "test2_1.txt": {'opts': (4, 2, 2, 2, 2), 'verbose':  True,  'trace': "test2.txt"},
    "test2_2.txt": {'opts': (4, 2, 2, 2, 1), 'verbose':  True,  'trace': "test2.txt"},
    "test3_1.txt": {'opts': (4, 2, 2, 2, 2), 'verbose':  True,  'trace': "test3.txt"},
    "test3_2.txt": {'opts': (4, 2, 2, 2, 1), 'verbose':  True,  'trace': "test3.txt"},
    "test4_1.txt": {'opts': (4, 2, 2, 2, 2), 'verbose':  True,  'trace': "test4.txt"},
    "test4_2.txt": {'opts': (4, 2, 2, 2, 1), 'verbose':  True,  'trace': "test4.txt"},
    "barnes-big.txt": {'opts': (4, 3, 3, 3, 4), 'verbose':  True, 'trace': "barnes.txt"},
}

script_dir = os.path.abspath(os.path.dirname(sys.argv[0]))
root_dir   = os.path.join(script_dir, "..")
traces_dir = os.path.join(root_dir, "traces")
valid_dir  = os.path.join(script_dir, "validation")
prog       = os.path.join(root_dir, "tomasulo_sim")
diff       = "diff -u"

def format_run_cmd(validation_file, quiet=False):
    run = validation_runs[validation_file]
    cmd = prog + " " + " ".join(str(opt) for opt in run['opts'])
    if run['verbose'] and not quiet:
        cmd += " -v"
    relative_path = os.path.relpath(traces_dir, start=root_dir)
    cmd += " " + os.path.join(relative_path, run['trace'])
    return cmd

def format_diff_cmd(validation_file, quiet):
    cmd = format_run_cmd(validation_file, quiet)
    cmd += " | {} {} -".format(diff, os.path.join(valid_dir, validation_file))
    return cmd

def run_validation_files(cmd_formatter, files, quiet=False):
    for r in files:
        print
        print r + ":"
        sys.stdout.flush()
        c = cmd_formatter(r, quiet)
        subprocess.call(c, shell=True)

def get_short_runs():
    return [r for r in validation_runs if "big" not in r]

def valid_run_name_or_exit(filename):
    validation_file = os.path.basename(filename)
    if validation_file not in validation_runs:
        print "Error: invalid validation file"
        sys.exit(1)
    return validation_file

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--diff', dest='diff', action='store_true',
                        help="Show a full diff of simulation output and the "
                        "provided validation file")
    parser.add_argument('-q', '--quiet', dest='quiet', action='store_true',
                        help="Run simulations in quiet (not verbose) mode. "
                        "Without this option, verbose mode is enabled if the "
                        "provided validation file was run in verbose mode.")
    parser.add_argument('validation_file', metavar='<validation_file>', nargs='?',
                        help="Validation file to compare simulation output to.")
    return parser.parse_args()

if __name__ == '__main__':
    args = parse_args()

    cmd_formatter = format_run_cmd
    if args.diff:
        cmd_formatter = format_diff_cmd

    if args.validation_file:
        run_name = valid_run_name_or_exit(args.validation_file)
        runs = [run_name]
    else:
        runs = sorted(validation_runs.keys())
    run_validation_files(cmd_formatter, runs, args.quiet)
