/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bytes.h"


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

uint64_t
range_size(uint64_t A, uint64_t B){
	if(A <= B){
		return (B - A) + 1;
	} else {
		return 0;
	}
}


static struct bytechunk *
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
		fprintf(stderr, "new_bytechunk(%016llx, %016llx): "
				"invalid size\n",
				first, last);
		return NULL;
	}
}

void
free_chunk(struct bytechunk *chunk){
	free(chunk);
}


int
is_empty_range(struct bytes *bytes, uint64_t first, uint64_t last){
	struct bytechunk *chunk;

	for(chunk = bytes->b_chunks; chunk != NULL; chunk = chunk->bc_next){
		if(is_range_overlapping(first, last,
					chunk->bc_first, chunk->bc_last)){
			return 0;
		}
	}
	return 1;
}


struct bytes *
new_bytes(void){
	struct bytes *bytes;

	bytes = calloc(1, sizeof(*bytes));
	if(bytes){
	}
	return bytes;
}


void
free_bytes(struct bytes *bytes){
	struct bytechunk *chunk;
	struct bytechunk *next;

	for(chunk = bytes->b_chunks; chunk != NULL; chunk = next){
		next = chunk->bc_next;
		free_chunk(chunk);
	}
	free(bytes);
}


static struct bytechunk *
insert_chunk(struct bytechunk *node, struct bytechunk *chunk){
	if(node == NULL){
		return chunk;
	} else if(chunk->bc_first < node->bc_first){
		chunk->bc_next = node;
		return chunk;
	} else {
		node->bc_next = insert_chunk(node->bc_next, chunk);
		return node;
	}
}

static int
find_adjacent_chunks(struct bytes *bytes, uint64_t first, uint64_t last,
		struct bytechunk **before_out, struct bytechunk **after_out){
	struct bytechunk *chunk;
	struct bytechunk *before;
	struct bytechunk *after;
	int count;

	count = 0;
	before = after = NULL;
	chunk = bytes->b_chunks;
	while(chunk){
		if(chunk->bc_last + 1 == first){
			before = chunk;
			count++;
		} else if(last + 1 == chunk->bc_first){
			after = chunk;
			count++;
		}
		chunk = chunk->bc_next;
	}
	if(before_out){
		*before_out = before;
	}
	if(after_out){
		*after_out = after;
	}
	return count;
}

static int
merge_chunks(struct bytechunk *before, struct bytechunk *after){
	uint64_t nold;
	uint64_t ngap;
	uint64_t nnew;
	uint64_t old_size;
	uint64_t gap_size;
	uint64_t new_size;
	uint32_t *buf;
	uint64_t n;
	
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
	free_chunk(after);
	return 0;

fail:
	return -1;
}

static int
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

static int
expand_chunk_down(struct bytechunk *chunk, uint64_t first){
	uint64_t new_size;
	uint64_t old_size;
	uint32_t *buf;
	uint64_t n;

	n = range_size(chunk->bc_first, chunk->bc_last);
	old_size = n * sizeof(*(chunk->bc_bytes));
	n = range_size(first, chunk->bc_last);
	new_size = n * sizeof(*(chunk->bc_bytes));

	buf = realloc(chunk->bc_bytes, new_size);
	if(!buf){
		goto fail;
	}

	memmove(&buf[(new_size - old_size)/4], buf, old_size);
	memset(&buf[old_size/4], 0, new_size - old_size);

	chunk->bc_bytes = buf;
	chunk->bc_first = first;
	return 0;

fail:
	return -1;
}

int
enable_bytes(struct bytes *bytes, uint64_t first, uint64_t last){
	struct bytechunk *chunk;
	struct bytechunk *before;
	struct bytechunk *after;

	if(is_empty_range(bytes, first, last)){
		before = after = NULL;
		find_adjacent_chunks(bytes, first, last, &before, &after);
		if(before && after){
			return merge_chunks(before, after);
		} else if(before){
			return expand_chunk_up(before, last);
		} else if(after){
			return expand_chunk_down(after, first);
		} else {
			chunk = new_bytechunk(first, last);
			if(chunk){
				chunk = insert_chunk(bytes->b_chunks, chunk);
				bytes->b_chunks = chunk;
			} else {
				goto fail;
			}
		}
	} else {
		goto fail; // TODO
	}
	return 0;
fail:
	return -1;
}

struct bytechunk *
first_chunk(struct bytes *bytes){
	return bytes->b_chunks;
}

struct bytechunk *
next_chunk(struct bytechunk *chunk){
	return chunk->bc_next;
}

struct bytechunk *
find_chunk_containing_addr(struct bytes *bytes, uint64_t addr){
	struct bytechunk *chunk;

	chunk = first_chunk(bytes);
	while(chunk){
		if(chunk->bc_first <= addr && addr <= chunk->bc_last){
			return chunk;
		}
		chunk = next_chunk(chunk);
	}
	return NULL;
}


static int
real_copy_to_bytes(struct bytechunk *chunk, uint64_t addr,
		uint8_t *buf, size_t size){
	int offset;
	uint32_t flags;
	int i;

	offset = addr - chunk->bc_first;
	for(i = 0; i < size; i++){
		flags = chunk->bc_bytes[offset+i];
		flags = set_byte_field(flags, buf[i]);
		flags = set_value_field(flags, VALUE_VALID);
		chunk->bc_bytes[offset+i] = flags;
	}
	return 0;
}

int
copy_to_bytes(struct bytes *bytes, uint64_t addr, uint8_t *buf, size_t size){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		if(addr + size - 1 <= chunk->bc_last){
			return real_copy_to_bytes(chunk, addr, buf, size);
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

static int
real_copy_from_bytes(struct bytechunk *chunk, uint64_t addr,
		uint8_t *buf, size_t size){
	int offset;
	uint32_t flags;
	uint i;
	
	offset = addr - chunk->bc_first;
	for(i = 0; i < size; i++){
		flags = chunk->bc_bytes[offset+i];
		if(is_value_valid(flags)){
			buf[i] = get_byte_field(flags);
		} else {
			return -1;
		}
	}
	return 0;
}

int
copy_from_bytes(struct bytes *bytes, uint64_t addr, uint8_t *buf, size_t size){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		if(addr + size - 1 <= chunk->bc_last){
			return real_copy_from_bytes(chunk, addr, buf, size);
		} else {
			return -1;
		}
	} else {
		return -1;
	}

}

static int
is_chunk_range_class_unkown(struct bytechunk *chunk,
		uint64_t first, uint64_t last){
	int start;
	int stop;
	uint32_t flags;
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

static int
is_range_in_chunk(struct bytechunk *chunk, uint64_t first, uint64_t last){
	if(chunk->bc_first <= first && last <= chunk->bc_last){
		return 1;
	} else {
		return 0;
	}
}

static int
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

static int
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

static int
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

static int
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
set_class_unknown(struct bytes *bytes, uint64_t first, uint64_t last){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, first);
	if(chunk){
		if(is_range_in_chunk(chunk, first, last)){
			return set_chunk_range_class_unknown(chunk, first, last);
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}



int
set_class_code(struct bytes *bytes, uint64_t first, uint64_t last){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, first);
	if(chunk){
		if(is_chunk_range_class_unknown(chunk, first, last)){
			return set_chunk_range_class_code(chunk, first, last);
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

int
set_class_data(struct bytes *bytes, uint64_t first, uint64_t last){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, first);
	if(chunk){
		if(is_chunk_range_class_unknown(chunk, first, last)){
			return set_chunk_range_class_data(chunk, first, last);
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}


/*****************************************************************************/



int create_bytes(char *filename, uint32_t base_addr, struct bytes *bytes){
	FILE *f;
	long fsize;

	uint32_t addr;
	uint32_t count;
	uint32_t *byte_flags;
	uint32_t flags;
	int i;
	int c;

	f = fopen(filename, "r");
	if(f == NULL){
		fprintf(stderr, "fopen(\"%s\")\n", filename);
		goto fail;
	}

	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	if(fsize <= 0){
		fprintf(stderr, "couldn't find file size\n");
		goto fail;
	}
	
	addr = base_addr;
	count = fsize;
	byte_flags = calloc(sizeof(*byte_flags), count);

	for(i = 0; i < count; i++){
		c = fgetc(f);
		if(c == EOF){
			fprintf(stderr, "unexpected end of file\n");
			goto fail;
		}
		flags = 0;
		flags = set_byte_field(flags, c);
		flags = set_value_field(flags, VALUE_VALID);
		byte_flags[i] = flags;
	}
	fclose(f);

	if(bytes){
		bytes->addr = addr;
		bytes->count = count;
		bytes->byte_flags = byte_flags;
	} else {
		free(byte_flags);
	}
		
	return 0;
fail:
	return -1;
}


int load_bytes(char *filename, struct bytes *bytes){
	FILE *f;
	uint32_t addr;
	uint32_t count;
	uint32_t *byte_flags;
	size_t r;

	f = fopen(filename, "r");
	if(f == NULL){
		fprintf(stderr, "fopen(\"%s\")\n", filename);
		goto fail;
	}

	r = fread(&addr, sizeof(addr), 1, f);
	if(r != 1){
		fprintf(stderr, "fread()\n");
		goto fail;
	}

	r = fread(&count, sizeof(count), 1, f);
	if(r != 1){
		fprintf(stderr, "fread()\n");
		goto fail;
	}
	byte_flags = calloc(count, sizeof(uint32_t));
	if(byte_flags == NULL){
		fprintf(stderr, "callod()\n");
		goto fail;
	}

	r = fread(byte_flags, sizeof(uint32_t), count, f);
	if(r != count){
		fprintf(stderr, "fread()\n");
	}
	fclose(f);

	if(bytes){
		bytes->addr = addr;
		bytes->count = count;
		bytes->byte_flags = byte_flags;
	} else {
		free(byte_flags);
	}
	return 0;

fail:
	return -1;
}

int save_bytes(char *filename, struct bytes *bytes){
	FILE *f;
	size_t r;

	f = fopen(filename, "w+");
	if(f == NULL){
		fprintf(stderr, "fopen(\"%s\")\n", filename);
		goto fail;
	}

	r = fwrite(&bytes->addr, sizeof(bytes->addr), 1, f);
	if(r != 1){
		fprintf(stderr, "fwrite()\n");
		goto fail;
	}
	r = fwrite(&bytes->count, sizeof(bytes->count), 1, f);
	if(r != 1){
		fprintf(stderr, "fwrite()\n");
		goto fail;
	}
	r = fwrite(bytes->byte_flags, sizeof(*(bytes->byte_flags)),
			bytes->count, f);
	if(r != bytes->count){
		fprintf(stderr, "fwrite()\n");
		goto fail;
	}
	fclose(f);
	return 0;

fail:
	return -1;
}
