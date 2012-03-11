#include "CuTest.h"

#include <deque.c>

/* Tests */
void test_deque_create(CuTest *tc)
{
	deque_t *d = NULL;
	d = deque_create();
	CuAssertPtrNotNull(tc, d);

	deque_node_t *n = NULL;
	int i = 4;
	n = deque_create_node(&i);
	CuAssertPtrNotNull(tc, n);
	CuAssertIntEquals(tc, i, *(int *) deque_node_data(n));
}

void test_deque_append(CuTest *tc)
{
	deque_t *d = deque_create();
	CuAssertTrue(tc, deque_is_empty(d));

	int node_count = 10;
	deque_node_t *nodes[node_count];
	for (int i = 0; i < node_count; ++i) {
		nodes[i] = deque_create_node(&i);
		deque_append(d, nodes[i]);
		CuAssertTrue(tc, !deque_is_empty(d));
		CuAssertPtrEquals(tc, nodes[0], deque_start(d));
		CuAssertTrue(tc, deque_end(d, nodes[i]));

		for (int j = i; j > 0; --j)
			CuAssertPtrEquals(tc, nodes[j],
					  deque_next(d, nodes[j-1]));
	}
}

void test_deque_prepend(CuTest *tc)
{
	deque_t *d = deque_create();
	CuAssertTrue(tc, deque_is_empty(d));

	int node_count = 30;
	deque_node_t *nodes[node_count];
	for (int i = 0; i < node_count; ++i) {
		nodes[i] = deque_create_node(&i);
		deque_prepend(d, nodes[i]);
		CuAssertTrue(tc, !deque_is_empty(d));
		CuAssertPtrEquals(tc, nodes[i], deque_start(d));
		CuAssertTrue(tc, deque_end(d, nodes[0]));

		for (int j = i; j > 0; --j)
			CuAssertPtrEquals(tc, nodes[j-1],
					  deque_next(d, nodes[j]));
	}
}

/* Suite */
CuSuite* test_deque_get_suite()
{
	CuSuite *suite = CuSuiteNew();

	/* SUITE_ADD_TEST calls */
	SUITE_ADD_TEST(suite, test_deque_create);
	SUITE_ADD_TEST(suite, test_deque_append);
	SUITE_ADD_TEST(suite, test_deque_prepend);

	return suite;
}
