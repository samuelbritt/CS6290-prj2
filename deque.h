/*
 * Implementation of a double ended queue. Carries generic void* pointers in
 * the nodes.
 */

#ifndef DEQUE_H_
#define DEQUE_H_

#include <stdbool.h>

typedef struct deque deque_t;
typedef struct deque_node deque_node_t;

deque_t *deque_create();
void deque_destroy(deque_t *deque);

deque_node_t *deque_create_node(void *data);
deque_node_t *deque_delete_node(deque_t *deque, deque_node_t *node);
void deque_destroy_node(deque_node_t *node);

deque_node_t *deque_insert_before(deque_t *deque, deque_node_t *node,
				  deque_node_t *new_node);
deque_node_t *deque_insert_after(deque_t *deque, deque_node_t *node,
				 deque_node_t *new_node);
deque_node_t *deque_append(deque_t *deque, deque_node_t *node);
deque_node_t *deque_prepend(deque_t *deque, deque_node_t *node);

/* Functions to allow walking through deque */
bool deque_is_empty(deque_t *deque);
deque_node_t *deque_start(deque_t *deque);
deque_node_t *deque_next(deque_t *deque, deque_node_t *node);
bool deque_end(deque_t *deque, deque_node_t *node); /* returns true if last node */

#endif /* end of include guard: DEQUE_H_ */
