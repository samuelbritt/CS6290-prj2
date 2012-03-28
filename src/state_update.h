/*
 * Functions handling the update of the scheduling queue and register file
 */

#ifndef STATE_UPDATE_H_
#define STATE_UPDATE_H_

/* Call before anything else */
void su_init(int cdb_count);

/* Attempts to retire the instruction and return 0. If it can't (because all the
 * CDBs are busy), returns 1 */
int retire_inst(struct reservation_station *rs);

/* Updates both the scheduling queue and the register file where necessary.
 * Updates are done in tag order from the CDBs */
void state_update(struct int_register *reg_file);

/* frees any memory used by the state update unit */
void su_destroy();

#endif /* end of include guard: STATE_UPDATE_H_ */

