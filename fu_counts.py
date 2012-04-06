#!/usr/bin/env python
# encoding: utf-8

# reads the set of trace files provided on the command line and prints out the
# percentage of instructions of each FU type

import sys

for filename in sys.argv[1:]:
    trace = open(filename, 'r')

    fu_counts = [0, 0, 0]
    for line in trace:
        vals = line.split()
        fu_type = int(vals[1])
        fu_counts[fu_type] += 1

    total_instructions = float(sum(fu_counts))
    print filename
    print "FU0: %g%%" % (fu_counts[0] / total_instructions * 100)
    print "FU1: %g%%" % (fu_counts[1] / total_instructions * 100)
    print "FU2: %g%%" % (fu_counts[2] / total_instructions * 100)
    print
