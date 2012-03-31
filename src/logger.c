#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "logger.h"
#include "deque.h"
#include "common.h"

/* Print information at each step */
int VERBOSE = 0;

/* Maintain a sorted queue of pipeline stages (sorted on destination tag). If
 * VERBOSE is set, print to stdout and empty the queue when flush() is called.
 */
deque_t *log_queue;

struct log_node {
	int tag;
	char msg[1024];
};

/* Comparison function for deque_insert_sorted().
 * Returns > 0 if (the tag of) a > (the tag of) b,
 *         < 0 if a < b,
 *           0 if a == b */
static int
tag_compare(void *node_a, void *node_b)
{
	int a = ((struct log_node *) node_a)->tag;
	int b = ((struct log_node *) node_b)->tag;
	return a - b;
}

static bool
write_srcs_log_msg(char *buf, size_t buf_size, struct int_register srcs[])
{
	int n_written = 0;
	for (int i = 0; i < SRC_REGISTER_COUNT; ++i) {
		size_t space_left = buf_size - n_written;
		n_written += snprintf(buf + n_written, space_left,
				      "src%d/tag: %d/%d - ",
				      i+1, srcs[i].index, srcs[i].tag);
		if (n_written > space_left)
			return true;
	}
	return false;
}

static bool
write_inst_log_msg(char *buf, size_t buf_size,
		   int op, struct int_register *dest,
		   struct int_register srcs[], char *pipeline_stage)
{
	bool truncated = false;
	/* write the src register part of the log message to a temp buffer */
	char src_str[1024];
	if (write_srcs_log_msg(src_str, sizeof(src_str), srcs))
		truncated = true;

	/* write the entire log message */
	int n = snprintf(buf, buf_size,
			 "op: k%d - dest/tag: %d/%d - %sstage: %s\n",
			 op, dest->index, dest->tag, src_str, pipeline_stage);
	if (n > buf_size)
		truncated = true;
	return truncated;
}

void
vlog_inst(int op, struct int_register *dest, struct int_register srcs[],
	  char *pipeline_stage)
{
	if (!VERBOSE)
		return;

	struct log_node *node = emalloc(sizeof(*node));
	node->tag = dest->tag;
	if (write_inst_log_msg(node->msg, sizeof(node->msg),
			       op, dest, srcs, pipeline_stage))
		vlog("Warning: Truncated log message");
	deque_insert_sorted(log_queue, node, tag_compare);
}

static void
clear_log_queue()
{
	while (!deque_is_empty(log_queue))
		free(deque_delete_first(log_queue));
}

static void
print_node(void *node_, void *arg)
{
	struct log_node *node = node_;
	vlog(node->msg);
}

void
vlog_flush()
{
	deque_foreach(log_queue, print_node, NULL);
	clear_log_queue();
}

void
vlog_init()
{
	log_queue = deque_create();
}

void
vlog_destroy()
{
	deque_destroy(log_queue);
}

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
