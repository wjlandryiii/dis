/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "mapping.h"


void make_file(FILE *f){
	struct mapping *map;

	map = new_mapping();

	set_mapping(map, 0x08040000, 1);
	set_mapping(map, 0x08040001, 2);
	set_mapping(map, 0x08040002, 3);
	set_mapping(map, 0x08040003, 4);

	pack_mapping(map, f);
	free_mapping(map);
}

void load_file(FILE *f){
	struct mapping *map;
	uint64_t value;

	map = unpack_mapping(f);
	lookup_mapping(map, 0x08040000, &value);
	printf("%" PRIx64 "\n", value);

	lookup_mapping(map, 0x08040001, &value);
	printf("%" PRIx64 "\n", value);

	lookup_mapping(map, 0x08040002, &value);
	printf("%" PRIx64 "\n", value);
	
	lookup_mapping(map, 0x08040003, &value);
	printf("%" PRIx64 "\n", value);

	free_mapping(map);
}

int main(int argc, char *argv[]){
	FILE *f;

	f = tmpfile();
	make_file(f);
	fseek(f, 0, SEEK_SET);
	load_file(f);
	return 0;
}
