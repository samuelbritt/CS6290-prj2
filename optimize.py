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

class SimParams(object):
    def __init__(self, **kwargs):
        self.fetch_rate = 0
        self.k0 = 0
        self.k1 = 0
        self.k2 = 0
        self._cdbs = 0
        for key in kwargs:
            setattr(self, key, kwargs[key])
        fu_count = sum((self.k0, self.k1, self.k2))

    @property
    def cdbs(self):
        return min((self._cdbs, self.fu_count()))
    @cdbs.setter
    def cdbs(self, val):
        self._cdbs = val

    @property
    def hw(self):
        return sum((self.cdbs, self.fu_count()))

    def fu_count(self):
        return sum((self.k0, self.k1, self.k2))

    def format_cmd(self):
        return ("--fetch-rate={rate} --cdb={cdbs} "
                "--k0={k0} --k1={k1} --k2={k2}".format(
                   rate=self.fetch_rate, cdbs=self.cdbs,
                   k0=self.k0, k1=self.k1, k2=self.k2))

    def __str__(self):
        return ("fetch-rate: {rate}, cdbs: {cdbs}, "
                "k0: {k0}, k1: {k1}, k2: {k2}".format(
                    rate=self.fetch_rate, cdbs=self.cdbs,
                    k0=self.k0, k1=self.k1, k2=self.k2))

    def set_max(self):
        self.fetch_rate = 8
        self.cdbs = 6
        self.k0 = 2
        self.k1 = 2
        self.k2 = 2

    def copy(self):
        p = SimParams()
        p.__dict__ = dict(self.__dict__)
        return p

class Run(object):
    def __init__(self, tracefile, **kwargs):
        self.params = SimParams(**kwargs)
        self.tracefile = tracefile
        self.max_ipc = 0

    @property
    def hw(self):
        return self.params.hw

    def optimize(self):
        print "Finding Maximum IPC..."
        self.max_ipc = self.calculate_max_ipc()
        min_hw = [self.max_ipc, self.params.copy()]
        cutoff = 0.9 * self.max_ipc

        print "Optimizing FUs..."
        min_hw = self.optimize_fus(cutoff, min_hw)
        print "Optimal FUs:"
        print "ipc:", min_hw[0], min_hw[1]

        print "Optimizing CDBs..."
        min_hw = self.optimize_cdbs(cutoff, min_hw)

        print "Final Results:"
        print "ipc", min_hw[0], min_hw[1]

    def optimize_fus(self, ipc_cutoff, min_params):
        optimal_ipc = min_params[0]
        optimal_params = min_params[1]
        for k0 in (1,2):
            self.params.k0 = k0
            for k1 in (1,2):
                self.params.k1 = k1
                for k2 in (1,2):
                    self.params.k2 = k2
                    ipc = self.calculate_ipc()
                    print "ipc:", ipc, self.params
                    if ipc < ipc_cutoff:
                        continue
                    optimal_ipc, optimal_params = self.pick_best(ipc, self.params,
                                                                 optimal_ipc,
                                                                 optimal_params)
                    optimal_params = optimal_params.copy()
        return optimal_ipc, optimal_params

    def optimize_cdbs(self, ipc_cutoff, min_hw):
        optimal_ipc = min_hw[0]
        optimal_params = min_hw[1]
        self.params = optimal_params
        for r in range(1, optimal_params.cdbs):
            self.params.cdbs = r
            ipc = self.calculate_ipc()
            print "ipc:", ipc, self.params
            if ipc > ipc_cutoff:
                return ipc, self.params.copy()


    def pick_best(self, ipc_a, params_a, ipc_b, params_b):
        opt_ipc = ipc_a
        opt_params = params_a
        if (params_b.hw < opt_params.hw or
            (params_b.hw == params_a.hw and
             ipc_b > opt_ipc)):
            opt_ipc = ipc_b
            opt_params = params_b
        return opt_ipc, opt_params


    def calculate_max_ipc(self):
        self.params.set_max()
        return self.calculate_ipc() 

    def calculate_ipc(self):
        stats_file = self.run()
        ipc = 0 
        for line in stats_file:
            if line.startswith("IPC (avg)"):
                ipc = float(line.split(":")[-1])
        return ipc

    def run(self):
        """ returns a file containing the commands output to stdout """
        cmd = self.format_cmd()
        out = tempfile.TemporaryFile()
        subprocess.call(cmd, stdout=out, shell=True)
        out.seek(0)
        return out

    def format_cmd(self):
        return ("{prog} {params} {tracefile}".format(
            prog=prog, params=self.params.format_cmd(),
            tracefile=self.tracefile))

    def __str__(self):
        return "trace: {tracefile} {params}".format(tracefile=self.tracefile,
                                                     params=self.params)

if __name__ == '__main__':
    tracefiles = sys.argv[1:]
    for tfile in tracefiles:
        print
        print "Optimizing", tfile
        r = Run(tracefile = tfile)
        r.optimize()
