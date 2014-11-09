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

void dump_chunk(struct bytechunk *chunk);

int chunk_contains_addr(struct bytechunk *chunk, uint64_t addr);
int chunk_contains_range(struct bytechunk *chunk, uint64_t first, uint64_t last);

int chunk_get_byte_fields(struct bytechunk *chunk, uint64_t addr, uint32_t *flags);
int chunk_put_byte_fields(struct bytechunk *chunk, uint64_t addr, uint32_t fields);

int merge_chunks(struct bytechunk *before, struct bytechunk *after);
int expand_chunk_up(struct bytechunk *chunk, uint64_t last); 
int expand_chunk_down(struct bytechunk *chunk, uint64_t first);


/* VALUE FIELD */
int chunk_get_byte(struct bytechunk *chunk, uint64_t addr, uint8_t *byte_out);
int chunk_put_byte(struct bytechunk *chunk, uint64_t addr, uint8_t byte);
int copy_from_chunk(struct bytechunk *chunk, uint64_t addr, uint8_t *buf, size_t size);
int copy_to_chunk(struct bytechunk *chunk, uint64_t addr, uint8_t *buf, size_t size);
int chunk_set_range_values(struct bytechunk *chunk, uint8_t c, uint64_t first, uint64_t last);

int chunk_get_word(struct bytechunk *chunk, uint64_t addr, uint16_t *word_out);
int chunk_get_dword(struct bytechunk *chunk, uint64_t addr, uint32_t *dword_out);
int chunk_get_qword(struct bytechunk *chunk, uint64_t addr, uint64_t *qword_out);
int chunk_put_word(struct bytechunk *chunk, uint64_t addr, uint16_t word);
int chunk_put_dword(struct bytechunk *chunk, uint64_t addr, uint32_t dword);
int chunk_put_qword(struct bytechunk *chunk, uint64_t addr, uint64_t qword);




/* CLASS FIELD */
int chunk_get_byte_class(struct bytechunk *chunk, uint64_t addr, uint32_t *class_out);
int chunk_set_byte_class(struct bytechunk *chunk, uint64_t addr, uint32_t class_);

int is_chunk_range_class(struct bytechunk *chunk, uint64_t first, uint64_t last, uint32_t class_);
int chunk_set_range_class(struct bytechunk *chunk, uint64_t first, uint64_t last, uint32_t class_);

int is_chunk_range_class_unknown(struct bytechunk *chunk, uint64_t first, uint64_t last);
int is_chunk_range_class_code(struct bytechunk *chunk, uint64_t first, uint64_t last);
int is_chunk_range_class_data(struct bytechunk *chunk, uint64_t first, uint64_t last);
int is_chunk_range_class_tail(struct bytechunk *chunk, uint64_t first, uint64_t last);
int chunk_set_range_class_unknown(struct bytechunk *chunk, uint64_t first, uint64_t last);
int chunk_set_range_class_code(struct bytechunk *chunk, uint64_t first, uint64_t last);
int chunk_set_range_class_data(struct bytechunk *chunk, uint64_t first, uint64_t last);
int chunk_set_range_class_tail(struct bytechunk *chunk, uint64_t first, uint64_t last);




/* DATATYPE FIELD */
int chunk_get_byte_datatype(struct bytechunk *chunk, uint64_t addr, uint32_t *datatype_out);
int chunk_put_byte_datatype(struct bytechunk *chunk, uint64_t addr, uint32_t datatype);




/*  ITEMS  */
int chunk_first_item(struct bytechunk *chunk, uint64_t *head_out);
int chunk_last_item(struct bytechunk *chunk, uint64_t *head_out);
int chunk_next_item(struct bytechunk *chunk, uint64_t addr, uint64_t *next_out);
int chunk_prev_item(struct bytechunk *chunk, uint64_t addr, uint64_t *prev_out);
int chunk_item_head(struct bytechunk *chunk, uint64_t addr, uint64_t *head_out);
int chunk_item_end(struct bytechunk *chunk, uint64_t addr, uint64_t *end_out);
int chunk_first_not_tail(struct bytechunk *chunk, uint64_t *first_out);
int chunk_last_not_tail(struct bytechunk *chunk, uint64_t *last_out);
int chunk_next_not_tail(struct bytechunk *chunk, uint64_t addr, uint64_t *next_out);
int chunk_prev_not_tail(struct bytechunk *chunk, uint64_t addr, uint64_t *prev_out);

int chunk_create_code_item(struct bytechunk *chunk, uint64_t first, uint64_t last);
int chunk_create_data_item_byte(struct bytechunk *chunk, uint64_t addr);
int chunk_create_data_item_word(struct bytechunk *chunk, uint64_t addr);
int chunk_create_data_item_dword(struct bytechunk *chunk, uint64_t addr);
int chunk_create_data_item_qword(struct bytechunk *chunk, uint64_t addr);
		

#endif
