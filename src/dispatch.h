/*
 * Module to group together functions related to the dispatch stage of the
 * pipeline.
 */

#ifndef DISPATCH_H_
#define DISPATCH_H_

#include "deque.h"

struct int_register;

void dispatch(struct int_register *reg_file);

/* Functions to manipulate the internal dispatch queue */
void disp_init(); /* call before anything else */
void disp_destroy();
struct instruction *disp_add_inst();
bool disp_queue_is_empty();

#endif /* end of include guard: DISPATCH_H_ */

