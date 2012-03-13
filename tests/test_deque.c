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
}

void test_assign(CuTest *tc)
{
	deque_t *d = deque_create();
	int i = 4;
	deque_node_t *n = deque_append(d, &i);
	CuAssertIntEquals(tc, i, *(int *) deque_node_get(n));

	int j = 6;
	deque_node_set(n, &j);
	CuAssertIntEquals(tc, j, *(int *) deque_node_get(n));
	deque_destroy(d);
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

	deque_destroy(d);
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
		nodes[i] = deque_append(d, &vals[i]);
		CuAssertIntEquals(tc, i, *(int *)deque_node_get(nodes[i]));
		CuAssertTrue(tc, !deque_is_empty(d));
		CuAssertPtrEquals(tc, nodes[0], deque_first(d));
		CuAssertPtrEquals(tc, nodes[i], deque_last(d));

		for (int j = i; j > 0; --j)
			CuAssertPtrEquals(tc, nodes[j],
					  deque_next(nodes[j-1]));
	}
	deque_destroy(d);
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
		nodes[i] = deque_prepend(d, &vals[i]);
		CuAssertIntEquals(tc, i, *(int *)deque_node_get(nodes[i]));
		CuAssertTrue(tc, !deque_is_empty(d));
		CuAssertPtrEquals(tc, nodes[i], deque_first(d));
		CuAssertPtrEquals(tc, nodes[0], deque_last(d));

		for (int j = i; j > 0; --j)
			CuAssertPtrEquals(tc, nodes[j-1],
					  deque_next(nodes[j]));
	}
	deque_destroy(d);
}

void test_delete(CuTest *tc)
{
	deque_t *d = deque_create();
	int node_count = 3;
	deque_node_t *nodes[node_count];
	int vals[node_count];
	for (int i = 0; i < node_count; ++i) {
		vals[i] = i;
		nodes[i] = deque_append(d, &vals[i]);
	}

	int *v1 = deque_delete_node(d, nodes[1]);
	CuAssertPtrEquals(tc, &vals[1], v1);
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[2], deque_next(nodes[0]));
	CuAssertPtrEquals(tc, nodes[2], deque_last(d));

	int *v2 = deque_delete_node(d, nodes[2]);
	CuAssertPtrEquals(tc, &vals[2], v2);
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[0], deque_last(d));

	int *v0 = deque_delete_node(d, nodes[0]);
	CuAssertPtrEquals(tc, &vals[0], v0);
	CuAssertTrue(tc, deque_is_empty(d));

	deque_destroy(d);
}

void test_delete_first(CuTest *tc)
{
	int *v;
	deque_t *d = deque_create();
	int node_count = 3;
	deque_node_t *nodes[node_count];
	int vals[node_count];
	for (int i = 0; i < node_count; ++i) {
		vals[i] = i;
		nodes[i] = deque_append(d, &vals[i]);
	}

	/* 0 -> 1 -> 2 -> */
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[2], deque_last(d));

	/* 1 -> 2 -> */
	v = deque_delete_first(d);
	CuAssertPtrEquals(tc, &vals[0], v);
	CuAssertPtrEquals(tc, nodes[1], deque_first(d));
	CuAssertPtrEquals(tc, nodes[2], deque_next(nodes[1]));
	CuAssertPtrEquals(tc, nodes[2], deque_last(d));

	/* 2 -> */
	v = deque_delete_first(d);
	CuAssertPtrEquals(tc, &vals[1], v);
	CuAssertPtrEquals(tc, nodes[2], deque_first(d));
	CuAssertPtrEquals(tc, nodes[2], deque_last(d));

	v = deque_delete_first(d);
	CuAssertPtrEquals(tc, &vals[2], v);
	CuAssertTrue(tc, deque_is_empty(d));

	deque_destroy(d);
}

void test_delete_last(CuTest *tc)
{
	int *v;
	deque_t *d = deque_create();
	int node_count = 3;
	deque_node_t *nodes[node_count];
	int vals[node_count];
	for (int i = 0; i < node_count; ++i) {
		vals[i] = i;
		nodes[i] = deque_append(d, &vals[i]);
	}

	/* 0 -> 1 -> 2 -> */
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[2], deque_last(d));

	/* 0 -> 1 -> */
	v = deque_delete_last(d);
	CuAssertPtrEquals(tc, &vals[2], v);
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[1], deque_next(nodes[0]));
	CuAssertPtrEquals(tc, nodes[1], deque_last(d));

	/* 0 -> */
	v = deque_delete_last(d);
	CuAssertPtrEquals(tc, &vals[1], v);
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[0], deque_last(d));

	v = deque_delete_last(d);
	CuAssertPtrEquals(tc, &vals[0], v);
	CuAssertTrue(tc, deque_is_empty(d));

	deque_destroy(d);
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
	int vals[node_count];
	for (int i = 0; i < node_count; ++i) {
		vals[i] = i;
		deque_append(d, &vals[i]);

		print_int(&vals[i], s_exp);
	}

	deque_foreach(d, print_int, s_act);
	CuAssertStrEquals(tc, s_exp, s_act);
	deque_destroy(d);
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
	SUITE_ADD_TEST(suite, test_delete);
	SUITE_ADD_TEST(suite, test_delete_first);
	SUITE_ADD_TEST(suite, test_delete_last);

	return suite;
}
