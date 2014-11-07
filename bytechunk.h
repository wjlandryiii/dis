/*
 * Copyright 2014 Joseph Landry
 */

#ifndef BYTECHUNK_H
#define BYTECHUNK_H

#include <stdint.h>

uint64_t
range_size(uint64_t A, uint64_t B);

int
is_range_overlapping(uint64_t A, uint64_t B, uint64_t C, uint64_t D);

int
is_range_adjacent(uint64_t A, uint64_t B, uint64_t C, uint64_t D);



struct bytechunk {
	struct bytechunk *bc_next;
	uint64_t bc_first;
	uint64_t bc_last;
	uint32_t *bc_bytes;
};


struct bytechunk *new_bytechunk(uint64_t first, uint64_t last);
void free_bytechunk(struct bytechunk *chunk); 

int chunk_get_byte_fields(struct bytechunk *chunk, uint64_t addr, uint32_t *flags);
int chunk_set_fields(struct bytechunk *chunk, uint64_t addr, uint32_t fields);

int merge_chunks(struct bytechunk *before, struct bytechunk *after);
int expand_chunk_up(struct bytechunk *chunk, uint64_t last); 
int expand_chunk_down(struct bytechunk *chunk, uint64_t first);


int real_copy_from_bytes(struct bytechunk *chunk, uint64_t addr, uint8_t *buf, size_t size);
int real_copy_to_bytes(struct bytechunk *chunk, uint64_t addr, uint8_t *buf, size_t size);

int chunk_set_bytes(struct bytechunk *chunk, uint8_t c, uint64_t first, uint64_t last);


int chunk_item_head(struct bytechunk *chunk, uint64_t addr, uint64_t *head_out);
int chunk_item_end(struct bytechunk *chunk, uint64_t addr, uint64_t *end_out);

int is_chunk_range_class_unknown(struct bytechunk *chunk, uint64_t first, uint64_t last);
int set_chunk_range_class_unknown(struct bytechunk *chunk, uint64_t first, uint64_t last);
int set_chunk_range_class_code(struct bytechunk *chunk, uint64_t first, uint64_t last);
int set_chunk_range_class_data(struct bytechunk *chunk, uint64_t first, uint64_t last);


int set_chunk_datatype_field(struct bytechunk *chunk, uint64_t addr, uint32_t datatype);

#endif
