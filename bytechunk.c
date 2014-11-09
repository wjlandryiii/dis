/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

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

void dump_chunk(struct bytechunk *chunk){
	int i;

	printf("\n\n");
	printf("CHUNK: %p\n", chunk);
	printf("chunk->bc_next:  %p\n", chunk->bc_next);
	printf("chunk->bc_first: %016" PRIx64 "\n", chunk->bc_first);
	printf("chunk->bc_last:  %016" PRIx64 "\n", chunk->bc_last);
	printf("chunk->bc_bytes:\n");

	for(i = 0; i <= chunk->bc_last - chunk->bc_first; i++){
		printf("\t%3d\t%08" PRIx32 "\n", i, chunk->bc_bytes[i]);
	}
	printf("\n");
}

int
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
chunk_contains_range(struct bytechunk *chunk, uint64_t first, uint64_t last){
	if(chunk->bc_first <= first && last <= chunk->bc_last){
		return 1;
	} else {
		return 0;
	}
}

static int
chunk_addr_to_index(struct bytechunk  *chunk, uint64_t addr){
	int i;
	i = addr - chunk->bc_first;
	assert(0 <= i);
	return i;
}


int
chunk_get_byte_fields(struct bytechunk *chunk, uint64_t addr, uint32_t *fields_out){
	int i;

	if(chunk_contains_addr(chunk, addr)){
		i = chunk_addr_to_index(chunk, addr);
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
chunk_put_byte_fields(struct bytechunk *chunk, uint64_t addr, uint32_t fields){
	int i;

	if(chunk_contains_addr(chunk, addr)){
		i = chunk_addr_to_index(chunk, addr);
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


/****************************    Value Field     *****************************/

int chunk_get_byte(struct bytechunk *chunk, uint64_t addr, uint8_t *byte_out){
	uint32_t fields;
	int i;

	if(chunk_contains_addr(chunk, addr)){
		i = chunk_addr_to_index(chunk, addr);
		fields = chunk->bc_bytes[i];
		if(is_value_valid(fields)){
			if(byte_out){
				*byte_out = get_byte_value_field(fields);
			}
			return 0;
		} else {
			dis_errno = DER_INVVALUE;
			return -1;
		}
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int chunk_put_byte(struct bytechunk *chunk, uint64_t addr, uint8_t byte){
	uint32_t fields;
	int i;

	if(chunk_contains_addr(chunk, addr)){
		i = chunk_addr_to_index(chunk, addr);
		fields = chunk->bc_bytes[i];
		fields = set_byte_value_field(fields, byte);
		fields = set_value_field(fields, VALUE_VALID);
		chunk->bc_bytes[i] = fields;
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}


int
copy_from_chunk(struct bytechunk *chunk, uint64_t addr,
		uint8_t *buf, size_t size){
	int offset;
	uint32_t flags;
	int i;

	if(chunk_contains_range(chunk, addr, (addr + size) - 1)){	
		offset = chunk_addr_to_index(chunk, addr);
		for(i = 0; i < size; i++){
			flags = chunk->bc_bytes[offset+i];
			if(is_value_valid(flags)){
				buf[i] = get_byte_value_field(flags);
			} else {
				dis_errno = DER_INVVALUE;
				return -1;
			}
		}
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
	return 0;
}




int
copy_to_chunk(struct bytechunk *chunk, uint64_t addr,
		uint8_t *buf, size_t size){
	int offset;
	uint32_t flags;
	int i;

	if(chunk_contains_range(chunk, addr, (addr + size) - 1)){
		offset = chunk_addr_to_index(chunk, addr);
		for(i = 0; i < size; i++){
			flags = chunk->bc_bytes[offset+i];
			flags = set_byte_value_field(flags, buf[i]);
			flags = set_value_field(flags, VALUE_VALID);
			chunk->bc_bytes[offset+i] = flags;
		}
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int
chunk_set_range_values(struct bytechunk *chunk, uint8_t c, uint64_t first, uint64_t last){
	uint32_t flags;
	int start;
	int stop;
	int i;

	if(chunk_contains_range(chunk, first, last)){
		start = chunk_addr_to_index(chunk, first);
		stop = chunk_addr_to_index(chunk, last) + 1;

		for(i = start; i < stop; i++){
			flags = chunk->bc_bytes[i];
			flags = set_byte_value_field(flags, c);
			flags = set_value_field(flags, VALUE_VALID);
			chunk->bc_bytes[i] = flags;
		}
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int chunk_get_word(struct bytechunk *chunk, uint64_t addr, uint16_t *word_out){
	uint16_t word;
	register int r;

	r = copy_from_chunk(chunk, addr, (void *)&word, sizeof(word));
	if(!r){
		if(word_out){
			*word_out = word;
		}
		return 0;
	} else {
		return r;
	}
}
int chunk_get_dword(struct bytechunk *chunk, uint64_t addr, uint32_t *dword_out){
	uint32_t dword;
	register int r;

	r = copy_from_chunk(chunk, addr, (void *)&dword, sizeof(dword));
	if(!r){
		if(dword_out){
			*dword_out = dword;
		}
		return 0;
	} else {
		return r;
	}
}
int chunk_get_qword(struct bytechunk *chunk, uint64_t addr, uint64_t *qword_out){
	uint64_t qword;
	register int r;

	r = copy_from_chunk(chunk, addr, (void *)&qword, sizeof(qword));
	if(!r){
		if(qword_out){
			*qword_out = qword;
		}
		return 0;
	} else {
		return r;
	}
}

int chunk_put_word(struct bytechunk *chunk, uint64_t addr, uint16_t word){
	return copy_to_chunk(chunk, addr, (void *)&word, sizeof(word));
}

int chunk_put_dword(struct bytechunk *chunk, uint64_t addr, uint32_t dword){
	return copy_to_chunk(chunk, addr, (void *)&dword, sizeof(dword));
}

int chunk_put_qword(struct bytechunk *chunk, uint64_t addr, uint64_t qword){
	return copy_to_chunk(chunk, addr, (void *)&qword, sizeof(qword));
}


/******************************   Class Field   ******************************/


int chunk_get_byte_class(struct bytechunk *chunk, uint64_t addr, uint32_t *class_out){
	int i;

	if(chunk_contains_addr(chunk, addr)){
		i = chunk_addr_to_index(chunk, addr);
		if(class_out){
			*class_out = get_class_field(chunk->bc_bytes[i]);
		}
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int is_chunk_range_class(struct bytechunk *chunk, uint64_t first, uint64_t last, uint32_t class){
	uint32_t fields;
	int start;
	int stop;
	int i;

	if(chunk_contains_range(chunk, first, last)){
		start = chunk_addr_to_index(chunk, first);
		stop = chunk_addr_to_index(chunk, last) + 1;

		for(i = start; i < stop; i++){
			fields = chunk->bc_bytes[i];
			if(get_class_field(fields) != class){
				return 0;
			}
		}
		return 1;
	} else {
		return 0;
	}
}


int is_chunk_range_class_unknown(struct bytechunk *chunk, uint64_t first, uint64_t last){
	return is_chunk_range_class(chunk, first, last, CLASS_UNKNOWN);
}

int is_chunk_range_class_code(struct bytechunk *chunk, uint64_t first, uint64_t last){
	return is_chunk_range_class(chunk, first, last, CLASS_CODE);
}


int is_chunk_range_class_data(struct bytechunk *chunk, uint64_t first, uint64_t last){
	return is_chunk_range_class(chunk, first, last, CLASS_DATA);
}


int is_chunk_range_class_tail(struct bytechunk *chunk, uint64_t first, uint64_t last){
	return is_chunk_range_class(chunk, first, last, CLASS_TAIL);
}




int chunk_set_byte_class(struct bytechunk *chunk, uint64_t addr, uint32_t class){
	int i;

	if(chunk_contains_addr(chunk, addr)){
		i = chunk_addr_to_index(chunk, addr);
		chunk->bc_bytes[i] = set_class_field(chunk->bc_bytes[i], class);
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int chunk_set_range_class(struct bytechunk *chunk, uint64_t first, uint64_t last, uint32_t class){
	uint32_t fields;
	int start;
	int stop;
	int i;

	if(chunk_contains_range(chunk, first, last)){
		start = chunk_addr_to_index(chunk, first);
		stop = chunk_addr_to_index(chunk, last) + 1;

		for(i = start; i < stop; i++){
			fields = chunk->bc_bytes[i];
			fields = set_class_field(fields, class);
			chunk->bc_bytes[i] = fields;
		}
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int chunk_set_range_class_unknown(struct bytechunk *chunk, uint64_t first, uint64_t last){
	return chunk_set_range_class(chunk, first, last, CLASS_UNKNOWN);
}

int chunk_set_range_class_code(struct bytechunk *chunk, uint64_t first, uint64_t last){
	return chunk_set_range_class(chunk, first, last, CLASS_CODE);
}

int chunk_set_range_class_data(struct bytechunk *chunk, uint64_t first, uint64_t last){
	return chunk_set_range_class(chunk, first, last, CLASS_DATA);
}

int chunk_set_range_class_tail(struct bytechunk *chunk, uint64_t first, uint64_t last){
	return chunk_set_range_class(chunk, first, last, CLASS_TAIL);
}

/*******************************   Datatype Field   **************************/

int chunk_get_byte_datatype(struct bytechunk *chunk, uint64_t addr, uint32_t *datatype_out){
	uint32_t fields;
	int i;

	if(chunk_contains_addr(chunk, addr)){
		i = chunk_addr_to_index(chunk, addr);
		fields = chunk->bc_bytes[i];
		if(datatype_out){
			*datatype_out = get_datatype_field(fields);
		}
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int
chunk_put_byte_datatype(struct bytechunk *chunk, uint64_t addr, uint32_t datatype){
	uint32_t flags;
	int offset;

	if(chunk_contains_addr(chunk, addr)){
		offset = chunk_addr_to_index(chunk, addr);
		flags = chunk->bc_bytes[offset];
		flags = set_datatype_field(flags, datatype);
		chunk->bc_bytes[offset] = flags;
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}


/**************************    Items     *************************************/

int chunk_first_item(struct bytechunk *chunk, uint64_t *first_out){
	int i;
	int start;
	int stop;
	uint32_t fields;

	start = 0;
	stop = chunk_addr_to_index(chunk, chunk->bc_last) + 1;

	for(i = start; i < stop; i++){
		fields = chunk->bc_bytes[i];
		if(!is_class_tail(fields) && !is_class_unknown(fields)){
			if(first_out){
				*first_out = chunk->bc_first + i;
			}
			return 0;
		}
	}
	dis_errno = DER_NOTFOUND;
	return -1;
}

int chunk_last_item(struct bytechunk *chunk, uint64_t *last_out){
	uint32_t fields;
	int start;
	int stop;
	int i;

	start = chunk_addr_to_index(chunk, chunk->bc_last);
	stop = -1;

	for(i = start; i > stop; i--){
		fields = chunk->bc_bytes[i];
		if(!is_class_tail(fields) && !is_class_unknown(fields)){
			if(last_out){
				*last_out = chunk->bc_first + i;
			}
			return 0;
		}
	}
	dis_errno = DER_NOTFOUND;
	return -1;
}

int chunk_next_item(struct bytechunk *chunk, uint64_t addr, uint64_t *next_out){
	uint32_t fields;
	int start;
	int stop;
	int i;

	if(chunk_contains_addr(chunk, addr)){
		start = chunk_addr_to_index(chunk, addr) + 1;
		stop = chunk_addr_to_index(chunk, chunk->bc_last) + 1;

		for(i = start; i < stop; i++){
			fields = chunk->bc_bytes[i];
			if(!is_class_tail(fields) && !is_class_unknown(fields)){
				if(next_out){
					*next_out = chunk->bc_first + i;
				}
				return 0;
			}
		}
		dis_errno = DER_NOTFOUND;
		return -1;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}


int chunk_prev_item(struct bytechunk *chunk, uint64_t addr, uint64_t *prev_out){
	uint32_t fields;
	int start;
	int stop;
	int i;

	if(chunk_contains_addr(chunk, addr)){
		start = chunk_addr_to_index(chunk, addr) - 1;
		stop = -1;

		for(i = start; i > stop; i--){
			fields = chunk->bc_bytes[i];
			if(!is_class_tail(fields) && !is_class_unknown(fields)){
				if(prev_out){
					*prev_out = chunk->bc_first + i;
				}
				return 0;
			}
		}
		dis_errno = DER_NOTFOUND;
		return -1;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}


int
chunk_item_head(struct bytechunk *chunk, uint64_t addr, uint64_t *head_out){
	uint32_t fields;
	int start;
	int i;


	if(chunk_contains_addr(chunk, addr)){
		start = chunk_addr_to_index(chunk, addr);

		fields = chunk->bc_bytes[start];

		if(is_class_unknown(fields)){
			return -1;
		}

		for(i = start; i >= 0; i--){
			fields = chunk->bc_bytes[i];
			if(!is_class_tail(fields)){
				if(head_out){
					*head_out = chunk->bc_first + i;
					return 0;
				}
			}
		}
		dis_errno = DER_NOTFOUND;
		return -1;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}


int
chunk_item_end(struct bytechunk *chunk, uint64_t addr, uint64_t *end_out){
	uint32_t flags;
	int start;
	int stop;
	int i;

	if(chunk_contains_addr(chunk, addr)){
		start = chunk_addr_to_index(chunk, addr) + 1;
		stop = chunk_addr_to_index(chunk, chunk->bc_last) + 1;

		for(i = start; i < stop; i++){
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
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int chunk_first_not_tail(struct bytechunk *chunk, uint64_t *first_out){
	uint32_t fields;
	int start;
	int stop;
	int i;

	start = 0;
	stop = chunk_addr_to_index(chunk, chunk->bc_last) + 1;

	for(i = start; i < stop; i++){
		fields = chunk->bc_bytes[i];
		if(!is_class_tail(fields)){
			if(first_out){
				*first_out = chunk->bc_first + i;
			}
			return 0;
		}
	}
	dis_errno = DER_NOTFOUND;
	return -1;
}

int chunk_last_not_tail(struct bytechunk *chunk, uint64_t *last_out){
	uint32_t fields;
	int start;
	int stop;
	int i;

	start = chunk_addr_to_index(chunk, chunk->bc_last);
	stop = -1;

	for(i = start; i > stop; i--){
		fields = chunk->bc_bytes[i];
		if(!is_class_tail(fields)){
			if(last_out){
				*last_out = chunk->bc_first + i;
			}
			return 0;
		}
	}
	dis_errno = DER_NOTFOUND;
	return -1;
}

int chunk_next_not_tail(struct bytechunk *chunk, uint64_t addr, uint64_t *next_out){
	int i;
	int start;
	int stop;
	uint32_t fields;

	if(chunk_contains_addr(chunk, addr)){
		start = chunk_addr_to_index(chunk, addr) + 1;
		stop = chunk_addr_to_index(chunk, chunk->bc_last) + 1;

		for(i = start; i < stop; i++){
			fields = chunk->bc_bytes[i];
			if(!is_class_tail(fields)){
				if(next_out){
					*next_out = chunk->bc_first + i;
				}
				return 0;
			}
		}
		dis_errno = DER_NOTFOUND;
		return -1;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}


int chunk_prev_not_tail(struct bytechunk *chunk, uint64_t addr, uint64_t *prev_out){
	int i;
	int start;
	int stop;
	uint32_t fields;

	if(chunk_contains_addr(chunk, addr)){
		start = chunk_addr_to_index(chunk, addr) - 1;
		stop = -1;

		for(i = start; i > stop; i--){
			fields = chunk->bc_bytes[i];
			if(!is_class_tail(fields)){
				if(prev_out){
					*prev_out = chunk->bc_first + i;
				}
				return 0;
			}
		}
		dis_errno = DER_NOTFOUND;
		return -1;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int chunk_create_code_item(struct bytechunk *chunk, uint64_t first, uint64_t last){
	register int r;

	if(first > last){
		dis_errno = DER_INVPARAM;
		return -1;
	}
	
	if(chunk_contains_range(chunk, first, last)){
		if(!is_chunk_range_class_unknown(chunk, first, last)){
			dis_errno = DER_NOTUNKNOWN;
			return -1;
		}

		r = chunk_set_byte_class(chunk, first, CLASS_CODE);
		if(r){
			return r;
		}

		if(first + 1 <= last){
			r = chunk_set_range_class_tail(chunk, first+1, last);
			if(r){
				return r;
			}
		}
		return 0;
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int chunk_create_data_item_byte(struct bytechunk *chunk, uint64_t addr){
	uint32_t fields;
	int i;

	if(chunk_contains_addr(chunk, addr)){
		i = chunk_addr_to_index(chunk, addr);
		fields = chunk->bc_bytes[i];

		if(is_class_unknown(fields)){
			fields = set_class_field(fields, CLASS_DATA);
			fields = set_datatype_field(fields, DATATYPE_BYTE);
			chunk->bc_bytes[i] = fields;
			return 0;
		} else {
			dis_errno = DER_NOTUNKNOWN;
			return -1;
		}
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int chunk_create_data_item_word(struct bytechunk *chunk, uint64_t addr){
	uint32_t fields;
	int i;

	if(chunk_contains_range(chunk, addr, addr+1)){
		if(is_chunk_range_class_unknown(chunk, addr, addr+1)){
			i = chunk_addr_to_index(chunk, addr);
			fields = chunk->bc_bytes[i];
			fields = set_class_field(fields, CLASS_DATA);
			fields = set_datatype_field(fields, DATATYPE_WORD);
			chunk->bc_bytes[i] = fields;
			return chunk_set_byte_class(chunk, addr+1, CLASS_TAIL);
		} else {
			dis_errno = DER_NOTUNKNOWN;
			return -1;
		}
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int chunk_create_data_item_dword(struct bytechunk *chunk, uint64_t addr){
	uint32_t fields;
	int i;

	if(chunk_contains_range(chunk, addr, addr+3)){
		if(is_chunk_range_class_unknown(chunk, addr, addr+3)){
			i = chunk_addr_to_index(chunk, addr);
			fields = chunk->bc_bytes[i];
			fields = set_class_field(fields, CLASS_DATA);
			fields = set_datatype_field(fields, DATATYPE_DWORD);
			chunk->bc_bytes[i] = fields;
			return chunk_set_range_class_tail(chunk, addr+1, addr+3);
		} else {
			dis_errno = DER_NOTUNKNOWN;
			return -1;
		}
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}

int chunk_create_data_item_qword(struct bytechunk *chunk, uint64_t addr){
	uint32_t fields;
	int i;

	if(chunk_contains_range(chunk, addr, addr+7)){
		if(is_chunk_range_class_unknown(chunk, addr, addr+7)){
			i = chunk_addr_to_index(chunk, addr);
			fields = chunk->bc_bytes[i];
			fields = set_class_field(fields, CLASS_DATA);
			fields = set_datatype_field(fields, DATATYPE_QWORD);
			chunk->bc_bytes[i] = fields;
			return chunk_set_range_class_tail(chunk, addr+1, addr+7);
		} else {
			dis_errno = DER_NOTUNKNOWN;
			return -1;
		}
	} else {
		dis_errno = DER_BOUNDS;
		return -1;
	}
}
