/*
 * Copyright 2014 Joseph Landry
 */

#ifndef BYTES_H
#define BYTES_H

#include <stdint.h>

struct bytechunk {
	struct bytechunk *bc_next;
	uint64_t bc_first;
	uint64_t bc_last;
	uint32_t *bc_bytes;
};


struct bytes {
	uint32_t addr;
	uint32_t count;
	uint32_t *byte_flags;
	struct bytechunk *b_chunks;
};

struct bytes *
new_bytes(void);

void
free_bytes(struct bytes *bytes);

struct bytechunk *
first_chunk(struct bytes *bytes);

struct bytechunk *
next_chunk(struct bytechunk *chunk);

int
enable_bytes(struct bytes *bytes, uint64_t first, uint64_t last);


/* TODO
int
disable_bytes(struct bytes *bytes, uint64_t first, uint64_t last);
*/

int
get_byte_fields(struct bytes *bytes, uint64_t addr, uint32_t *fields_out);

/* set_byte_fields() should only be used for testing! */
int
set_byte_fields(struct bytes *bytes, uint64_t addr, uint32_t fields);



int
copy_from_bytes(struct bytes *bytes, uint64_t addr, uint8_t *buf, size_t size);

int
bytes_get_byte(struct bytes *t, uint64_t addr, uint8_t *byte_out);

int
bytes_get_word(struct bytes *bytes, uint64_t addr, uint16_t *word_out);

int
bytes_get_dword(struct bytes *bytes, uint64_t addr, uint32_t *dword_out);

int
bytes_get_qword(struct bytes *bytes, uint64_t addr, uint64_t *qword_out);


int
copy_to_bytes(struct bytes *bytes, uint64_t addr, uint8_t *buf, size_t size);

int
set_bytes(struct bytes *bytes, uint8_t c, uint64_t first, uint64_t last);

int
bytes_put_byte(struct bytes *t, uint64_t addr, uint8_t value);

int
bytes_put_word(struct bytes *bytes, uint64_t addr, uint16_t value);

int
bytes_put_dword(struct bytes *bytes, uint64_t addr, uint32_t value);

int
bytes_put_qword(struct bytes *bytes, uint64_t addr, uint64_t value);



int bytes_get_byte_class(struct bytes *bytes, uint64_t addr, uint32_t *class_out);

int item_head(struct bytes *bytes, uint64_t addr, uint64_t *head_out);
int item_end(struct bytes *bytes, uint64_t addr, uint64_t *end_out);


/* TODO
int first_item(struct bytes *bytes, uint64_t *addr);
int next_item(struct bytes *bytes, uint64_t addr, uint64_t *next_out);
int prev_item(struct bytes *bytes, uint64_t addr, uint64_t *prev_out);
int next_not_tail(struct bytes *bytes, uint64_t addr, uint64_t *next_out);
*/

int
set_class_code(struct bytes *bytes, uint64_t first, uint64_t last);

int
set_class_data(struct bytes *bytes, uint64_t first, uint64_t last);

int
set_class_unknown(struct bytes *bytes, uint64_t first, uint64_t last);


int
get_bytes_datatype(struct bytes *bytes, uint64_t addr, uint32_t *datatype_out);

int
set_bytes_datatype_byte(struct bytes *bytes, uint64_t addr);

int
set_bytes_datatype_word(struct bytes *bytes, uint64_t addr);

int
set_bytes_datatype_dword(struct bytes *bytes, uint64_t addr);

int
set_bytes_datatype_qword(struct bytes *bytes, uint64_t addr);

/*****************************************************************************/


extern int 
create_bytes(char *filename, uint32_t base_addr, struct bytes *bytes);

extern int
load_bytes(char *filename, struct bytes *bytes);

extern int
save_bytes(char *filename, struct bytes *bytes);

#endif
