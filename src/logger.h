extern int VERBOSE;

/* Logs to stdout if `verbose` is set */
void vlog(const char *format, ...);

/* Use for printing the pipeline stage of a particular instruction */
void vlog_inst(int inst_id, char *pipeline_stage);
