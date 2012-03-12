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
	deque_node_destroy(n);
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
	deque_node_destroy(n);
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
	deque_destroy(d);
}

static void print_int(void *i, void *str)
{
	char *s1 = str;
	char s2[1024];
	sprintf(s2, "%d -> ", *(int *) i);
	strcat(s1, s2);
}

void test_insert(CuTest *tc)
{
	deque_t *d = deque_create();
	int node_count = 3;
	deque_node_t *nodes[node_count];
	int vals[node_count];
	for (int i = 0; i < node_count; ++i) {
		vals[i] = i;
		nodes[i] = deque_node_create(&vals[i]);
	}

	deque_append(d, nodes[0]);
	deque_insert_after(nodes[0], nodes[1]);
	deque_insert_before(nodes[0], nodes[2]);

	CuAssertPtrEquals(tc, nodes[2], deque_first(d));
	CuAssertPtrEquals(tc, nodes[0], deque_next(nodes[2]));
	CuAssertPtrEquals(tc, nodes[1], deque_next(nodes[0]));
	CuAssertPtrEquals(tc, nodes[1], deque_last(d));

	deque_destroy(d);
}

void test_remove(CuTest *tc)
{
	deque_t *d = deque_create();
	int node_count = 3;
	deque_node_t *nodes[node_count];
	int vals[node_count];
	for (int i = 0; i < node_count; ++i) {
		vals[i] = i;
		nodes[i] = deque_node_create(&vals[i]);
		deque_append(d, nodes[i]);
	}

	deque_node_t *n0, *n1, *n2;
	n1 = deque_node_remove(d, nodes[1]);
	CuAssertPtrEquals(tc, nodes[1], n1);
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[2], deque_next(nodes[0]));
	CuAssertPtrEquals(tc, nodes[2], deque_last(d));

	n2 = deque_node_remove(d, nodes[2]);
	CuAssertPtrEquals(tc, nodes[2], n2);
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[0], deque_last(d));

	n0 = deque_node_remove(d, nodes[0]);
	CuAssertPtrEquals(tc, nodes[0], n0);
	CuAssertTrue(tc, deque_is_empty(d));

	deque_append(d, nodes[0]);
	deque_append(d, nodes[1]);
	deque_append(d, nodes[2]);

	CuAssertPtrEquals(tc, nodes[1], n1);
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[1], deque_next(nodes[0]));
	CuAssertPtrEquals(tc, nodes[2], deque_next(nodes[1]));
	CuAssertPtrEquals(tc, nodes[2], deque_last(d));

	n2 = deque_node_remove(d, nodes[2]);
	CuAssertPtrEquals(tc, nodes[2], n2);
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[1], deque_next(nodes[0]));
	CuAssertPtrEquals(tc, nodes[1], deque_last(d));

	n0 = deque_node_remove(d, nodes[0]);
	CuAssertPtrEquals(tc, nodes[0], n0);
	CuAssertPtrEquals(tc, nodes[1], deque_first(d));
	CuAssertPtrEquals(tc, nodes[1], deque_last(d));

	n1 = deque_node_remove(d, nodes[1]);
	CuAssertPtrEquals(tc, nodes[1], n1);
	CuAssertTrue(tc, deque_is_empty(d));

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
		nodes[i] = deque_node_create(&vals[i]);
		deque_append(d, nodes[i]);
	}

	deque_node_delete(d, nodes[1]);
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[2], deque_next(nodes[0]));
	CuAssertPtrEquals(tc, nodes[2], deque_last(d));

	deque_node_delete(d, nodes[2]);
	CuAssertPtrEquals(tc, nodes[0], deque_first(d));
	CuAssertPtrEquals(tc, nodes[0], deque_last(d));

	deque_node_delete(d, nodes[0]);
	CuAssertTrue(tc, deque_is_empty(d));

	deque_destroy(d);
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
	SUITE_ADD_TEST(suite, test_insert);
	SUITE_ADD_TEST(suite, test_foreach);
	SUITE_ADD_TEST(suite, test_remove);
	SUITE_ADD_TEST(suite, test_delete);

	return suite;
}
