/*
 * Module to group together functions related to the instruction fetch stage of
 * the pipeline.
 */

#ifndef FETCH_H_
#define FETCH_H_

/* Fetches `fetch_rate` instructions and puts them in the dispatch queue */
void instruction_fetch(FILE *trace_file, int fetch_rate,
		       deque_t *dispatch_queue);


#endif /* end of include guard: FETCH_H_ */

