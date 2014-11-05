/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "testrunner.h"

void fail_test(char *file, int line){
	printf("[FAILED]\n");
	printf("%s:%d\n", file, line);
}

static int
run_test(struct test *test){
	int r;

	printf("%-60s", test->te_name);
	fflush(stdout);
	r = test->te_fn();
	if(!r){
		printf("[OK]\n");
	} else {
		printf("\n\n!!!!!  FAILED !!!!!\n");
	}
	return r;
}


static int
run_all(struct test *tests){
	struct test *test;
	int r;

	test = tests;
	while(test->te_name){
		r = run_test(test);
		if(r){
			return r;
		}
		test += 1;
	}
	return 0;
}


static struct test *
find_test(struct test *tests, char *name){
	struct test *test;

	test = tests;
	while(test->te_name){
		if(strcmp(test->te_name, name) == 0){
			return test;
		} else {
			test += 1;
		}
	}
	return NULL;
}

int
list_tests(struct test *tests){
	struct test *test;

	test = tests;
	while(test->te_name){
		printf("%s\n", test->te_name);
		test += 1;
	}
	return 0;
}


int run_tests(int argc, char *argv[], struct test *tests){
	struct test *test;

	if(argc < 2){
		return run_all(tests);
	} else {
		if(strcmp(argv[1], "-l") == 0){
			return list_tests(tests);
		} else {
			test = find_test(tests, argv[1]);
			if(test){
				return run_test(test);
			} else {
				fprintf(stderr, "test: %s not found\n",
						argv[1]);
				return 1;
			}
		}
	}
}
