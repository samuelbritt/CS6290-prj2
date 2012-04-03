/*
 * Functions handling the update of the scheduling queue and register file
 */

#ifndef STATE_UPDATE_H_
#define STATE_UPDATE_H_

#include "common.h"

/* Updates both the scheduling queue and the register file where necessary.
 * Updates are done in tag order from the CDBs */
void state_update(int cdb_count, struct int_register *reg_file);

#endif /* end of include guard: STATE_UPDATE_H_ */
