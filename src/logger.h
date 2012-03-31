#ifndef LOGGER_H_
#define LOGGER_H_

struct int_register;

extern int VERBOSE;

/* call at start, stop of simulation */
void vlog_init();
void vlog_destroy();

/* Logs to stdout if `verbose` is set */
void vlog(const char *format, ...);

/* Add this instruction to the next pipeline stage printout. The instructions
 * will be printed, sorted by tag, on the next vlog_flush(). */
void vlog_inst(int op, struct int_register *dest, struct int_register *srcs,
	       char *pipeline_stage);

/* Print out all instructions in queue, sorted by destination tag, and clear
 * the queue */
void vlog_flush();

#endif /* end of include guard: LOGGER_H_ */

