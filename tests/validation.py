#!/usr/bin/env python
# encoding: utf-8

#
# Validation Script
#
# To run all the validation traces, and output a diff comparing to the output
# provided by the TA, run the script with no arguments
#
# To run a specific run, give the name of the run as the first arg, e.g.
#       ./validation.py gcc1.txt
#
# To run a specific run, but instead of showing the output, show a diff
# comparing to the output provided by the TA, run with 'diff' as the second
# argument, e.g.
#       ./validation.py gcc1.txt diff

import os
import sys
import subprocess

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

traces_dir = "traces"
valid_dir  = "tests/validation"
prog       = "./tomasulo_sim"
diff       = "diff -u"

def run_cmd(validation_file):
    run = validation_runs[validation_file]
    cmd = prog + " " + " ".join(str(opt) for opt in run['opts'])
    if run['verbose']:
        cmd += " -v"
    cmd += " " + os.path.join(traces_dir, run['trace'])
    return cmd

def diff_cmd(validation_file):
    cmd = run_cmd(validation_file)
    cmd += " | {} {} -".format(diff, os.path.join(valid_dir, validation_file))
    return cmd

if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args) == 0:
        # No arguments: run a diff on all the short validation runs
        short_runs = [r for r in validation_runs if "big" not in r]
        for r in short_runs:
            c = diff_cmd(r)
            subprocess.call(c, shell=True)
    else:
        validation_file = args[0]
        try:
            r = validation_runs[validation_file]
        except KeyError:
            print "Invalid validation file"
            sys.exit(1)

        if len(args) > 1 and args[1] == "diff":
            c = diff_cmd(validation_file)
        else:
            c = run_cmd(validation_file)

        subprocess.call(c, shell=True)
