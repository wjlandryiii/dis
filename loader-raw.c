/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "workspace.h"
#include "loader.h"

int
can_load_file_raw(FILE *f){
	return 1;
}


int
load_file_raw(struct workspace *ws, FILE *f){
	long size;
	uint64_t first;
	uint64_t last;

	fseek(f, 0, SEEK_END);
	size = ftell(f);

	first = 0x10000;
	last = (first + size) - 1;

	return loader_load_segment(ws, f, 0, size, first, last, 0);
}
