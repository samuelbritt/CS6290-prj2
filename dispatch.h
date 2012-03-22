/*
 * Module to group together functions related to the dispatch stage of the
 * pipeline.
 */

#ifndef DISPATCH_H_
#define DISPATCH_H_

#include "deque.h"

struct int_register;

void dispatch(deque_t *dispatch_queue, struct int_register *reg_file,
	      deque_t *sched_queue);

#endif /* end of include guard: DISPATCH_H_ */

