/*
 * Copyright 2014 Joseph Landry
 */

#ifndef LOADER_H
#define LOADER_H

#include <stdio.h>

#include "workspace.h"

struct loader {
	char *ld_name;
	int (*ld_can_load_file)(FILE *f);
	int (*ld_load_file)(struct workspace *ws, FILE *f);
};

int
find_compatible_loaders(FILE *f, struct loader **compat_loaders, int count);

int
loader_load_file(struct loader *loader, struct workspace *ws, FILE *f);

int
loader_load_segment(struct workspace *ws, FILE *f, long offset, long file_size,
		uint64_t first, uint64_t last, int perms);

int can_load_file_raw(FILE *f);
int load_file_raw(struct workspace *ws, FILE *f);

int can_load_file_elf(FILE *f);
int load_file_elf(struct workspace *ws, FILE *f);

#endif
