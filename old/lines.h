/*
 * Copyright 2014 Joseph Landry
 */

#ifndef LINES_H
#define LINES_H

#include "stringstore.h"
#include "mapping.h"

struct lines {
	struct stringstore *l_store;
	struct mapping *l_map;
};


struct lines *
new_lines(struct stringstore *store, struct mapping *map);

int
free_lines(struct lines *lines);

int
set_line(struct lines *lines, uint64_t addr, char *line);

int
get_line(struct lines *lines, uint64_t addr, char *buf, size_t size);

#endif
