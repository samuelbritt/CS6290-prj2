#include <stdio.h>
#include <string.h>
#include "CuTest.h"

#include <deque.c>

/* Tests */

void test_deque_create_destroy(CuTest *tc)
{
	deque_t *d = NULL;
	d = deque_create();
	CuAssertPtrNotNull(tc, d);
	deque_destroy(d);

	deque_node_t *n = NULL;
	n = deque_node_create(NULL);
	CuAssertPtrNotNull(tc, n);
	deque_node_destroy(n);
}

void test_assign(CuTest *tc)
{
	deque_node_t *n = NULL;
	int i = 4;
	n = deque_node_create(&i);
	CuAssertIntEquals(tc, i, *(int *) deque_node_get(n));
}

void test_null(CuTest *tc)
{
	deque_t *d = deque_create();
	CuAssertTrue(tc, deque_is_empty(d));

	deque_node_t *head = d->head;
	deque_node_t *tail = d->tail;
	CuAssertPtrEquals(tc, NULL, head->prev);
	CuAssertPtrEquals(tc, tail, head->next);
	CuAssertPtrEquals(tc, head, tail->prev);
	CuAssertPtrEquals(tc, NULL, tail->next);

	int i = 0;
	deque_node_t *n = deque_node_create(&i);
	CuAssertPtrEquals(tc, NULL, n->prev);
	CuAssertPtrEquals(tc, NULL, n->next);
}

void test_deque_append(CuTest *tc)
{
	deque_t *d = deque_create();
	CuAssertTrue(tc, deque_is_empty(d));

	int node_count = 10;
	deque_node_t *nodes[node_count];
	int vals[node_count];
	for (int i = 0; i < node_count; ++i) {
		vals[i] = i;
		nodes[i] = deque_node_create(&vals[i]);
		deque_append(d, nodes[i]);
		CuAssertIntEquals(tc, i, *(int *)deque_node_get(nodes[i]));
		CuAssertTrue(tc, !deque_is_empty(d));
		CuAssertPtrEquals(tc, nodes[0], deque_first(d));
		CuAssertPtrEquals(tc, nodes[i], deque_last(d));

		for (int j = i; j > 0; --j)
			CuAssertPtrEquals(tc, nodes[j],
					  deque_next(nodes[j-1]));
	}
}

void test_deque_prepend(CuTest *tc)
{
	deque_t *d = deque_create();
	CuAssertTrue(tc, deque_is_empty(d));

	int node_count = 10;
	deque_node_t *nodes[node_count];
	int vals[node_count];
	for (int i = 0; i < node_count; ++i) {
		vals[i] = i;
		nodes[i] = deque_node_create(&vals[i]);
		deque_prepend(d, nodes[i]);
		CuAssertIntEquals(tc, i, *(int *)deque_node_get(nodes[i]));
		CuAssertTrue(tc, !deque_is_empty(d));
		CuAssertPtrEquals(tc, nodes[i], deque_first(d));
		CuAssertPtrEquals(tc, nodes[0], deque_last(d));

		for (int j = i; j > 0; --j)
			CuAssertPtrEquals(tc, nodes[j-1],
					  deque_next(nodes[j]));
	}
}

static void print_int(void *i, void *str)
{
	char *s1 = str;
	char s2[1024];
	sprintf(s2, "%d -> ", *(int *) i);
	strcat(s1, s2);
}

void test_foreach(CuTest *tc)
{
	deque_t *d = deque_create();
	CuAssertTrue(tc, deque_is_empty(d));

	char s_act[1024] = "";
	char s_exp[1024] = "";

	int node_count = 10;
	deque_node_t *nodes[node_count];
	int vals[node_count];
	for (int i = 0; i < node_count; ++i) {
		vals[i] = i;
		nodes[i] = deque_node_create(&vals[i]);
		deque_append(d, nodes[i]);

		print_int(&vals[i], s_exp);
	}

	deque_foreach(d, print_int, s_act);
	CuAssertStrEquals(tc, s_exp, s_act);
}


/* Suite */
CuSuite* test_deque_get_suite()
{
	CuSuite *suite = CuSuiteNew();

	/* SUITE_ADD_TEST calls */
	SUITE_ADD_TEST(suite, test_deque_create_destroy);
	SUITE_ADD_TEST(suite, test_assign);
	SUITE_ADD_TEST(suite, test_null);
	SUITE_ADD_TEST(suite, test_deque_append);
	SUITE_ADD_TEST(suite, test_deque_prepend);
	SUITE_ADD_TEST(suite, test_foreach);

	return suite;
}
