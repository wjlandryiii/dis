/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "bytechunk.h"
#include "bytefields.h"
#include "error.h"

uint64_t
range_size(uint64_t A, uint64_t B){
	if(A <= B){
		return (B - A) + 1;
	} else {
		return 0;
	}
}
int
is_range_overlapping(uint64_t A, uint64_t B, uint64_t C, uint64_t D){
	if(A < C || D < A){
		return 0;
	} else {
		return 1;
	}
}

int
is_range_adjacent(uint64_t A, uint64_t B, uint64_t C, uint64_t D){
	if(B + 1 == C || D + 1 == A){
		return 1;
	} else {
		return 0;
	}
}


struct bytechunk *
new_bytechunk(uint64_t first, uint64_t last){
	struct bytechunk *chunk;
	uint64_t size;

	size = range_size(first, last);
	if(size){
		chunk = calloc(1, sizeof(*chunk));
		if(chunk){
			chunk->bc_first = first;
			chunk->bc_last = last;
			chunk->bc_bytes = calloc(size, sizeof(uint32_t));
			if(chunk->bc_bytes){
				memset(chunk->bc_bytes, 0, size*4);
			} else {
				free(chunk);
				chunk = NULL;
			}
		}
		return chunk;
	} else {
		fprintf(stderr, "new_bytechunk(): invalid size\n");
		return NULL;
	}
}



void
free_bytechunk(struct bytechunk *chunk){
	free(chunk);
}

static int
chunk_contains_addr(struct bytechunk *chunk, uint64_t addr){
	if(addr < chunk->bc_first){
		return 0;
	} else if(chunk->bc_last < addr){
		return 0;
	} else {
		return 1;
	}
}


int
chunk_get_byte_fields(struct bytechunk *chunk, uint64_t addr, uint32_t *fields_out){
	int i;

	if(chunk_contains_addr(chunk, addr)){
		i = addr - chunk->bc_first;
		if(fields_out){
			*fields_out = chunk->bc_bytes[i];
		}
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int
chunk_set_byte_fields(struct bytechunk *chunk, uint64_t addr, uint32_t fields){
	int i;

	if(chunk_contains_addr(chunk, addr)){
		i = addr - chunk->bc_first;
		chunk->bc_bytes[i] = fields;
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}


int
merge_chunks(struct bytechunk *before, struct bytechunk *after){
	uint64_t nold;
	uint64_t ngap;
	uint64_t nnew;
	uint64_t old_size;
	uint64_t gap_size;
	uint64_t new_size;
	uint32_t *buf;
	
	assert(before->bc_next == after);

	nold = range_size(before->bc_first, before->bc_last);
	old_size = nold * 4;
	nnew = range_size(before->bc_first, after->bc_last);
	new_size = nnew * 4;
	ngap = range_size(before->bc_last+1, after->bc_first-1);
	gap_size = ngap * 4;

	buf = realloc(before->bc_bytes, new_size);
	if(!buf){
		goto fail;
	}
	memset(&buf[nold], 0, gap_size);
	memcpy(&buf[nold+ngap], after->bc_bytes, new_size-gap_size-old_size);

	before->bc_last = after->bc_last;
	before->bc_next = after->bc_next;
	before->bc_bytes = buf;
	free_bytechunk(after);
	return 0;

fail:
	return -1;
}

int
expand_chunk_up(struct bytechunk *chunk, uint64_t last){
	uint64_t new_size;
	uint64_t old_size;
	uint32_t *buf;
	uint64_t n;

	n = range_size(chunk->bc_first, chunk->bc_last);
	old_size = n * sizeof(*(chunk->bc_bytes));
	n = range_size(chunk->bc_first, last);
	new_size = n * sizeof(*(chunk->bc_bytes));

	buf = realloc(chunk->bc_bytes, new_size);
	if(!buf){
		goto fail;
	}
	memset(&buf[old_size/4], 0, new_size - old_size);
	chunk->bc_bytes = buf;
	chunk->bc_last = last;
	return 0;

fail:
	return -1;
}


int
expand_chunk_down(struct bytechunk *chunk, uint64_t first){
	uint32_t *buf;
	size_t old_nelem;
	size_t new_nelem;

	old_nelem = range_size(chunk->bc_first, chunk->bc_last);
	new_nelem = range_size(first, chunk->bc_last);

	buf = realloc(chunk->bc_bytes, new_nelem * sizeof(uint32_t));
	if(!buf){
		goto fail;
	}

	memmove(&buf[new_nelem - old_nelem], buf, old_nelem * sizeof(uint32_t));
	memset(buf, 0, (new_nelem - old_nelem) * sizeof(uint32_t) );

	chunk->bc_bytes = buf;
	chunk->bc_first = first;
	return 0;

fail:
	return -1;
}

int
copy_bytes_from_chunk(struct bytechunk *chunk, uint64_t addr,
		uint8_t *buf, size_t size){
	int offset;
	uint32_t flags;
	uint i;
	
	offset = addr - chunk->bc_first;
	for(i = 0; i < size; i++){
		flags = chunk->bc_bytes[offset+i];
		if(is_value_valid(flags)){
			buf[i] = get_byte_value_field(flags);
		} else {
			dis_errno = DER_INVVALUE;
			return -1;
		}
	}
	return 0;
}




int
copy_bytes_to_chunk(struct bytechunk *chunk, uint64_t addr,
		uint8_t *buf, size_t size){
	int offset;
	uint32_t flags;
	int i;

	offset = addr - chunk->bc_first;
	for(i = 0; i < size; i++){
		flags = chunk->bc_bytes[offset+i];
		flags = set_byte_value_field(flags, buf[i]);
		flags = set_value_field(flags, VALUE_VALID);
		chunk->bc_bytes[offset+i] = flags;
	}
	return 0;
}

int
chunk_set_bytes(struct bytechunk *chunk, uint8_t c, uint64_t first, uint64_t last){
	uint32_t flags;
	int start;
	int stop;
	int i;

	start = first - chunk->bc_first;
	stop = (last - chunk->bc_first) + 1;

	for(i = start; i < stop; i++){
		flags = chunk->bc_bytes[i];
		flags = set_byte_value_field(flags, c);
		flags = set_value_field(flags, VALUE_VALID);
		chunk->bc_bytes[i] = flags;
	}
	return 0;
}


int chunk_get_byte_class(struct bytechunk *chunk, uint64_t addr, uint32_t *class_out){
	int i;

	i = addr - chunk->bc_first;
	if(class_out){
		*class_out = get_class_field(chunk->bc_bytes[i]);
	}
	return 0;
}

int chunk_set_byte_class(struct bytechunk *chunk, uint64_t addr, uint32_t class){
	int i;

	i = addr - chunk->bc_first;
	if(class){
		chunk->bc_bytes[i] = set_class_field(chunk->bc_bytes[i], class);
	}
	return 0;
}

int
is_chunk_range_class_unknown(struct bytechunk *chunk,
		uint64_t first, uint64_t last){
	uint32_t flags;
	int start;
	int stop;
	int i;

	start = first - chunk->bc_first;
	stop = (last - chunk->bc_first) + 1;

	for(i = start; i < stop; i++){
		flags = chunk->bc_bytes[i];
		if(!is_class_unknown(flags)){
			return 0;
		}
	}
	return 1;
}


int
chunk_item_head(struct bytechunk *chunk, uint64_t addr, uint64_t *head_out){
	uint32_t flags;
	int start;
	int i;

	start = addr - chunk->bc_first;

	flags = chunk->bc_bytes[start];

	if(is_class_unknown(flags)){
		return -1;
	}

	for(i = start; i >= 0; i--){
		flags = chunk->bc_bytes[i];
		if(!is_class_tail(flags)){
			if(head_out){
				*head_out = chunk->bc_first + i;
				return 0;
			}
		}
	}
	return -1;
}


int
chunk_item_end(struct bytechunk *chunk, uint64_t addr, uint64_t *end_out){
	uint32_t flags;
	int start;
	int end;
	int i;

	start = addr - chunk->bc_first;
	end = (chunk->bc_last - chunk->bc_first) + 1;

	for(i = start + 1; i < end; i++){
		flags = chunk->bc_bytes[i];
		if(!is_class_tail(flags)){
			if(end_out){
				*end_out = (chunk->bc_first + i) - 1;
				return 0;
			}
		}
	}
	if(end_out){
		*end_out = (chunk->bc_first + i) - 1;
	}
	return 0;
}



int
set_chunk_range_class_unknown(struct bytechunk *chunk,
		uint64_t first, uint64_t last){
	uint32_t flags;
	int start;
	int stop;
	int i;

	start = first - chunk->bc_first;
	stop = (last - chunk->bc_first) + 1;

	for(i = start; i < stop; i++){
		flags = chunk->bc_bytes[i];
		flags = set_class_field(flags, CLASS_UNKNOWN);
		chunk->bc_bytes[i] = flags;
	}
	return 0;
}

int
set_chunk_range_class_code(struct bytechunk *chunk,
		uint64_t first, uint64_t last){
	uint32_t flags;
	int start;
	int stop;
	int i;

	start = first - chunk->bc_first;
	stop = (last - chunk->bc_first) + 1;

	flags = chunk->bc_bytes[start];
	flags = set_class_field(flags, CLASS_CODE);
	chunk->bc_bytes[start] = flags;

	for(i = start + 1; i < stop; i++){
		flags = chunk->bc_bytes[i];
		flags = set_class_field(flags, CLASS_TAIL);
		chunk->bc_bytes[i] = flags;
	}
	return 0;
}


int
set_chunk_range_class_data(struct bytechunk *chunk,
		uint64_t first, uint64_t last){
	uint32_t flags;
	int start;
	int stop;
	int i;

	start = first - chunk->bc_first;
	stop = (last - chunk->bc_first) + 1;

	flags = chunk->bc_bytes[start];
	flags = set_class_field(flags, CLASS_DATA);
	chunk->bc_bytes[start] = flags;

	for(i = start + 1; i < stop; i++){
		flags = chunk->bc_bytes[i];
		flags = set_class_field(flags, CLASS_TAIL);
		chunk->bc_bytes[i] = flags;
	}
	return 0;
}

int
set_chunk_datatype_field(struct bytechunk *chunk, uint64_t addr, uint32_t datatype){
	uint32_t flags;
	int offset;

	offset = chunk->bc_first - addr;
	flags = chunk->bc_bytes[offset];
	flags = set_datatype_field(flags, datatype);
	chunk->bc_bytes[offset] = flags;
	return 0;
}
