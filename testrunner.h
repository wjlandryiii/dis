/*
 * Copyright 2014 Joseph Landry
 */

#ifndef TESTRUNNER_H
#define TESTRUNNER_H

struct test {
	char *te_name;
	int (*te_fn)(void);
};

void add_module_tests(char *module, struct test *tests);

#define FAIL_IF(A) if(A){ fail_test(__FILE__, __LINE__); return -1; }

void fail_test(char *file, int line);
int run_tests(int argc, char *argv[], struct test *tests);

#endif
