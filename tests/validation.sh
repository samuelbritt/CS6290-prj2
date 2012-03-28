#!/bin/sh

TRACESDIR=traces
VALIDDIR=tests/validation

PROG=./tomasulo_sim
DIFF="diff -u"

# Usage: tomalulo_sim [-v] <fetch-rate> <k0> <k1> <k2> <cdb-count> <trace_file>
$PROG 4 2 2 2 2    $TRACESDIR/barnes.txt | $DIFF $VALIDDIR/barnes1.txt -
$PROG 4 3 3 3 4    $TRACESDIR/barnes.txt | $DIFF $VALIDDIR/barnes2.txt -
$PROG 8 3 3 3 2    $TRACESDIR/gcc.txt    | $DIFF $VALIDDIR/gcc1.txt -
$PROG 8 3 3 3 8    $TRACESDIR/gcc.txt    | $DIFF $VALIDDIR/gcc2.txt -
$PROG 4 2 2 2 4    $TRACESDIR/ocean.txt  | $DIFF $VALIDDIR/ocean1.txt -
$PROG 1 1 1 1 1    $TRACESDIR/perl.txt   | $DIFF $VALIDDIR/perl1.txt -
$PROG 4 2 2 2 2 -v $TRACESDIR/test1.txt  | $DIFF $VALIDDIR/test1_1.txt -
$PROG 4 2 2 2 1 -v $TRACESDIR/test1.txt  | $DIFF $VALIDDIR/test1_2.txt -
$PROG 4 2 2 2 2 -v $TRACESDIR/test2.txt  | $DIFF $VALIDDIR/test2_1.txt -
$PROG 4 2 2 2 1 -v $TRACESDIR/test2.txt  | $DIFF $VALIDDIR/test2_2.txt -
$PROG 4 2 2 2 2 -v $TRACESDIR/test3.txt  | $DIFF $VALIDDIR/test3_1.txt -
$PROG 4 2 2 2 1 -v $TRACESDIR/test3.txt  | $DIFF $VALIDDIR/test3_2.txt -
$PROG 4 2 2 2 2 -v $TRACESDIR/test4.txt  | $DIFF $VALIDDIR/test4_1.txt -
$PROG 4 2 2 2 1 -v $TRACESDIR/test4.txt  | $DIFF $VALIDDIR/test4_2.txt -
