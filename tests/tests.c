#include <stdio.h>
#include "CuTest.h"

/* *_get_suite() declarations... */
CuSuite* test_deque_get_suite();

void run_all_tests(void) {
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	/* CuSuiteAddSuite() calls... */
	CuSuiteAddSuite(suite, test_deque_get_suite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}

int main(int argc, const char *argv[])
{
	run_all_tests();
	return 0;
}
