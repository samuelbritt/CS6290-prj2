/*
 * Implementation of a double ended queue. Carries generic void* pointers in
 * the nodes.
 */

#ifndef DEQUE_H_
#define DEQUE_H_

#include <stdbool.h>

/* opaque pointers */
typedef struct deque deque_t;
typedef struct deque_node deque_node_t;

/*
 * Creation functions
 */
deque_t *deque_create();

deque_node_t *deque_create_node(void *data);


/*
 * Update functions
 */
deque_node_t *deque_insert_before(deque_t *deque, deque_node_t *node,
				  deque_node_t *new_node);

deque_node_t *deque_insert_after(deque_t *deque, deque_node_t *node,
				 deque_node_t *new_node);

/* add to end of deque */
deque_node_t *deque_append(deque_t *deque, deque_node_t *node);

/* add to start of deque */
deque_node_t *deque_prepend(deque_t *deque, deque_node_t *node);


/*
 * Reading functions
 */
/* returns true if empty */
bool deque_is_empty(deque_t *deque);

/* start(), next(), and end() allow walking through deque */
/* returns the first node in the deque */
deque_node_t *deque_start(deque_t *deque);

/* returns the node after the given node */
deque_node_t *deque_next(deque_t *deque, deque_node_t *node);

/* returns true if `node` is the last in the deque */
bool deque_end(deque_t *deque, deque_node_t *node);

/* returns the data in the node */
void *deque_node_data(deque_node_t *node);


/*
 * Delete functions
 */
void deque_destroy(deque_t *deque);

/* Deletes from deque, but does not destroy the node */
deque_node_t *deque_delete_node(deque_t *deque, deque_node_t *node);

void deque_destroy_node(deque_node_t *node);

#endif /* end of include guard: DEQUE_H_ */
