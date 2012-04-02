/*
 * Module to group together functions related to the instruction fetch stage of
 * the pipeline.
 */

#ifndef FETCH_H_
#define FETCH_H_

#include <stdio.h>

/* Fetches `fetch_rate` instructions and puts them in the dispatch queue.
 * Returns the number of instructions successfully fetched. */
int instruction_fetch(FILE *trace_file, int fetch_rate);

#endif /* end of include guard: FETCH_H_ */

