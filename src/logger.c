#include <stdio.h>
#include <stdarg.h>

#include "logger.h"
#include "common.h"

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
vlog_inst(int op, struct int_register *dest, struct int_register srcs[],
	  char *pipeline_stage)
{
	vlog("op: k%d - dest/tag: %d/%d - ", op, dest->index, dest->tag);
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
	     vlog("src%d/tag: %d/%d - ", i+1, srcs[i].index, srcs[i].tag);
	}
	vlog("stage: %s\n", pipeline_stage);
}
