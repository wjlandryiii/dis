/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "testrunner.h"

struct moduletests {
	char *mt_module;
	struct test *mt_tests;
};

#define MAX_MODULES	20
static struct moduletests moduletests[MAX_MODULES] = {{0}};
static int nmodules = 0;

void add_module_tests(char *module, struct test *tests){
	if(nmodules >= MAX_MODULES){
		fprintf(stderr, "MAX_MODULES REACHED\n");
		fprintf(stderr, "EDIT MAX_MODULES DEFINE IN %s\n", __FILE__);
		exit(1);
	}
	moduletests[nmodules].mt_module = module;
	moduletests[nmodules].mt_tests = tests;
	nmodules++;
	printf("Added module: %s\n", module);
}


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

int run_all_modules(void){
	int i;
	int r;

	for(i = 0; i < nmodules; i++){
		printf("\n\n");
		printf("STARTING MODULE: %s\n", moduletests[i].mt_module);
		printf("\n");
		r = run_all(moduletests[i].mt_tests);
		if(r){
			return r;
		}
	}
	return 0;
}

int main(int argc, char *argv[]){
	run_all_modules();
	return 0;
}
