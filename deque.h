/*
 * Implementation of a double ended queue. Carries generic void* pointers in
 * the nodes.
 */

#ifndef DEQUE_H_
#define DEQUE_H_

#include <stdbool.h>

/* opaque types */
typedef struct deque deque_t;
typedef struct deque_node deque_node_t;

/*
 * Creation functions
 */
deque_t *deque_create();

deque_node_t *deque_node_create(void *data);


/*
 * Update functions
 */
/* Insert new node before/after the given sibling node */
deque_node_t *deque_insert_before(deque_node_t *sibling,
				  deque_node_t *new_node);

deque_node_t *deque_insert_after(deque_node_t *sibling,
				 deque_node_t *new_node);

/* add to end of deque */
deque_node_t *deque_append(deque_t *deque, deque_node_t *new_node);

/* add to start of deque */
deque_node_t *deque_prepend(deque_t *deque, deque_node_t *new_node);

/* get and set the data in the node */
void *deque_node_get(deque_node_t *node);
void deque_node_set(deque_node_t *node, void *data);


/*
 * Reading functions
 */
/* returns true if empty */
bool deque_is_empty(deque_t *deque);

/* returns the first, next, and last node in the deque */
deque_node_t *deque_first(deque_t *deque);
deque_node_t *deque_next(deque_node_t *node);
deque_node_t *deque_last(deque_t *deque);

/* Call `func` on every node in deque. Convenience method for using first(),
 * next() and last() to iterate */
typedef void (*deque_func)(void *node_data, void *user_data);
void deque_foreach(deque_t *d, deque_func user_func, void *user_data);


/*
 * Delete functions
 */
/* Destroys all nodes used by deque and the deque itself */
void deque_destroy(deque_t *deque);

/* Removes node from deque, destroys the node, and returns the data it points
 * to */
void *deque_node_delete(deque_t *deque, deque_node_t *node);

/* Removes from deque and returns it */
deque_node_t *deque_node_remove(deque_t *deque, deque_node_t *node);

/* Frees any memory used by node and returns the data it points to */
void *deque_node_destroy(deque_node_t *node);

#endif /* end of include guard: DEQUE_H_ */
