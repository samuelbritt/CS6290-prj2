#!/usr/bin/env python
# encoding: utf-8

import os
import sys
import subprocess
import argparse
import tempfile

script_dir = os.path.abspath(os.path.dirname(sys.argv[0]))
root_dir   = script_dir
prog       = os.path.join(root_dir, "tomasulo_sim")

def format_cmd(args):
    cmd = ("{prog} --fetch-rate={rate} --cdb={cdbs} "
           "--k0={k0} --k1={k1} --k2={k2} {tracefile}".format(
               prog=prog, rate=args.fetch_rate, cdbs=args.cdb_count,
               k0=args.k0, k1=args.k1, k2=args.k2,
               tracefile=args.tracefile
           ))
    return cmd

def run_cmd(cmd):
    """ returns a file containing the commands output to stdout """
    out = tempfile.TemporaryFile()
    subprocess.call(cmd, stdout=out, shell=True)
    out.seek(0)
    return out

def find_ipc_from_stats_output(stats_file):
    ipc = 0 
    for line in stats_file:
        if line.startswith("IPC (avg)"):
            ipc = float(line.split(":")[-1])
    return ipc


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', '--fetch-rate', dest='fetch_rate', type=int,
                        default=8, choices=(4,8), help="fetch rate")
    parser.add_argument('--k0', dest='k0', type=int, default=2, choices=(1,2), help="k0")
    parser.add_argument('--k1', dest='k1', type=int, default=2, choices=(1,2), help="k1")
    parser.add_argument('--k2', dest='k2', type=int, default=2, choices=(1,2), help="k2")
    parser.add_argument('-c', '--cdbs', dest='cdb_count', type=int,
                        default=6, choices=xrange(1,7), help="cdbs")
    parser.add_argument('tracefile', metavar='<tracefile>', help="Trace file")
    args = parser.parse_args()
    args.cdb_count = min(args.cdb_count, sum((args.k0, args.k1, args.k2)))
    return args

def print_args(args):
    return ("trace: {tracefile} fetch-rate: {rate}, cdbs: {cdbs}, "
            "k0: {k0}, k1: {k1}, k2: {k2}".format(
               tracefile=args.tracefile,
               rate=args.fetch_rate, cdbs=args.cdb_count,
               k0=args.k0, k1=args.k1, k2=args.k2
            ))

def hw_count(args):
    return sum((args.cdb_count, args.k0, args.k1, args.k2))

if __name__ == '__main__':
    args = parse_args()
    cmd = format_cmd(args)
    stats = run_cmd(cmd)
    ipc = find_ipc_from_stats_output(stats)

    print
    print print_args(args)
    print "hw:", hw_count(args)
    print "ipc:", ipc
