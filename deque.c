#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "deque.h"
#include "common.h"

struct deque_node {
	void *data;
	struct deque_node *prev;
	struct deque_node *next;
};

struct deque {
	/* These are sentinals. First real node is head->next, and last node
	 * is tail->prev */
	struct deque_node *head;
	struct deque_node *tail;
};

deque_node_t *deque_start(deque_t *deque)
{
	return deque->head->next;
}

deque_node_t *deque_next(deque_t *deque, deque_node_t *node)
{
	return node->next;
}

bool deque_end(deque_t *deque, deque_node_t *node)
{
	return node == deque->tail->prev;
}

bool deque_is_empty(deque_t *deque)
{
	return deque->head->next == deque->tail;
}

deque_t *deque_create()
{
	deque_t *dq = emalloc(sizeof(*dq));
	dq->head = deque_create_node(NULL);
	dq->tail = deque_create_node(NULL);
	dq->head->next = dq->tail;
	dq->tail->prev = dq->head;
	dq->head->prev = NULL;
	dq->tail->next = NULL;
	return dq;
}

void deque_destroy(deque_t *deque)
{
	deque_node_t *p, *next;
	p = deque_start(deque);
	while (!deque_end(deque, p)) {
		next = deque_next(deque, p);
		deque_destroy_node(p);
		p = next;
	}
	deque_destroy_node(deque->head);
	deque_destroy_node(deque->tail);
	free(deque);
}

deque_node_t *deque_create_node(void *data)
{
	deque_node_t *n = emalloc(sizeof(n));
	n->data = data;
	n->prev = NULL;
	n->next = NULL;
	return n;
}

void deque_destroy_node(deque_node_t *node)
{
	free(node);
}

deque_node_t *deque_insert_before(deque_t *deque, deque_node_t *node,
				  deque_node_t *new_node)
{
	new_node->next = node;
	new_node->prev = node->prev;
	node->prev->next = new_node;
	node->prev = new_node;
	return new_node;
}


deque_node_t *deque_insert_after(deque_t *deque, deque_node_t *node,
				 deque_node_t *new_node)
{
	new_node->next = node->next;
	new_node->prev = node;
	node->next->prev = new_node;
	node->next = new_node;
	return new_node;
}

deque_node_t *deque_prepend(deque_t *deque, deque_node_t *node)
{
	return deque_insert_after(deque, deque->head, node);
}


deque_node_t *deque_append(deque_t *deque, deque_node_t *node)
{
	return deque_insert_before(deque, deque->tail, node);
}

deque_node_t *deque_delete_node(deque_t *deque, deque_node_t *node)
{
	if (node == deque->head)
		deque->head = node->next;
	node->prev->next = node->next;
	node->next->prev = node->prev;
	return node;
}
