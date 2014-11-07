/*
 * Copyright 2014 Joseph Landry
 */

#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include "error.h"

struct test {
	char *te_name;
	int (*te_fn)(void);
};

void add_module_tests(char *module, struct test *tests);

#define FAIL_IF(A) if(A){\
	fail_test(__FILE__, __LINE__);\
	return -1; }

#define FAIL_IF_ERR(A) if(A != 0){\
	fail_test(__FILE__, __LINE__);\
	printf("Error: %d\n", dis_errno);\
	if(dis_errno < DER_NERRORS){\
		printf("Message %s\n", dis_errstr[dis_errno]);\
	}\
       	return -1; }

void fail_test(char *file, int line);
int run_tests(int argc, char *argv[], struct test *tests);

#endif
