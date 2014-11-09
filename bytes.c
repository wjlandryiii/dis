/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bytes.h"
#include "bytechunk.h"
#include "bytefields.h"
#include "error.h"



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
		free_bytechunk(chunk);
	}
	free(bytes);
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

struct bytechunk *first_chunk(struct bytes *bytes){
	return bytes->b_chunks;
}


struct bytechunk *next_chunk(struct bytechunk *chunk){
	return chunk->bc_next;
}


struct bytechunk *find_chunk_containing_addr(struct bytes *bytes, uint64_t addr){
	struct bytechunk *chunk;

	chunk = first_chunk(bytes);
	while(chunk){
		if(chunk_contains_addr(chunk, addr)){
			return chunk;
		}
		chunk = next_chunk(chunk);
	}
	dis_errno = DER_NOTFOUND;
	return NULL;
}


int get_byte_fields(struct bytes *bytes, uint64_t addr, uint32_t *fields_out){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_get_byte_fields(chunk, addr, fields_out);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}


int set_byte_fields(struct bytes *bytes, uint64_t addr, uint32_t fields){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_set_byte_fields(chunk, addr, fields);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}



/*********************   Value   *********************************************/

int set_bytes(struct bytes *bytes, uint8_t c, uint64_t first, uint64_t last){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, first);
	if(chunk){
		return chunk_set_bytes(chunk, c, first, last);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}


int
copy_from_bytes(struct bytes *bytes, uint64_t addr, uint8_t *buf, size_t size){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return copy_bytes_from_chunk(chunk, addr, buf, size);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}

int
bytes_get_byte(struct bytes *bytes, uint64_t addr, uint8_t *byte_out){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_get_byte(chunk, addr, byte_out);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}

int
bytes_get_word(struct bytes *bytes, uint64_t addr, uint16_t *word_out){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_get_word(chunk, addr, word_out);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}

int
bytes_get_dword(struct bytes *bytes, uint64_t addr, uint32_t *dword_out){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_get_dword(chunk, addr, dword_out);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}

int
bytes_get_qword(struct bytes *bytes, uint64_t addr, uint64_t *qword_out){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_get_qword(chunk, addr, qword_out);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}


int
copy_to_bytes(struct bytes *bytes, uint64_t addr, uint8_t *buf, size_t size){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return copy_bytes_to_chunk(chunk, addr, buf, size);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}

int
bytes_put_byte(struct bytes *bytes, uint64_t addr, uint8_t value){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_set_byte(chunk, addr, value);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}

int
bytes_put_word(struct bytes *bytes, uint64_t addr, uint16_t value){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_set_word(chunk, addr, value);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}

int
bytes_put_dword(struct bytes *bytes, uint64_t addr, uint32_t value){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_set_dword(chunk, addr, value);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}

int
bytes_put_qword(struct bytes *bytes, uint64_t addr, uint64_t value){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_set_qword(chunk, addr, value);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}



/*******************************   Class   ***********************************/

int bytes_get_byte_class(struct bytes *bytes, uint64_t addr, uint32_t *class_out){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_get_byte_class(chunk, addr, class_out);	
	} else {
		dis_errno = DER_INVADDR;
		return -1;	
	}
}

int bytes_set_byte_class(struct bytes *bytes, uint64_t addr, uint32_t class){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_set_byte_class(chunk, addr, class);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}

int
set_class_unknown(struct bytes *bytes, uint64_t first, uint64_t last){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, first);
	if(chunk){
		return set_chunk_range_class_unknown(chunk, first, last);	
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}



int
set_class_code(struct bytes *bytes, uint64_t first, uint64_t last){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, first);
	if(chunk){
		return set_chunk_range_class_code(chunk, first, last);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}



int
set_class_data(struct bytes *bytes, uint64_t first, uint64_t last){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, first);
	if(chunk){
		return set_chunk_range_class_data(chunk, first, last);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}



/**************************   Datatype   *************************************/



int get_bytes_datatype(struct bytes *bytes, uint64_t addr, uint32_t *datatype_out){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_get_byte_datatype(chunk, addr, datatype_out);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}

int set_bytes_datatype(struct bytes *bytes, uint64_t addr, uint32_t datatype){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_set_byte_datatype(chunk, addr, datatype);
	} else {
		dis_errno = DER_INVADDR;
		return -1;
	}
}


/* TODO delete these */
int
set_bytes_datatype_byte(struct bytes *bytes, uint64_t addr){
	assert(0);
}

int
set_bytes_datatype_word(struct bytes *bytes, uint64_t addr){
	assert(0);
}

int
set_bytes_datatype_dword(struct bytes *bytes, uint64_t addr){
	assert(0);
}

int
set_bytes_datatype_qword(struct bytes *bytes, uint64_t addr){
	assert(0);
}



/**********************   Items   ********************************************/



int
item_head(struct bytes *bytes, uint64_t addr, uint64_t *head_out){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_item_head(chunk, addr, head_out);		
	} else {
		return -1;
	}
}


int
item_end(struct bytes *bytes, uint64_t addr, uint64_t *end_out){
	struct bytechunk *chunk;

	chunk = find_chunk_containing_addr(bytes, addr);
	if(chunk){
		return chunk_item_end(chunk, addr, end_out);		
	} else {
		return -1;
	}
}
