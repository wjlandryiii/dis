/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "lines.h"

struct testdata {
	uint64_t addr;
	char *line;
};

struct testdata data[] = {
	{ 0x08040000, "push ebp"},
	{ 0x08040001, "mov ebp, esp"},
	{ 0x08040002, "mov eax, 1"},
	{ 0x08040003, "pop ebp" },
	{ 0x08040004, "ret" },
};

void make_file(FILE *f){
	struct stringstore *store;
	struct mapping *map;
	struct lines *lines;
	int i;
	int r;

	store = new_stringstore();
	map = new_mapping();
	lines = new_lines(store, map);

	for(i = 0; i < sizeof(data)/sizeof(data[0]); i++){
		printf("store %08" PRIx64 " => %s\n", data[i].addr, data[i].line);
		r = set_line(lines, data[i].addr, data[i].line);
		if(r != 0){
			fprintf(stderr, "set_line()\n");
			exit(1);
		}
	}

	pack_stringstore(store, f);
	pack_mapping(map, f);
	free_lines(lines);
	free_mapping(map);
	free_stringstore(store);
}

void load_file(FILE *f){
	struct stringstore *store;
	struct mapping *map;
	struct lines *lines;
	int i;
	int r;

	char buf[1024];

	store = unpack_stringstore(f);
	map = unpack_mapping(f);
	lines = new_lines(store, map);

	for(i = 0; i < sizeof(data)/sizeof(data[0]); i++){
		r = get_line(lines, data[i].addr, buf, sizeof(buf));
		if(r < 0){
			fprintf(stderr, "get_line()\n");
			exit(1);
		}
		printf("%08" PRIx64 ": %s\n", data[i].addr, buf);
	}
	free_lines(lines);
	free_mapping(map);
	free_stringstore(store);

}

int main(int argc, char *argv[]){
	FILE *f;

	f = tmpfile();

	make_file(f);
	fseek(f, 0, SEEK_SET);
	printf("\nLOADING...\n");
	load_file(f);
	return 0;
}
