/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "loader.h"
#include "bytes.h"

struct loader loaders[] = {
	{"elf", can_load_file_elf, load_file_elf},
	{"raw", can_load_file_raw, load_file_raw},
};


int
find_compatible_loaders(FILE *f, struct loader **compat_loaders, int count){
	int nloaders;
	int i;
	int n;

	n = 0;
	nloaders = sizeof(loaders)/sizeof(loaders[0]);
	for(i = 0; i < nloaders; i++){
		fseek(f, 0, SEEK_SET);
		if(loaders[i].ld_can_load_file(f)){
			if(n < count){
				compat_loaders[n++] = &loaders[i];
			}
		}
	}
	return n;
}

int
loader_load_file(struct loader *loader, struct workspace *ws, FILE *f){
	return loader->ld_load_file(ws, f);
}

int
loader_load_segment(struct workspace *ws, FILE *f, long offset, long file_size,
		uint64_t first, uint64_t last, int perms){
	uint8_t buf[512];
	struct bytes *bytes = ws->ws_bytes;
	int nbytes;
	int r;
	long i;

	r = enable_bytes(bytes, first, last);
	if(r){
		goto fail;
	}

	fseek(f, offset, SEEK_SET);
	for(i = 0; i < file_size; i+=nbytes){
		if(file_size - i > sizeof(buf)){
			nbytes = sizeof(buf);
		} else {
			nbytes = file_size - i;
		}

		r = fread(buf, sizeof(uint8_t), nbytes, f);
		if(r != nbytes){
			fprintf(stderr, "fread()\n");
			goto fail;
		}
		r = copy_to_bytes(bytes, first+i, buf, nbytes);
		if(r < 0){
			fprintf(stderr, "copy_from_bytes()\n");
			goto fail;
		}
	}
	return 0;
fail:
	return -1;
}

#include "loader-raw.c"
#include "loader-elf.c"
