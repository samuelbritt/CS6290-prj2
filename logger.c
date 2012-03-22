#include <stdio.h>
#include <stdarg.h>

/* Print information at each step */
int VERBOSE = 0;

/* Logs to stdout if `verbose` is set */
void
vlog(const char *format, ...)
{
	if (!VERBOSE)
		return;
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
}

void
vlog_inst(int inst_id, char *pipeline_stage)
{
	vlog("Instruction %5d: %s\n", inst_id, pipeline_stage);
}
