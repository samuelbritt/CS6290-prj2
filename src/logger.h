#ifndef LOGGER_H_
#define LOGGER_H_

struct int_register;

extern int VERBOSE;

/* Logs to stdout if `verbose` is set */
void vlog(const char *format, ...);

/* Use for printing the pipeline stage of a particular instruction */
void vlog_inst(int op, struct int_register *dest, struct int_register *srcs,
	       char *pipeline_stage);

#endif /* end of include guard: LOGGER_H_ */

