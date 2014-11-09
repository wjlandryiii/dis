/*
 * Copyright 2014 Joseph Landry
 */

#ifndef BYTES_H
#define BYTES_H

#include <stdint.h>

struct bytes {
	uint32_t addr;
	uint32_t count;
	uint32_t *byte_flags;
	struct bytechunk *b_chunks;
};

struct bytes *new_bytes(void);
void free_bytes(struct bytes *bytes);

struct bytechunk *bytes_first_chunk(struct bytes *bytes);
struct bytechunk *bytes_last_chunk(struct bytes *bytes);
struct bytechunk *bytes_next_chunk(struct bytechunk *chunk);
struct bytechunk *bytes_prev_chunk(struct bytechunk *chunk);

int enable_bytes(struct bytes *bytes, uint64_t first, uint64_t last);
/* TODO int disable_bytes(struct bytes *bytes, uint64_t first, uint64_t last); */

int get_byte_fields(struct bytes *bytes, uint64_t addr, uint32_t *fields_out);
int set_byte_fields(struct bytes *bytes, uint64_t addr, uint32_t fields);



/* VALUE */
int copy_from_bytes(struct bytes *bytes, uint64_t addr, uint8_t *buf, size_t size);
int bytes_get_byte(struct bytes *bytes, uint64_t addr, uint8_t *byte_out);
int bytes_get_word(struct bytes *bytes, uint64_t addr, uint16_t *word_out);
int bytes_get_dword(struct bytes *bytes, uint64_t addr, uint32_t *dword_out);
int bytes_get_qword(struct bytes *bytes, uint64_t addr, uint64_t *qword_out);

int copy_to_bytes(struct bytes *bytes, uint64_t addr, uint8_t *buf, size_t size);
int set_bytes(struct bytes *bytes, uint8_t c, uint64_t first, uint64_t last);
int bytes_put_byte(struct bytes *bytes, uint64_t addr, uint8_t value);
int bytes_put_word(struct bytes *bytes, uint64_t addr, uint16_t value);
int bytes_put_dword(struct bytes *bytes, uint64_t addr, uint32_t value);
int bytes_put_qword(struct bytes *bytes, uint64_t addr, uint64_t value); 



/* CLASS */
int bytes_get_byte_class(struct bytes *bytes, uint64_t addr, uint32_t *class_out);
int bytes_set_byte_class(struct bytes *bytes, uint64_t addr, uint32_t class_);



/* DATATYPE */
int get_bytes_datatype(struct bytes *bytes, uint64_t addr, uint32_t *datatype_out);
int set_bytes_datatype(struct bytes *bytes, uint64_t addr, uint32_t datatype);

/* ITEMS */

int bytes_first_address(struct bytes *bytes, uint64_t *addr_out);
int bytes_last_address(struct bytes *bytes, uint64_t *addr_out);
int bytes_next_address(struct bytes *bytes, uint64_t *addr_out);
int bytes_prev_address(struct bytes *bytes, uint64_t *addr_out);

int bytes_first_item(struct bytes *bytes, uint64_t *first_out);
int bytes_last_item(struct bytes *bytes, uint64_t *last_out);
int bytes_next_item(struct bytes *bytes, uint64_t addr, uint64_t *next_out);
int bytes_prev_item(struct bytes *bytes, uint64_t addr, uint64_t *prev_out);
int bytes_item_head(struct bytes *bytes, uint64_t addr, uint64_t *head_out);
int bytes_item_end(struct bytes *bytes, uint64_t addr, uint64_t *end_out);
int bytes_first_not_tail(struct bytes *bytes, uint64_t *first_out);
int bytes_last_not_tail(struct bytes *bytes, uint64_t *last_out);
int bytes_next_not_tail(struct bytes *bytes, uint64_t *next_out);
int bytes_prev_not_tail(struct bytes *bytes, uint64_t *prev_out);

int bytes_create_code_item(struct bytes *bytes, uint64_t first, uint64_t last);
int bytes_create_data_item_byte(struct bytes *bytes, uint64_t addr);
int bytes_create_data_item_word(struct bytes *bytes, uint64_t addr);
int bytes_create_data_item_dword(struct bytes *bytes, uint64_t addr);
int bytes_create_data_item_qword(struct bytes *bytes, uint64_t addr);


#endif
