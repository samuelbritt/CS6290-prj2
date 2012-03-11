/*
 * Functions and definitions needed by several modules
 */

#ifndef COMMON_H_
#define COMMON_H_

#define fail(msg) do {			\
	fprintf(stderr, "%s\n", (msg));	\
	exit(EXIT_FAILURE);		\
} while (0);

/* *alloc's that fail */
static inline void *emalloc(size_t size)
{
	void *p = malloc(size);
	if (!p)
		fail("Memory error");
	return p;
}

#endif /* end of include guard: COMMON_H_ */
