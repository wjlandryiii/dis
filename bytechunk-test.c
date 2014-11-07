/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "bytefields.h"
#include "bytechunk.h"
#include "testrunner.h"

static
int test_newfree(void){
	struct bytechunk *chunk;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	FAIL_IF(chunk->bc_first != 10);
	FAIL_IF(chunk->bc_last != 19);
	FAIL_IF(chunk->bc_bytes == NULL);

	free_bytechunk(chunk);
	return 0;
}

static int
test_chunk_get_byte_fields(void){
	struct bytechunk *chunk;
	uint32_t fields;
	register int r;

	chunk = new_bytechunk(10, 19);

	chunk->bc_bytes[0] = 0x12345678;
	chunk->bc_bytes[9] = 0x87654321;

	r = chunk_get_byte_fields(chunk, 10, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x12345678);

	r = chunk_get_byte_fields(chunk, 19, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x87654321);
	free_bytechunk(chunk);
	return 0;
}

static int
test_chunk_get_byte_fields_bounds(void){
	struct bytechunk *chunk;
	uint32_t fields;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	dis_errno = DER_OK;
	r = chunk_get_byte_fields(chunk, 0, &fields);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);
	
	dis_errno = DER_OK;
	r = chunk_get_byte_fields(chunk, 9, &fields);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);
	
	dis_errno = DER_OK;
	r = chunk_get_byte_fields(chunk, 20, &fields);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);

	dis_errno = DER_OK;
	r = chunk_get_byte_fields(chunk, 25, &fields);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);

	free_bytechunk(chunk);
	return 0;
}

static int
test_chunk_set_byte_fields(void){
	struct bytechunk *chunk;
	uint32_t fields;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_byte_fields(chunk, 10, 0x12345678);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_fields(chunk, 10, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x12345678);

	r = chunk_set_byte_fields(chunk, 14, 0x44444444);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_fields(chunk, 14, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x44444444);

	r = chunk_set_byte_fields(chunk, 19, 0x87654321);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_fields(chunk, 19, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x87654321);

	free_bytechunk(chunk);
	return 0;
}


static int
test_chunk_set_byte_fields_bounds(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	dis_errno = DER_OK;
	r = chunk_set_byte_fields(chunk, 0, 0x12345678);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);

	dis_errno = DER_OK;
	r = chunk_set_byte_fields(chunk, 9, 0x12345678);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);

	dis_errno = DER_OK;
	r = chunk_set_byte_fields(chunk, 20, 0x12345678);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);
	
	dis_errno = DER_OK;
	r = chunk_set_byte_fields(chunk, 25, 0x12345678);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);

	free_bytechunk(chunk);
	return 0;
}


static
int test_merge_chunks(void){
	struct bytechunk *chunk_before;
	struct bytechunk *chunk_after;
	register int r;

	chunk_before = new_bytechunk(10, 19);
	FAIL_IF(chunk_before == NULL);

	chunk_after = new_bytechunk(30, 39);
	FAIL_IF(chunk_after == NULL);


	chunk_before->bc_next = chunk_after;

	r = merge_chunks(chunk_before, chunk_after);
	FAIL_IF_ERR(r);

	FAIL_IF(chunk_before->bc_first != 10);
	FAIL_IF(chunk_before->bc_last != 39);
	FAIL_IF(chunk_before->bc_next != NULL);

	free_bytechunk(chunk_before);
	return 0;
}

static
int test_expand_chunk_up(void){
	struct bytechunk *chunk;
	uint32_t fields;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_byte_fields(chunk, 10, 0x10101010);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_fields(chunk, 19, 0x19191919);
	FAIL_IF_ERR(r);

	r = expand_chunk_up(chunk, 49);
	FAIL_IF_ERR(r);
	FAIL_IF(chunk->bc_first != 10);
	FAIL_IF(chunk->bc_last != 49);
	FAIL_IF(chunk->bc_bytes == NULL);
	
	r = chunk_get_byte_fields(chunk, 10, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x10101010);
	r = chunk_get_byte_fields(chunk, 19, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x19191919);
	r = chunk_get_byte_fields(chunk, 49, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x00000000);

	free_bytechunk(chunk);
	return 0;
}

static
int test_expand_chunk_down(void){
	struct bytechunk *chunk;
	uint32_t fields;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_byte_fields(chunk, 10, 0x10101010);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_fields(chunk, 19, 0x19191919);
	FAIL_IF_ERR(r);


	r = expand_chunk_down(chunk, 5);
	FAIL_IF_ERR(r);
	FAIL_IF(chunk->bc_first != 5);
	FAIL_IF(chunk->bc_last != 19);
	FAIL_IF(chunk->bc_bytes == NULL);


	r = chunk_get_byte_fields(chunk, 10, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x10101010);
	r = chunk_get_byte_fields(chunk, 19, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x19191919);
	r = chunk_get_byte_fields(chunk, 5, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x00000000);

	free_bytechunk(chunk);
	return 0;
}


static int
test_copy_bytes_from_chunk(void){
	struct bytechunk *chunk;
	uint8_t buf[10];
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = 0;
	r |= chunk_set_byte_fields(chunk, 10, set_value_field(0x10, VALUE_VALID));
	r |= chunk_set_byte_fields(chunk, 11, set_value_field(0x11, VALUE_VALID));
	r |= chunk_set_byte_fields(chunk, 12, set_value_field(0x12, VALUE_VALID));
	r |= chunk_set_byte_fields(chunk, 13, set_value_field(0x13, VALUE_VALID));
	r |= chunk_set_byte_fields(chunk, 14, set_value_field(0x14, VALUE_VALID));
	r |= chunk_set_byte_fields(chunk, 15, set_value_field(0x15, VALUE_VALID));
	r |= chunk_set_byte_fields(chunk, 16, set_value_field(0x16, VALUE_VALID));
	r |= chunk_set_byte_fields(chunk, 17, set_value_field(0x17, VALUE_VALID));
	r |= chunk_set_byte_fields(chunk, 18, set_value_field(0x18, VALUE_VALID));
	r |= chunk_set_byte_fields(chunk, 19, set_value_field(0x19, VALUE_VALID));
	FAIL_IF(r != 0);

	r = copy_bytes_from_chunk(chunk, 10, buf, sizeof(buf));
	FAIL_IF_ERR(r);

	r = 0;
	r |= buf[0] ^ 0x10;
	r |= buf[1] ^ 0x11;
	r |= buf[2] ^ 0x12;
	r |= buf[3] ^ 0x13;
	r |= buf[4] ^ 0x14;
	r |= buf[5] ^ 0x15;
	r |= buf[6] ^ 0x16;
	r |= buf[7] ^ 0x17;
	r |= buf[8] ^ 0x18;
	r |= buf[9] ^ 0x19;
	FAIL_IF(r != 0);

	free_bytechunk(chunk);
	return 0;
}

static int
test_copy_bytes_from_chunk_invalid_value(void){
	struct bytechunk *chunk;
	uint8_t buf[10];
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = 0;
	r |= chunk_set_byte_fields(chunk, 10, set_value_field(0x10, VALUE_VALID));
	r |= chunk_set_byte_fields(chunk, 12, set_value_field(0x12, VALUE_VALID));
	
	r = copy_bytes_from_chunk(chunk, 10, buf, 3);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_INVVALUE);

	free_bytechunk(chunk);
	return 0;
}

static int
test_copy_bytes_to_chunk(void){
	struct bytechunk *chunk;
	uint8_t src[10];
	uint8_t dst[10];
	register int r;

	src[0] = 0x10;
	src[1] = 0x11;
	src[2] = 0x12;
	src[3] = 0x13;
	src[4] = 0x14;
	src[5] = 0x15;
	src[6] = 0x16;
	src[7] = 0x17;
	src[8] = 0x18;
	src[9] = 0x19;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = copy_bytes_to_chunk(chunk, 10, src, 10);
	FAIL_IF_ERR(r);

	r = copy_bytes_from_chunk(chunk, 10, dst, 10);
	FAIL_IF_ERR(r);

	r = memcmp(src, dst, 10);
	FAIL_IF(r != 0);

	free_bytechunk(chunk);
	return 0;	
}

static int
test_chunk_set_bytes(void){
	struct bytechunk *chunk;
	uint8_t dst[10];
	int i;
	register int r;


	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_bytes(chunk, 0x90, 10, 19);
	FAIL_IF_ERR(r);

	r = copy_bytes_from_chunk(chunk, 10, dst, 10);
	FAIL_IF_ERR(r);

	r = 0;
	for(i = 0; i < 10; i++){
		r |= dst[i] ^ 0x90;
	}
	FAIL_IF(r != 0);

	free_bytechunk(chunk);
	return 0;
}


static struct test tests[] = {
	{"newfree", test_newfree},
	{"chunk_get_byte_fields", test_chunk_get_byte_fields},
	{"chunk_get_byte_fields-bounds", test_chunk_get_byte_fields_bounds},
	{"chunk_set_byte_fields", test_chunk_set_byte_fields},
	{"chunk_set_byte_fields-bounds", test_chunk_set_byte_fields_bounds},
	{"merge_chunks", test_merge_chunks},
	{"expand_chunk_up", test_expand_chunk_up},
	{"expand_chunk_down", test_expand_chunk_down},
	{"copy_bytes_from_chunk", test_copy_bytes_from_chunk},
	{"copy_bytes_from_chunk-invalid_value", test_copy_bytes_from_chunk_invalid_value},
	{"copy_bytes_to_chunk", test_copy_bytes_to_chunk},
	{"chunk_set_bytes", test_chunk_set_bytes},
	{NULL, NULL},
};

void test_bytechunk_init(void) __attribute__ ((constructor));
void test_bytechunk_init(void){
	add_module_tests("byteschunk", tests);
}
