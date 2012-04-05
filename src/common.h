/*
 * Functions and definitions needed by several modules
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

static inline void *ecalloc(size_t size)
{
	void *p = calloc(1, size);
	if (!p)
		fail("Memory error");
	return p;
}

static inline void *erealloc(void *p, size_t size)
{
	p = realloc(p, size);
	if (!p)
		fail("Memory error");
	return p;
}

/* Defines and Data structures used by several modules */

/* Max number of source registers for any instruction */
#define SRC_REGISTER_COUNT 2

/* Number of architectural registers */
#define ARCH_REGISTER_COUNT 128

/* Integer register */
struct int_register {
	bool ready;
	int tag;
	int val;
	int index;  /* index into reg_file */
};

/* Common data bus */
struct cdb {
	int tag;
	int val;
	int reg_num;
};

/* Instruction as read from file */
struct instruction {
	int id;
	void *addr;
	int fu_type;
	int dest_reg_num;
	int src_reg_num[SRC_REGISTER_COUNT];
};

struct reservation_station {
	bool fired;       /* so we don't re-schedule instructions */
	bool ready_to_fire;
	int fu_type;
	struct int_register dest;
	struct int_register src[SRC_REGISTER_COUNT];
};

#endif /* end of include guard: COMMON_H_ */
