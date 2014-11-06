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

void test_workspace_init(void) __attribute__ ((constructor));
void test_workspace_init(void){
	add_module_tests("workspace", tests);
}
