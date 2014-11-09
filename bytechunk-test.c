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
test_chunk_put_byte_fields(void){
	struct bytechunk *chunk;
	uint32_t fields;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_put_byte_fields(chunk, 10, 0x12345678);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_fields(chunk, 10, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x12345678);

	r = chunk_put_byte_fields(chunk, 14, 0x44444444);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_fields(chunk, 14, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x44444444);

	r = chunk_put_byte_fields(chunk, 19, 0x87654321);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_fields(chunk, 19, &fields);
	FAIL_IF_ERR(r);
	FAIL_IF(fields != 0x87654321);

	free_bytechunk(chunk);
	return 0;
}


static int
test_chunk_put_byte_fields_bounds(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	dis_errno = DER_OK;
	r = chunk_put_byte_fields(chunk, 0, 0x12345678);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);

	dis_errno = DER_OK;
	r = chunk_put_byte_fields(chunk, 9, 0x12345678);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);

	dis_errno = DER_OK;
	r = chunk_put_byte_fields(chunk, 20, 0x12345678);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_BOUNDS);
	
	dis_errno = DER_OK;
	r = chunk_put_byte_fields(chunk, 25, 0x12345678);
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

	r = chunk_put_byte_fields(chunk, 10, 0x10101010);
	FAIL_IF_ERR(r);
	r = chunk_put_byte_fields(chunk, 19, 0x19191919);
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

	r = chunk_put_byte_fields(chunk, 10, 0x10101010);
	FAIL_IF_ERR(r);
	r = chunk_put_byte_fields(chunk, 19, 0x19191919);
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


/*******************************    Value Field    ***************************/

static int test_chunk_get_byte(void){
	struct bytechunk *chunk;
	uint32_t fields;
	uint8_t byte;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_get_byte(chunk, 10, &byte);
	FAIL_IF(!r);
	FAIL_IF(dis_errno != DER_INVVALUE);

	fields = 0;
	fields = set_byte_value_field(fields, 0x90);
	fields = set_value_field(fields, VALUE_VALID);
	chunk->bc_bytes[0] = fields;

	byte = 0;
	r = chunk_get_byte(chunk, 10, &byte);
	FAIL_IF_ERR(r);
	FAIL_IF(byte != 0x90);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_put_byte(void){
	struct bytechunk *chunk;
	uint8_t byte;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_put_byte(chunk, 10, 0x90);
	FAIL_IF_ERR(r);

	byte = 0;
	r = chunk_get_byte(chunk, 10, &byte);
	FAIL_IF_ERR(r);
	FAIL_IF(byte != 0x90);

	free_bytechunk(chunk);
	return 0;
}


static int
test_copy_from_chunk(void){
	struct bytechunk *chunk;
	uint8_t buf[10];
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = 0;
	r |= chunk_put_byte_fields(chunk, 10, set_value_field(0x10, VALUE_VALID));
	r |= chunk_put_byte_fields(chunk, 11, set_value_field(0x11, VALUE_VALID));
	r |= chunk_put_byte_fields(chunk, 12, set_value_field(0x12, VALUE_VALID));
	r |= chunk_put_byte_fields(chunk, 13, set_value_field(0x13, VALUE_VALID));
	r |= chunk_put_byte_fields(chunk, 14, set_value_field(0x14, VALUE_VALID));
	r |= chunk_put_byte_fields(chunk, 15, set_value_field(0x15, VALUE_VALID));
	r |= chunk_put_byte_fields(chunk, 16, set_value_field(0x16, VALUE_VALID));
	r |= chunk_put_byte_fields(chunk, 17, set_value_field(0x17, VALUE_VALID));
	r |= chunk_put_byte_fields(chunk, 18, set_value_field(0x18, VALUE_VALID));
	r |= chunk_put_byte_fields(chunk, 19, set_value_field(0x19, VALUE_VALID));
	FAIL_IF(r != 0);

	r = copy_from_chunk(chunk, 10, buf, sizeof(buf));
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
test_copy_from_chunk_invalid_value(void){
	struct bytechunk *chunk;
	uint8_t buf[10];
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = 0;
	r |= chunk_put_byte_fields(chunk, 10, set_value_field(0x10, VALUE_VALID));
	r |= chunk_put_byte_fields(chunk, 12, set_value_field(0x12, VALUE_VALID));
	
	r = copy_from_chunk(chunk, 10, buf, 3);
	FAIL_IF(r == 0);
	FAIL_IF(dis_errno != DER_INVVALUE);

	free_bytechunk(chunk);
	return 0;
}

static int
test_copy_to_chunk(void){
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

	r = copy_to_chunk(chunk, 10, src, 10);
	FAIL_IF_ERR(r);

	r = copy_from_chunk(chunk, 10, dst, 10);
	FAIL_IF_ERR(r);

	r = memcmp(src, dst, 10);
	FAIL_IF(r != 0);

	free_bytechunk(chunk);
	return 0;	
}

static int
test_chunk_set_range_values(void){
	struct bytechunk *chunk;
	uint8_t dst[10];
	int i;
	register int r;


	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_values(chunk, 0x90, 10, 19);
	FAIL_IF_ERR(r);

	r = copy_from_chunk(chunk, 10, dst, 10);
	FAIL_IF_ERR(r);

	r = 0;
	for(i = 0; i < 10; i++){
		r |= dst[i] ^ 0x90;
	}
	FAIL_IF(r != 0);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_get_word(void){
	struct bytechunk *chunk;
	uint16_t word;
	register int r;


	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = 0;
	r |= chunk_put_byte(chunk, 10, 0x23);
	r |= chunk_put_byte(chunk, 11, 0x01);
	FAIL_IF_ERR(r);

	word = 0;
	r = chunk_get_word(chunk, 10, &word);
	FAIL_IF_ERR(r);
	FAIL_IF(word != 0x0123);
	
	free_bytechunk(chunk);
	return 0;
}


static int test_chunk_get_dword(void){
	struct bytechunk *chunk;
	uint32_t dword;
	register int r;


	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = 0;
	r |= chunk_put_byte(chunk, 10, 0x67);
	r |= chunk_put_byte(chunk, 11, 0x45);
	r |= chunk_put_byte(chunk, 12, 0x23);
	r |= chunk_put_byte(chunk, 13, 0x01);
	FAIL_IF_ERR(r);

	dword = 0;
	r = chunk_get_dword(chunk, 10, &dword);
	FAIL_IF_ERR(r);
	FAIL_IF(dword != 0x01234567);
	
	free_bytechunk(chunk);
	return 0;
}


static int test_chunk_get_qword(void){
	struct bytechunk *chunk;
	uint64_t qword;
	register int r;


	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = 0;
	r |= chunk_put_byte(chunk, 10, 0xEF);
	r |= chunk_put_byte(chunk, 11, 0xCD);
	r |= chunk_put_byte(chunk, 12, 0xAB);
	r |= chunk_put_byte(chunk, 13, 0x89);
	r |= chunk_put_byte(chunk, 14, 0x67);
	r |= chunk_put_byte(chunk, 15, 0x45);
	r |= chunk_put_byte(chunk, 16, 0x23);
	r |= chunk_put_byte(chunk, 17, 0x01);
	FAIL_IF_ERR(r);

	qword = 0;
	r = chunk_get_qword(chunk, 10, &qword);
	FAIL_IF_ERR(r);
	FAIL_IF(qword != 0x0123456789ABCDEF);
	
	free_bytechunk(chunk);
	return 0;
}


static int test_chunk_put_word(void){
	struct bytechunk *chunk;
	uint16_t word;
	register int r;


	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_put_word(chunk, 10, 0x0123);
	FAIL_IF_ERR(r);

	word = 0;
	r = chunk_get_word(chunk, 10, &word);
	FAIL_IF_ERR(r);
	FAIL_IF(word != 0x0123);

	free_bytechunk(chunk);
	return 0;
}


static int test_chunk_put_dword(void){
	struct bytechunk *chunk;
	uint32_t dword;
	register int r;


	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_put_dword(chunk, 10, 0x01234567);
	FAIL_IF_ERR(r);

	dword = 0;
	r = chunk_get_dword(chunk, 10, &dword);
	FAIL_IF_ERR(r);
	FAIL_IF(dword != 0x01234567);

	free_bytechunk(chunk);
	return 0;
}


static int test_chunk_put_qword(void){
	struct bytechunk *chunk;
	uint64_t qword;
	register int r;


	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_put_qword(chunk, 10, 0x0123456789ABCDEF);
	FAIL_IF_ERR(r);

	qword = 0;
	r = chunk_get_qword(chunk, 10, &qword);
	FAIL_IF_ERR(r);
	FAIL_IF(qword != 0x0123456789ABCDEF);

	free_bytechunk(chunk);
	return 0;
}



/****************************   Class Field   ********************************/



static int
test_chunk_get_byte_class(void){
	struct bytechunk *chunk;
	uint32_t class;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_put_byte_fields(chunk, 10, set_class_field(0, CLASS_UNKNOWN));
	FAIL_IF_ERR(r);
	r = chunk_get_byte_class(chunk, 10, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_UNKNOWN);

	chunk_put_byte_fields(chunk, 11, set_class_field(0, CLASS_CODE));
	FAIL_IF_ERR(r);
	r = chunk_get_byte_class(chunk, 11, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_CODE);

	chunk_put_byte_fields(chunk, 12, set_class_field(0, CLASS_DATA));
	FAIL_IF_ERR(r);
	r = chunk_get_byte_class(chunk, 12, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);

	chunk_put_byte_fields(chunk, 13, set_class_field(0, CLASS_TAIL));
	FAIL_IF_ERR(r);
	r = chunk_get_byte_class(chunk, 13, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_TAIL);

	free_bytechunk(chunk);
	return 0;
}

static int
test_chunk_set_byte_class(void){
	struct bytechunk *chunk;
	uint32_t class;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_byte_class(chunk, 10, CLASS_UNKNOWN);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_class(chunk, 10, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_UNKNOWN);
	
	chunk_put_byte_fields(chunk, 11, CLASS_CODE);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_class(chunk, 11, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_CODE);

	chunk_put_byte_fields(chunk, 12, CLASS_DATA);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_class(chunk, 12, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);

	chunk_put_byte_fields(chunk, 13, CLASS_TAIL);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_class(chunk, 13, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_TAIL);

	free_bytechunk(chunk);
	return 0;
}


static int test_is_chunk_range_class(void){
	struct bytechunk *chunk;
	register int r;
	int i;
	
	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	for(i = 10; i < 20; i++){
		chunk_set_byte_class(chunk, i, CLASS_UNKNOWN);
	}
	r = is_chunk_range_class(chunk, 10, 19, CLASS_UNKNOWN);
	FAIL_IF(!r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_CODE);
	FAIL_IF(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_DATA);
	FAIL_IF(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_TAIL);
	FAIL_IF(r);

	for(i = 10; i < 20; i++){
		chunk_set_byte_class(chunk, i, CLASS_CODE);
	}
	r = is_chunk_range_class(chunk, 10, 19, CLASS_UNKNOWN);
	FAIL_IF(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_CODE);
	FAIL_IF(!r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_DATA);
	FAIL_IF(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_TAIL);
	FAIL_IF(r);

	for(i = 10; i < 20; i++){
		chunk_set_byte_class(chunk, i, CLASS_DATA);
	}
	r = is_chunk_range_class(chunk, 10, 19, CLASS_UNKNOWN);
	FAIL_IF(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_CODE);
	FAIL_IF(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_DATA);
	FAIL_IF(!r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_TAIL);
	FAIL_IF(r);

	for(i = 10; i < 20; i++){
		chunk_set_byte_class(chunk, i, CLASS_TAIL);
	}
	r = is_chunk_range_class(chunk, 10, 19, CLASS_UNKNOWN);
	FAIL_IF(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_CODE);
	FAIL_IF(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_DATA);
	FAIL_IF(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_TAIL);
	FAIL_IF(!r);

	free_bytechunk(chunk);
	return 0;
}


static int test_chunk_set_range_class(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class(chunk, 10, 19, CLASS_UNKNOWN);
	FAIL_IF_ERR(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_UNKNOWN);
	FAIL_IF(!r);
	
	r = chunk_set_range_class(chunk, 10, 19, CLASS_CODE);
	FAIL_IF_ERR(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_CODE);
	FAIL_IF(!r);
	
	r = chunk_set_range_class(chunk, 10, 19, CLASS_DATA);
	FAIL_IF_ERR(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_DATA);
	FAIL_IF(!r);
	
	r = chunk_set_range_class(chunk, 10, 19, CLASS_TAIL);
	FAIL_IF_ERR(r);
	r = is_chunk_range_class(chunk, 10, 19, CLASS_TAIL);
	FAIL_IF(!r);

	free_bytechunk(chunk);
	return 0;
}


static int test_is_chunk_range_class_unknown(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = is_chunk_range_class_unknown(chunk, 10, 19);
	FAIL_IF(!r);

	r = chunk_set_byte_class(chunk, 14, CLASS_DATA);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_unknown(chunk, 10, 19);
	FAIL_IF(r);

	free_bytechunk(chunk);
	return 0;
}

static int test_is_chunk_range_class_code(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class(chunk, 10, 19, CLASS_CODE);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_code(chunk, 10, 19);
	FAIL_IF(!r);

	r = chunk_set_byte_class(chunk, 14, CLASS_UNKNOWN);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_code(chunk, 10, 19);
	FAIL_IF(r);

	free_bytechunk(chunk);
	return 0;
}

static int test_is_chunk_range_class_data(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class(chunk, 10, 19, CLASS_DATA);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_data(chunk, 10, 19);
	FAIL_IF(!r);

	r = chunk_set_byte_class(chunk, 14, CLASS_UNKNOWN);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_data(chunk, 10, 19);
	FAIL_IF(r);

	free_bytechunk(chunk);
	return 0;
}

static int test_is_chunk_range_class_tail(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class(chunk, 10, 19, CLASS_TAIL);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_tail(chunk, 10, 19);
	FAIL_IF(!r);

	r = chunk_set_byte_class(chunk, 14, CLASS_UNKNOWN);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_tail(chunk, 10, 19);
	FAIL_IF(r);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_set_range_class_unknown(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class(chunk, 10, 19, CLASS_CODE);
	FAIL_IF_ERR(r);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_unknown(chunk, 10, 19);
	FAIL_IF(r == 0);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_set_range_class_code(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);

	r = chunk_set_range_class_code(chunk, 10, 19);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_code(chunk, 10, 19);
	FAIL_IF(r == 0);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_set_range_class_data(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);

	r = chunk_set_range_class_data(chunk, 10, 19);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_data(chunk, 10, 19);
	FAIL_IF(r == 0);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_set_range_class_tail(void){
	struct bytechunk *chunk;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);

	r = chunk_set_range_class_tail(chunk, 10, 19);
	FAIL_IF_ERR(r);

	r = is_chunk_range_class_tail(chunk, 10, 19);
	FAIL_IF(r == 0);

	free_bytechunk(chunk);
	return 0;
}


/***************************   Datatype Field    *****************************/

static int test_chunk_get_byte_datatype(void){
	struct bytechunk *chunk;
	uint32_t fields;
	uint32_t datatype;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	fields = 0;
	fields = set_class_field(fields, CLASS_DATA);
	fields = set_datatype_field(fields, DATATYPE_BYTE);
	r = chunk_put_byte_fields(chunk, 10, fields);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_datatype(chunk, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_BYTE);
	
	fields = 0;
	fields = set_class_field(fields, CLASS_DATA);
	fields = set_datatype_field(fields, DATATYPE_WORD);
	r = chunk_put_byte_fields(chunk, 10, fields);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_datatype(chunk, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_WORD);

	fields = 0;
	fields = set_class_field(fields, CLASS_DATA);
	fields = set_datatype_field(fields, DATATYPE_DWORD);
	r = chunk_put_byte_fields(chunk, 10, fields);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_datatype(chunk, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_DWORD);

	fields = 0;
	fields = set_class_field(fields, CLASS_DATA);
	fields = set_datatype_field(fields, DATATYPE_QWORD);
	r = chunk_put_byte_fields(chunk, 10, fields);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_datatype(chunk, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_QWORD);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_put_byte_datatype(void){
	struct bytechunk *chunk;
	uint32_t datatype;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_put_byte_datatype(chunk, 10, DATATYPE_BYTE);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_datatype(chunk, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_BYTE);
	
	r = chunk_put_byte_datatype(chunk, 10, DATATYPE_WORD);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_datatype(chunk, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_WORD);

	r = chunk_put_byte_datatype(chunk, 10, DATATYPE_DWORD);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_datatype(chunk, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_DWORD);

	r = chunk_put_byte_datatype(chunk, 10, DATATYPE_QWORD);
	FAIL_IF_ERR(r);
	r = chunk_get_byte_datatype(chunk, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_QWORD);

	free_bytechunk(chunk);
	return 0;
}


/****************************    Items    ************************************/


static int test_chunk_first_item(void){
	struct bytechunk *chunk;
	uint64_t addr;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_class(chunk, 10, CLASS_CODE);
	FAIL_IF_ERR(r);
	addr = 0;
	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_class(chunk, 10, CLASS_DATA);
       	FAIL_IF_ERR(r);
	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_class(chunk, 14, CLASS_CODE);
       	FAIL_IF_ERR(r);
	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_class(chunk, 14, CLASS_DATA);
       	FAIL_IF_ERR(r);
	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);
	
	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_class(chunk, 19, CLASS_CODE);
       	FAIL_IF_ERR(r);
	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 19);
	
	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_class(chunk, 19, CLASS_DATA);
       	FAIL_IF_ERR(r);
	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 19);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = chunk_first_item(chunk, &addr);
	FAIL_IF(!r);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_last_item(void){
	struct bytechunk *chunk;
	uint64_t addr;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_class(chunk, 14, CLASS_DATA);
	r |= chunk_set_range_class(chunk, 15, 17, CLASS_TAIL);
       	FAIL_IF_ERR(r);
	r = chunk_last_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_next_item(void){
	struct bytechunk *chunk;
	uint64_t addr;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);
	r = 0;
	r |= chunk_set_byte_class(chunk, 10, CLASS_DATA); /* HEAD */
	r |= chunk_set_byte_class(chunk, 11, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 12, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 13, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 14, CLASS_CODE); /* HEAD */
	r |= chunk_set_byte_class(chunk, 15, CLASS_CODE); /* HEAD */
	r |= chunk_set_byte_class(chunk, 16, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 17, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 18, CLASS_DATA); /* HEAD */
	r |= chunk_set_byte_class(chunk, 19, CLASS_TAIL);
       	FAIL_IF_ERR(r);
	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	
	r = chunk_next_item(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);
	r = chunk_next_item(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 15);
	r = chunk_next_item(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 18);
	r = chunk_next_item(chunk, addr, &addr);
	FAIL_IF(!r);

	free_bytechunk(chunk);
	return 0;
}


static int test_chunk_prev_item(void){
	struct bytechunk *chunk;
	uint64_t addr;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);
	r = 0;
	r |= chunk_set_byte_class(chunk, 10, CLASS_DATA); /* HEAD */
	r |= chunk_set_byte_class(chunk, 11, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 12, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 13, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 14, CLASS_CODE); /* HEAD */
	r |= chunk_set_byte_class(chunk, 15, CLASS_CODE); /* HEAD */
	r |= chunk_set_byte_class(chunk, 16, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 17, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 18, CLASS_DATA); /* HEAD */
	r |= chunk_set_byte_class(chunk, 19, CLASS_TAIL);
       	FAIL_IF_ERR(r);
	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);

	r = chunk_last_item(chunk, &addr);
	FAIL_IF_ERR(r);	

	r = chunk_prev_item(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 15);
	r = chunk_prev_item(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);
	r = chunk_prev_item(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);
	r = chunk_prev_item(chunk, addr, &addr);
	FAIL_IF(!r);
	
	free_bytechunk(chunk);
	return 0;
} 


static int test_chunk_item_head(void){
	struct bytechunk *chunk;
	uint64_t addr;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_class(chunk, 14, CLASS_DATA);
	r |= chunk_set_range_class(chunk, 15, 19, CLASS_TAIL);
       	FAIL_IF_ERR(r);

	r = chunk_item_head(chunk, 18, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_item_end(void){
	struct bytechunk *chunk;
	uint64_t addr;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = chunk_set_byte_class(chunk, 14, CLASS_DATA);
	r |= chunk_set_range_class(chunk, 15, 19, CLASS_TAIL);
       	FAIL_IF_ERR(r);

	r = chunk_item_end(chunk, 14, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 19);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_first_not_tail(void){
	struct bytechunk *chunk;
	uint64_t addr;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);

	r = chunk_first_not_tail(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_last_not_tail(void){
	struct bytechunk *chunk;
	uint64_t addr;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = 0;
	r |= chunk_set_byte_class(chunk, 17, CLASS_CODE);
	r |= chunk_set_byte_class(chunk, 18, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 19, CLASS_TAIL);
	FAIL_IF_ERR(r);

	addr = 0;
	r = chunk_last_not_tail(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 17);
	return 0;
}


static int test_chunk_next_not_tail(void){
	struct bytechunk *chunk;
	uint64_t addr;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = 0;
	r |= chunk_set_byte_class(chunk, 10, CLASS_UNKNOWN);
	r |= chunk_set_byte_class(chunk, 11, CLASS_DATA);
	r |= chunk_set_byte_class(chunk, 12, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 13, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 14, CLASS_UNKNOWN);
	r |= chunk_set_byte_class(chunk, 15, CLASS_UNKNOWN);
	r |= chunk_set_byte_class(chunk, 16, CLASS_UNKNOWN);
	r |= chunk_set_byte_class(chunk, 17, CLASS_CODE);
	r |= chunk_set_byte_class(chunk, 18, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 19, CLASS_TAIL);
	FAIL_IF_ERR(r);

	addr = 0;
	r = chunk_first_not_tail(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	r = chunk_next_not_tail(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 11);

	r = chunk_next_not_tail(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	r = chunk_next_not_tail(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 15);

	r = chunk_next_not_tail(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 16);

	r = chunk_next_not_tail(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 17);

	r = chunk_next_not_tail(chunk, addr, &addr);
	FAIL_IF(!r);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_prev_not_tail(void){
	struct bytechunk *chunk;
	uint64_t addr;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_set_range_class_unknown(chunk, 10, 19);
	FAIL_IF_ERR(r);
	r = 0;
	r |= chunk_set_byte_class(chunk, 10, CLASS_UNKNOWN);
	r |= chunk_set_byte_class(chunk, 11, CLASS_DATA);
	r |= chunk_set_byte_class(chunk, 12, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 13, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 14, CLASS_UNKNOWN);
	r |= chunk_set_byte_class(chunk, 15, CLASS_UNKNOWN);
	r |= chunk_set_byte_class(chunk, 16, CLASS_UNKNOWN);
	r |= chunk_set_byte_class(chunk, 17, CLASS_CODE);
	r |= chunk_set_byte_class(chunk, 18, CLASS_TAIL);
	r |= chunk_set_byte_class(chunk, 19, CLASS_TAIL);
	FAIL_IF_ERR(r);

	addr = 0;
	r = chunk_last_not_tail(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 17);

	r = chunk_prev_not_tail(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 16);

	r = chunk_prev_not_tail(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 15);

	r = chunk_prev_not_tail(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	r = chunk_prev_not_tail(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 11);

	r = chunk_prev_not_tail(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	r = chunk_prev_not_tail(chunk, addr, &addr);
	FAIL_IF(!r);
	
	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_create_code_item(void){
	struct bytechunk *chunk;
	uint64_t addr;
	uint32_t class;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_create_code_item(chunk, 14, 17);
	FAIL_IF_ERR(r);

	addr = 0;
	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	r = chunk_get_byte_class(chunk, addr, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_CODE);

	r = chunk_item_end(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 17);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_create_data_item_byte(void){
	struct bytechunk *chunk;
	uint64_t addr;
	uint32_t class;
	uint32_t datatype;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_create_data_item_byte(chunk, 14);
	FAIL_IF_ERR(r);

	addr = 0;
	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	r = chunk_get_byte_class(chunk, addr, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);
	
	r = chunk_get_byte_datatype(chunk, addr, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_BYTE);

	r = chunk_item_end(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	free_bytechunk(chunk);
	return 0;
}

static int test_chunk_create_data_item_word(void){
	struct bytechunk *chunk;
	uint64_t addr;
	uint32_t class;
	uint32_t datatype;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_create_data_item_word(chunk, 14);
	FAIL_IF_ERR(r);

	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	r = chunk_get_byte_class(chunk, addr, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);

	r = chunk_get_byte_datatype(chunk, addr, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_WORD);

	r = chunk_item_end(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 15);

	free_bytechunk(chunk);
	return 0;
}


static int test_chunk_create_data_item_dword(void){
	struct bytechunk *chunk;
	uint64_t addr;
	uint32_t class;
	uint32_t datatype;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_create_data_item_dword(chunk, 14);
	FAIL_IF_ERR(r);

	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	r = chunk_get_byte_class(chunk, addr, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);

	datatype = -1;	
	r = chunk_get_byte_datatype(chunk, addr, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_DWORD);

	r = chunk_item_end(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 17);

	free_bytechunk(chunk);
	return 0;
}


static int test_chunk_create_data_item_qword(void){
	struct bytechunk *chunk;
	uint64_t addr;
	uint32_t class;
	uint32_t datatype;
	register int r;

	chunk = new_bytechunk(10, 19);
	FAIL_IF(chunk == NULL);

	r = chunk_create_data_item_qword(chunk, 11);
	FAIL_IF_ERR(r);

	r = chunk_first_item(chunk, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 11);

	r = chunk_get_byte_class(chunk, addr, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);

	datatype = -1;	
	r = chunk_get_byte_datatype(chunk, addr, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_QWORD);

	r = chunk_item_end(chunk, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 18);

	free_bytechunk(chunk);
	return 0;
}



static struct test tests[] = {
	{"newfree", test_newfree},
	{"chunk_get_byte_fields", test_chunk_get_byte_fields},
	{"chunk_get_byte_fields-bounds", test_chunk_get_byte_fields_bounds},
	{"chunk_put_byte_fields", test_chunk_put_byte_fields},
	{"chunk_put_byte_fields-bounds", test_chunk_put_byte_fields_bounds},
	{"merge_chunks", test_merge_chunks},
	{"expand_chunk_up", test_expand_chunk_up},
	{"expand_chunk_down", test_expand_chunk_down},
	/* VALUE FIELD */
	{"chunk_get_byte", test_chunk_get_byte},
	{"chunk_put_byte", test_chunk_put_byte},
	{"copy_from_chunk", test_copy_from_chunk},
	{"copy_from_chunk-invalid_value", test_copy_from_chunk_invalid_value},
	{"copy_to_chunk", test_copy_to_chunk},
	{"chunk_set_range_values", test_chunk_set_range_values},
	{"chunk_get_word", test_chunk_get_word},
	{"chunk_get_dword", test_chunk_get_dword},
	{"chunk_get_qword", test_chunk_get_qword},
	{"chunk_put_word", test_chunk_put_word},
	{"chunk_put_dword", test_chunk_put_dword},
	{"chunk_put_qword", test_chunk_put_qword},
	/* CLASS FIELD */
	{"chunk_get_byte_class", test_chunk_get_byte_class},
	{"chunk_set_byte_class", test_chunk_set_byte_class},
	{"is_chunk_range_class", test_is_chunk_range_class},
	{"chunk_set_range_class", test_chunk_set_range_class},
	{"is_chunk_range_class_unknown", test_is_chunk_range_class_unknown},
	{"is_chunk_range_class_code", test_is_chunk_range_class_code},
	{"is_chunk_range_class_data", test_is_chunk_range_class_data},
	{"is_chunk_range_class_tail", test_is_chunk_range_class_tail},
	{"chunk_set_range_class_unknown", test_chunk_set_range_class_unknown},
	{"chunk_set_range_class_code", test_chunk_set_range_class_code},
	{"chunk_set_range_class_data", test_chunk_set_range_class_data},
	{"chunk_set_range_class_tail", test_chunk_set_range_class_tail},
	/* DATATYPE FIELD */
	{"chunk_get_byte_datatype", test_chunk_get_byte_datatype},
	{"chunk_put_byte_datatype", test_chunk_put_byte_datatype},
	/* ITEMS */
	{"chunk_first_item", test_chunk_first_item},
	{"chunk_last_item", test_chunk_last_item},
	{"chunk_next_item", test_chunk_next_item},
	{"chunk_prev_item", test_chunk_prev_item},
	{"chunk_item_head", test_chunk_item_head},
	{"chunk_item_end", test_chunk_item_end},
	{"chunk_first_not_tail", test_chunk_first_not_tail},
	{"chunk_last_not_tail", test_chunk_last_not_tail},
	{"chunk_next_not_tail", test_chunk_next_not_tail},
	{"chunk_prev_not_tail", test_chunk_prev_not_tail},
	{"chunk_create_code_item", test_chunk_create_code_item},
	{"chunk_create_data_item_byte", test_chunk_create_data_item_byte},
	{"chunk_create_data_item_word", test_chunk_create_data_item_word},
	{"chunk_create_data_item_dword", test_chunk_create_data_item_dword},
	{"chunk_create_data_item_qword", test_chunk_create_data_item_qword},
	{NULL, NULL},
};

void test_bytechunk_init(void) __attribute__ ((constructor));
void test_bytechunk_init(void){
	add_module_tests("byteschunk", tests);
}
