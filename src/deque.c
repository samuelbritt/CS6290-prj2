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
	/* These are sentinels. First real node is head->next, and last node
	 * is tail->prev */
	struct deque_node *head;
	struct deque_node *tail;
};

static deque_node_t *
deque_node_create(void *data)
{
	deque_node_t *n = emalloc(sizeof(*n));
	n->data = data;
	n->prev = NULL;
	n->next = NULL;
	return n;
}

deque_t *
deque_create()
{
	deque_t *dq = emalloc(sizeof(*dq));
	dq->head = deque_node_create(NULL);
	dq->tail = deque_node_create(NULL);
	dq->head->next = dq->tail;
	dq->tail->prev = dq->head;
	return dq;
}

deque_node_t *
deque_first(deque_t *deque)
{
	return deque->head->next;
}

deque_node_t *
deque_next(deque_node_t *node)
{
	return node->next;
}

deque_node_t *
deque_last(deque_t *deque)
{
	return deque->tail->prev;
}

bool
deque_is_empty(deque_t *deque)
{
	return deque->head->next == deque->tail;
}

void *
deque_node_get(deque_node_t *node)
{
	return node->data;
}

void
deque_node_set(deque_node_t *node, void *data)
{
	node->data = data;
}

void
deque_foreach(deque_t *d, deque_func user_func, void *user_data)
{
	deque_node_t *p;
	for (p = deque_first(d); p != d->tail; p = deque_next(p)) {
		user_func(p->data, user_data);
	}
}

static deque_node_t *
deque_insert_before(deque_node_t *sibling, deque_node_t *new_node)
{
	new_node->next = sibling;
	new_node->prev = sibling->prev;
	sibling->prev->next = new_node;
	sibling->prev = new_node;
	return new_node;
}


static deque_node_t *
deque_insert_after(deque_node_t *sibling, deque_node_t *new_node)
{
	new_node->next = sibling->next;
	new_node->prev = sibling;
	sibling->next->prev = new_node;
	sibling->next = new_node;
	return new_node;
}

deque_node_t *
deque_prepend(deque_t *deque, void *data)
{
	deque_node_t *node = deque_node_create(data);
	return deque_insert_after(deque->head, node);
}

deque_node_t *
deque_append(deque_t *deque, void *data)
{
	deque_node_t *node = deque_node_create(data);
	return deque_insert_before(deque->tail, node);
}

deque_node_t *
deque_insert_sorted(deque_t *deque, void *data, deque_comp_func comp_func)
{
	deque_node_t *new_node = deque_node_create(data);
	deque_node_t *p = deque_first(deque);
	while (p != deque->tail && comp_func(data, p->data) >= 0)
		p = p->next;
	return deque_insert_before(p, new_node);
}

/* Remove node from list without destroying the node */
static deque_node_t *
deque_remove_node(deque_t *deque, deque_node_t *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

/* Destroy the node and return the data it pointed to */
static void *
deque_destroy_node(deque_node_t *node)
{
	void *data = node->data;
	free(node);
	return data;
}

void *
deque_delete_node(deque_t *d, deque_node_t *n)
{
	deque_remove_node(d, n);
	return deque_destroy_node(n);
}

void *
deque_delete(deque_t *d, void *data)
{
	if (deque_is_empty(d))
		return NULL;

	deque_node_t *p = deque_first(d);
	while (p != d->tail) {
		if (p->data == data)
			return deque_delete_node(d, p);
		p = deque_next(p);
	}
	return NULL;
}

void *
deque_delete_first(deque_t *d)
{
	if (deque_is_empty(d))
		return NULL;

	return deque_delete_node(d, deque_first(d));
}
void *
deque_delete_last(deque_t *d)
{
	if (deque_is_empty(d))
		return NULL;

	return deque_delete_node(d, deque_last(d));
}

void
deque_destroy(deque_t *d)
{
	while (!deque_is_empty(d))
		deque_delete_first(d);
	deque_destroy_node(d->head);
	deque_destroy_node(d->tail);
	free(d);
}
