/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "workspace.h"
#include "testrunner.h"


int test_newfree(void){
	struct workspace *ws;

	ws = new_workspace();
	free_workspace(ws);
	return 0;
}

struct test tests[] = {
	{"newfree", test_newfree},
	{NULL, NULL},
};

int main(int argc, char *argv[]){
	return run_tests(argc, argv, tests);
}
