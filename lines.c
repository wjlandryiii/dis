/*
 * Copyright 2014 Joseph Landry
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "stringstore.h"
#include "mapping.h"
#include "lines.h"


struct lines *
new_lines(struct stringstore *store, struct mapping *map){
	struct lines *lines;

	lines = calloc(1, sizeof(*lines));
	if(lines){
		lines->l_store = store;
		lines->l_map = map;
	}
	return lines;
}

int
free_lines(struct lines *lines){
	free(lines);
	return 0;
}

int
set_line(struct lines *lines, uint64_t addr, char *line){
	uint64_t value;
	int32_t sid;
	int r;	

	r = lookup_mapping(lines->l_map, addr, &value);
	if(r == 0){
		sid = value;
		update_string(lines->l_store, sid, line);	
	} else {
		sid = store_string(lines->l_store, line);
		if(sid < 0){
			fprintf(stderr, "store_string()\n");
			goto fail;
		}
		value = sid;
		set_mapping(lines->l_map, addr, value);
	}
	return 0;

fail:
	return -1;
}

int
get_line(struct lines *lines, uint64_t addr, char *buf, size_t size){
	uint64_t value;
	int32_t sid;
	int r;

	r = lookup_mapping(lines->l_map, addr, &value);
	if(r == 0){
		sid = value;
		if(sid < 0){
			fprintf(stderr, "invalid string id\n");
			goto fail;
		}
		return retrieve_string(lines->l_store, sid, buf, size);
	} else {
		goto fail;
	}

fail:
	return -1;
}
