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

/* Create an empty deque */
deque_t *deque_create();

/* add to the start, end of deque */
deque_node_t *deque_prepend(deque_t *deque, void *new_data);
deque_node_t *deque_append(deque_t *deque, void *new_data);

/* Comparison function for sorted insert. Returns integer x, where
 * 	x > 0   if a > b
 * 	x < 0   if a < b
 * 	x == 0  if a == b
 */
typedef int (*deque_comp_func)(void *a, void *b);
/* Insert data sorted according to comp_func */
deque_node_t * deque_insert_sorted(deque_t *deque, void *data,
				   deque_comp_func comp_func);

/*
 * Reading functions
 */

/* returns true if empty */
bool deque_is_empty(deque_t *deque);

/* returns the number of nodes in the deque */
int deque_length(deque_t *deque);

/* read the data in the node */
void *deque_node_get(deque_node_t *node);

/* returns the first, next, and last node in the deque */
deque_node_t *deque_first(deque_t *deque);
deque_node_t *deque_next(deque_node_t *node);
deque_node_t *deque_last(deque_t *deque);


/*
 * Update functions
 */

/* set the data in the node */
void deque_node_set(deque_node_t *node, void *data);

/* Call `user_func` on every node in deque. Convenience method for using
 * first(), next() and last() to iterate.
 *
 * The deque_foreach_func function returns an int from the deque_control enum.
 * If it returns DEQUE_STOP at any point, the iteration will cease and return
 * to the caller of deque_foreach(). Else it will continue until the end of
 * the deque. */
enum deque_control {
	DEQUE_STOP,
	DEQUE_CONTINUE
};

typedef int (*deque_foreach_func)(void *node_data, void *user_data);
void deque_foreach(deque_t *d, deque_foreach_func user_func, void *user_data);


/*
 * Delete functions
 */

/* Remove a node from deque and returns the data it points to */
void *deque_delete_node(deque_t *deque, deque_node_t *node);

/* Remove the node containing the given data and returns the data it points
 * to */
void *deque_delete(deque_t *deque, void *data);

/* Remove the first, last node from deque and returns the data it points to */
void *deque_delete_first(deque_t *deque);
void *deque_delete_last(deque_t *deque);

/* Destroy all nodes used by deque and the deque itself */
void deque_destroy(deque_t *deque);


#endif /* end of include guard: DEQUE_H_ */
