#include <stdio.h>
#include <string.h>

#include "bytes.h"
#include "bytefields.h"
#include "bytechunk.h"
#include "testrunner.h"

static int test_newfree(void){
	struct bytes *bytes;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	free_bytes(bytes);
	return 0;
}

static int test_enable_bytes(void){
	struct bytes *bytes;
	struct bytechunk *chunk;
	int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 100, 200);
	FAIL_IF(r != 0);

	chunk = bytes_first_chunk(bytes);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 100);
	FAIL_IF(chunk->bc_last != 200);
	FAIL_IF(chunk->bc_bytes == NULL);
	
	free_bytes(bytes);
	return 0;
}

static int test_enable_bytes_forward(){
	struct bytes *bytes;
	struct bytechunk *c;
	int i;
	uint64_t first;
	uint64_t last;

	bytes = new_bytes();

	for(i = 0; i < 5; i++){
		first = i*5;
		last = first + 2;
		enable_bytes(bytes, first, last);
	}
	
	c = bytes_first_chunk(bytes);
	for(i = 0; i < 5; i++){
		first = i*5;
		last = first + 2;

		FAIL_IF(c == NULL);
		FAIL_IF(c->bc_first != first);
		FAIL_IF(c->bc_last != last);
		c = bytes_next_chunk(c);
	}
	FAIL_IF(c != NULL);

	c = bytes_last_chunk(bytes);
	for(i = 4; i >= 0; i--){
		first = i * 5;
		last = first + 2;

		FAIL_IF(c == NULL);
		FAIL_IF(c->bc_first != first);
		FAIL_IF(c->bc_last != last);
		c = bytes_prev_chunk(c);
	}
	FAIL_IF(c != NULL);

	free_bytes(bytes);
	return 0;
}

static int test_enable_bytes_reverse(){
	struct bytes *bytes;
	struct bytechunk *c;
	int i;
	uint64_t first;
	uint64_t last;

	bytes = new_bytes();

	for(i = 0; i < 5; i++){
		first = 20-i*5;
		last = first+2;
		enable_bytes(bytes, first, last);
	}

	c = bytes_first_chunk(bytes);
	for(i = 0; i < 5; i++){
		first = i*5;
		last = first+2;

		FAIL_IF(c == NULL);
		FAIL_IF(c->bc_first != first);
		FAIL_IF(c->bc_last != last);
		c = bytes_next_chunk(c);
	}
	FAIL_IF(c != NULL);

	c = bytes_last_chunk(bytes);
	for(i = 4; i >= 0; i--){
		first = i * 5;
		last = first + 2;

		FAIL_IF(c == NULL);
		FAIL_IF(c->bc_first != first);
		FAIL_IF(c->bc_last != last);
		c = bytes_prev_chunk(c);
	}
	FAIL_IF(c != NULL);

	free_bytes(bytes);
	return 0;
}

static int test_enable_bytes_middle(void){
	struct bytes *bytes;
	struct bytechunk *c;
	int i;
	uint64_t first;
	uint64_t last;

	bytes = new_bytes();

	for(i = 0; i < 10; i++){
		first = i < 5 ? i*5*2 : (i-5)*5*2+5;
		last = first + 2;
		enable_bytes(bytes, first, last);
	}
	
	c = bytes_first_chunk(bytes);
	for(i = 0; i < 10; i++){
		first = i*5;
		last = first+2;

		FAIL_IF(c == NULL);
		FAIL_IF(c->bc_first != first);
		FAIL_IF(c->bc_last != last);
		c = bytes_next_chunk(c);
	}
	FAIL_IF(c != NULL);
	
	c = bytes_last_chunk(bytes);
	for(i = 9; i >= 0; i--){
		first = i*5;
		last = first+2;

		FAIL_IF(c == NULL);
		FAIL_IF(c->bc_first != first);
		FAIL_IF(c->bc_last != last);
		c = bytes_prev_chunk(c);
	}
	FAIL_IF(c != NULL);
	
	
	free_bytes(bytes);
	return 0;
}


static int test_enable_bytes_expand_up(void){
	struct bytes *bytes;
	struct bytechunk *chunk;
	uint64_t first;
	uint64_t last;
	int i;

	bytes = new_bytes();

	for(i = 0; i < 10; i++){
		first = i*5;
		last = first + 4;
		enable_bytes(bytes, first, last);
	}

	chunk = bytes_first_chunk(bytes);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 0);
	FAIL_IF(chunk->bc_last != 49);
	chunk = bytes_next_chunk(chunk);
	FAIL_IF(chunk != NULL);
	free_bytes(bytes);
	return 0;
}

static int test_enable_bytes_expand_down(void){
	struct bytes *bytes;
	struct bytechunk *chunk;
	uint64_t first;
	uint64_t last;
	int i;

	bytes = new_bytes();

	for(i = 0; i < 10; i++){
		first = (9*5)-i*5;
		last = first + 4;
		enable_bytes(bytes, first, last);
	}

	chunk = bytes_first_chunk(bytes);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 0);
	FAIL_IF(chunk->bc_last != 49);
	chunk = bytes_next_chunk(chunk);
	FAIL_IF(chunk != NULL);
	free_bytes(bytes);
	return 0;
}

static int test_enable_bytes_merge(void){
	struct bytes *bytes;
	struct bytechunk *chunk;
	struct bytechunk *prev_chunk;
	struct bytechunk *next_chunk;
	uint64_t first;
	uint64_t last;
	int i;

	bytes = new_bytes();

	for(i = 0; i < 10; i++){
		first = i < 5 ? i*5*2 : (i-5)*5*2+5;
		last = first + 4;
		enable_bytes(bytes, first, last);
	}
	
	chunk = bytes_first_chunk(bytes);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 0);
	FAIL_IF(chunk->bc_last != 49);
	next_chunk = bytes_next_chunk(chunk);
	FAIL_IF(next_chunk != NULL);
	prev_chunk = bytes_prev_chunk(chunk);
	FAIL_IF(prev_chunk != NULL);
	free_bytes(bytes);
	return 0;
}

static int test_get_byte_fields(void){
	struct bytes *bytes;
	struct bytechunk *chunk;
	uint32_t fields;
	int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = 0;
	r |= enable_bytes(bytes, 0, 9);
	r |= enable_bytes(bytes, 20, 29);
	r |= enable_bytes(bytes, 40, 49);
	FAIL_IF(r != 0);

	chunk = bytes_first_chunk(bytes);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 0);
	FAIL_IF(chunk->bc_last != 9);
	chunk->bc_bytes[0] = 0x11111111;
	chunk->bc_bytes[4] = 0x22222222;
	chunk->bc_bytes[9] = 0x33333333;

	chunk = bytes_next_chunk(chunk);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 20);
	FAIL_IF(chunk->bc_last != 29);
	chunk->bc_bytes[0] = 0x44444444;
	chunk->bc_bytes[4] = 0x55555555;
	chunk->bc_bytes[9] = 0x66666666;

	chunk = bytes_next_chunk(chunk);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 40);
	FAIL_IF(chunk->bc_last != 49);
	chunk->bc_bytes[0] = 0x77777777;
	chunk->bc_bytes[4] = 0x88888888;
	chunk->bc_bytes[9] = 0x99999999;

	chunk = bytes_next_chunk(chunk);
	FAIL_IF(chunk != NULL);

	r = get_byte_fields(bytes, 0, &fields);
	FAIL_IF(r != 0);
	FAIL_IF(fields != 0x11111111);
	r = get_byte_fields(bytes, 4, &fields);
	FAIL_IF(r != 0);
	FAIL_IF(fields != 0x22222222);
	r = get_byte_fields(bytes, 9, &fields);
	FAIL_IF(r != 0);
	FAIL_IF(fields != 0x33333333);

	r = get_byte_fields(bytes, 20, &fields);
	FAIL_IF(r != 0);
	FAIL_IF(fields != 0x44444444);
	r = get_byte_fields(bytes, 24, &fields);
	FAIL_IF(r != 0);
	FAIL_IF(fields != 0x55555555);
	r = get_byte_fields(bytes, 29, &fields);
	FAIL_IF(r != 0);
	FAIL_IF(fields != 0x66666666);
	
	r = get_byte_fields(bytes, 40, &fields);
	FAIL_IF(r != 0);
	FAIL_IF(fields != 0x77777777);
	r = get_byte_fields(bytes, 44, &fields);
	FAIL_IF(r != 0);
	FAIL_IF(fields != 0x88888888);
	r = get_byte_fields(bytes, 49, &fields);
	FAIL_IF(r != 0);
	FAIL_IF(fields != 0x99999999);

	free_bytes(bytes);
	return 0;
}


static int test_set_byte_fields(void){
	struct bytes *bytes;
	uint32_t fields;
	uint32_t ar[9][2] = {
		{ 0, 0x11111111},
		{ 4, 0x22222222},
		{ 9, 0x33333333},
		{20, 0x44444444},
		{24, 0x55555555},
		{29, 0x66666666},
		{40, 0x77777777},
		{44, 0x88888888},
		{49, 0x99999999},
	};
	int r;
	int i;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = 0;
	r |= enable_bytes(bytes, 0, 9);
	r |= enable_bytes(bytes, 20, 29);
	r |= enable_bytes(bytes, 40, 49);
	FAIL_IF(r != 0);

	for(i = 0; i < 9; i++){
		r = set_byte_fields(bytes, ar[i][0], ar[i][1]);
		FAIL_IF(r != 0);
	}
	
	for(i = 0; i < 9; i++){
		r = get_byte_fields(bytes, ar[i][0], &fields);
		FAIL_IF(r != 0);
		FAIL_IF(ar[i][1] != fields);
	}

	free_bytes(bytes);
	return 0;
}



/****************************   Value   **************************************/


static int test_copy_from_bytes(void){
	struct bytes *bytes;
	uint8_t src[10] = {
		0x10, 0x11, 0x12, 0x13, 0x14,
		0x15, 0x16, 0x17, 0x18, 0x19
	};
	uint8_t dst[10] = { 0 };
	int r;
	int i;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF(r != 0);

	for(i = 0; i < sizeof(src); i++){
		r = set_byte_fields(bytes, 10+i, set_value_field(src[i], VALUE_VALID));
		FAIL_IF(r != 0);
	}

	r = copy_from_bytes(bytes, 10, dst, sizeof(dst));
	FAIL_IF(r != 0);
	r = memcmp(src, dst, sizeof(src));
	FAIL_IF(r != 0);
	free_bytes(bytes);
	return 0;
}


static int test_bytes_get_byte(void){
	struct bytes *bytes;
	uint8_t byte;
	int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF(r != 0);

	r = 0;	
	r |= set_byte_fields(bytes, 10, set_value_field(0xAB, VALUE_VALID));
	FAIL_IF(r != 0);

	byte = 0;
	r = bytes_get_byte(bytes, 10, &byte);
	FAIL_IF(r != 0);
	FAIL_IF(byte != 0xAB);

	free_bytes(bytes);
	return 0;
}


static int test_bytes_get_word(void){
	struct bytes *bytes;
	uint16_t word;
	int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF(r != 0);

	r = 0;	
	r |= set_byte_fields(bytes, 10, set_value_field(0xAB, VALUE_VALID));
	r |= set_byte_fields(bytes, 11, set_value_field(0xCD, VALUE_VALID));
	FAIL_IF(r != 0);

	word = 0;
	r = bytes_get_word(bytes, 10, &word);
	FAIL_IF(r != 0);
	FAIL_IF(word != 0xCDAB);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_get_dword(void){
	struct bytes *bytes;
	uint32_t dword;
	int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF(r != 0);

	r = 0;	
	r |= set_byte_fields(bytes, 10, set_value_field(0xAB, VALUE_VALID));
	r |= set_byte_fields(bytes, 11, set_value_field(0xCD, VALUE_VALID));
	r |= set_byte_fields(bytes, 12, set_value_field(0xEF, VALUE_VALID));
	r |= set_byte_fields(bytes, 13, set_value_field(0x01, VALUE_VALID));
	FAIL_IF(r != 0);

	dword = 0;
	r = bytes_get_dword(bytes, 10, &dword);
	FAIL_IF(r != 0);
	FAIL_IF(dword != 0x01EFCDAB);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_get_qword(void){
	struct bytes *bytes;
	uint64_t qword;
	int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF(r != 0);

	r = 0;	
	r |= set_byte_fields(bytes, 10, set_value_field(0xAB, VALUE_VALID));
	r |= set_byte_fields(bytes, 11, set_value_field(0xCD, VALUE_VALID));
	r |= set_byte_fields(bytes, 12, set_value_field(0xEF, VALUE_VALID));
	r |= set_byte_fields(bytes, 13, set_value_field(0x01, VALUE_VALID));
	r |= set_byte_fields(bytes, 14, set_value_field(0x02, VALUE_VALID));
	r |= set_byte_fields(bytes, 15, set_value_field(0x03, VALUE_VALID));
	r |= set_byte_fields(bytes, 16, set_value_field(0x04, VALUE_VALID));
	r |= set_byte_fields(bytes, 17, set_value_field(0x05, VALUE_VALID));
	FAIL_IF(r != 0);

	qword = 0;
	r = bytes_get_qword(bytes, 10, &qword);
	FAIL_IF(r != 0);
	FAIL_IF(qword != 0x0504030201EFCDAB);

	free_bytes(bytes);
	return 0;
}


static int test_copy_to_bytes(void){
	struct bytes *bytes;
	uint8_t src[10];
	uint8_t dst[10];
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF(r != 0);
	
	memcpy(src, "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19", 10);
	r = copy_to_bytes(bytes, 10, src, sizeof(src));
	FAIL_IF_ERR(r);

	r = copy_from_bytes(bytes, 10, dst, sizeof(dst));
	FAIL_IF_ERR(r);
	FAIL_IF(memcmp(dst, src, sizeof(src)) != 0);

	free_bytes(bytes);
	return 0;
}

static int test_set_bytes(void){
	struct bytes *bytes;
	uint8_t dst[10];
	int i;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF(r != 0);

	r = set_bytes(bytes, 0x90, 10, 19);
	FAIL_IF(r != 0);

	r = copy_from_bytes(bytes, 10, dst, sizeof(dst));
	FAIL_IF_ERR(r);

	r = 0;
	for(i = 0; i < sizeof(dst); i++){
		r |= dst[i] ^ 0x90;
	}
	FAIL_IF(r != 0);
	return 0;
}

static int test_bytes_put_byte(void){
	struct bytes *bytes;
	uint8_t byte;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = bytes_put_byte(bytes, 10, 0x90);
	FAIL_IF_ERR(r);

	byte = 0x00;
	r = bytes_get_byte(bytes, 10, &byte);
	FAIL_IF_ERR(r);
	FAIL_IF(byte != 0x90);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_put_word(void){
	struct bytes *bytes;
	uint16_t word;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = bytes_put_word(bytes, 10, 0x0123);
	FAIL_IF_ERR(r);

	word = 0x0000;
	r = bytes_get_word(bytes, 10, &word);
	FAIL_IF_ERR(r);
	FAIL_IF(word != 0x0123);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_put_dword(void){
	struct bytes *bytes;
	uint32_t dword;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = bytes_put_dword(bytes, 10, 0x01234567);
	FAIL_IF_ERR(r);

	dword = 0x000000000;
	r = bytes_get_dword(bytes, 10, &dword);
	FAIL_IF_ERR(r);
	FAIL_IF(dword != 0x01234567);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_put_qword(void){
	struct bytes *bytes;
	uint64_t qword;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = bytes_put_qword(bytes, 10, 0x0123456789ABCDEF);
	FAIL_IF_ERR(r);

	qword = 0x00000000000000000;
	r = bytes_get_qword(bytes, 10, &qword);
	FAIL_IF_ERR(r);
	FAIL_IF(qword != 0x0123456789ABCDEF);

	free_bytes(bytes);
	return 0;
}



/*****************************   Class   *************************************/



static int test_bytes_get_byte_class(void){
	struct bytes *bytes;
	register int r;
	uint32_t class;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	/* DATA */
	r = set_byte_fields(bytes, 10, set_class_field(0, CLASS_DATA));
	FAIL_IF_ERR(r);
	r = bytes_get_byte_class(bytes, 10, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);

	/* CODE */
	r = set_byte_fields(bytes, 10, set_class_field(0, CLASS_CODE));
	FAIL_IF_ERR(r);
	r = bytes_get_byte_class(bytes, 10, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_CODE);

	/* TAIL */
	r = set_byte_fields(bytes, 10, set_class_field(0, CLASS_TAIL));
	FAIL_IF_ERR(r);
	r = bytes_get_byte_class(bytes, 10, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_TAIL);

	/* UNKNOWN */
	r = set_byte_fields(bytes, 10, set_class_field(0, CLASS_UNKNOWN));
	FAIL_IF_ERR(r);
	r = bytes_get_byte_class(bytes, 10, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_UNKNOWN);


	free_bytes(bytes);
	return 0;
}


static int test_bytes_set_byte_class(void){
	struct bytes *bytes;
	uint32_t class;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = bytes_set_byte_class(bytes, 10, CLASS_CODE);
	FAIL_IF_ERR(r);
	r = bytes_get_byte_class(bytes, 10, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_CODE);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_set_range_class(void){
	struct bytes *bytes;
	uint32_t class;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = 0;
	r |= bytes_set_range_class(bytes, 10, 12, CLASS_CODE);
	r |= bytes_set_range_class(bytes, 13, 15, CLASS_DATA);
	r |= bytes_set_range_class(bytes, 16, 19, CLASS_TAIL);
	FAIL_IF_ERR(r);

	class = 0;
	r = bytes_get_byte_class(bytes, 10, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_CODE);

	class = 0;
	r = bytes_get_byte_class(bytes, 11, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_CODE);

	class = 0;
	r = bytes_get_byte_class(bytes, 13, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);

	class = 0;
	r = bytes_get_byte_class(bytes, 14, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);
	
	class = 0;
	r = bytes_get_byte_class(bytes, 15, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);

	class = 0;
	r = bytes_get_byte_class(bytes, 16, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_TAIL);

	class = 0;
	r = bytes_get_byte_class(bytes, 17, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_TAIL);

	class = 0;
	r = bytes_get_byte_class(bytes, 18, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_TAIL);

	class = 0;
	r = bytes_get_byte_class(bytes, 19, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_TAIL);

	free_bytes(bytes);
	return 0;
}

/*****************************   Datatype   **********************************/


static int test_get_bytes_datatype(void){
	struct bytes *bytes;
	uint32_t datatype;
	uint32_t fields;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	/* BYTE */
	fields = 0;
	fields = set_class_field(fields, CLASS_DATA);
	fields = set_datatype_field(fields, DATATYPE_BYTE);
	r = set_byte_fields(bytes, 10, fields);
	FAIL_IF_ERR(r);

	datatype = 0;
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_BYTE);

	/* WORD */
	fields = 0;
	fields = set_class_field(fields, CLASS_DATA);
	fields = set_datatype_field(fields, DATATYPE_WORD);
	r = set_byte_fields(bytes, 10, fields);
	FAIL_IF(r != 0);
	
	datatype = 0;
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_WORD);

	/* DWORD */
	fields = 0;
	fields = set_class_field(fields, CLASS_DATA);
	fields = set_datatype_field(fields, DATATYPE_DWORD);
	r = set_byte_fields(bytes, 10, fields);
	FAIL_IF(r != 0);
	
	datatype = 0;
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_DWORD); 
	
	/* QWORD */
	fields = 0;
	fields = set_class_field(fields, CLASS_DATA);
	fields = set_datatype_field(fields, DATATYPE_QWORD);
	r = set_byte_fields(bytes, 10, fields);
	FAIL_IF(r != 0);
	
	datatype = 0;
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_QWORD); 

	free_bytes(bytes);
	return 0;
}

static int test_set_bytes_datatype(void){
	struct bytes *bytes;
	uint32_t datatype;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	/* BYTE */
	r = set_bytes_datatype(bytes, 10, DATATYPE_BYTE);
	FAIL_IF_ERR(r);
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_BYTE);

	/* WORD */
	r = set_bytes_datatype(bytes, 10, DATATYPE_WORD);
	FAIL_IF_ERR(r);
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_WORD);

	/* DWORD */
	r = set_bytes_datatype(bytes, 10, DATATYPE_DWORD);
	FAIL_IF_ERR(r);
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_DWORD);
	
	/* QWORD */
	r = set_bytes_datatype(bytes, 10, DATATYPE_QWORD);
	FAIL_IF_ERR(r);
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_QWORD);

	free_bytes(bytes);
	return 0;
}



/**********************************   Items   ********************************/

static int test_bytes_first_addr(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_first_addr(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	free_bytes(bytes);
	return 0;
}
static int test_bytes_last_addr(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_last_addr(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 19);

	free_bytes(bytes);
	return 0;
}
static int test_bytes_next_addr(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = enable_bytes(bytes, 30, 39);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_next_addr(bytes, 19, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 30);

	dis_errno = 0;
	addr = 0;
	r = bytes_next_addr(bytes, 39, &addr);
	FAIL_IF(!r);
	FAIL_IF(dis_errno != DER_NOTFOUND);

	free_bytes(bytes);
	return 0;
}
static int test_bytes_prev_addr(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = enable_bytes(bytes, 30, 39);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_prev_addr(bytes, 30, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 19);

	dis_errno = 0;
	addr = 0;
	r = bytes_prev_addr(bytes, 10, &addr);
	FAIL_IF(!r);
	FAIL_IF(dis_errno != DER_NOTFOUND);

	free_bytes(bytes);
	return 0;
}


static int test_bytes_first_item(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);
	r = enable_bytes(bytes, 30, 39);
	FAIL_IF_ERR(r);
	r = enable_bytes(bytes, 50, 59);
	FAIL_IF_ERR(r);

	dis_errno = 0;
	r = bytes_first_item(bytes, &addr);
	FAIL_IF(!r);
	FAIL_IF(dis_errno != DER_NOTFOUND);
	
	addr = 0;
	r = bytes_set_byte_class(bytes, 52, CLASS_DATA);
	FAIL_IF_ERR(r);
	r = bytes_first_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 52);

	addr = 0;
	r = bytes_set_byte_class(bytes, 39, CLASS_CODE);
	FAIL_IF_ERR(r);
	r = bytes_first_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 39);

	addr = 0;
	r = bytes_set_byte_class(bytes, 10, CLASS_CODE);
	FAIL_IF_ERR(r);
	r = bytes_first_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_last_item(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);
	r = enable_bytes(bytes, 30, 39);
	FAIL_IF_ERR(r);
	r = enable_bytes(bytes, 50, 59);
	FAIL_IF_ERR(r);

	dis_errno = 0;
	r = bytes_last_item(bytes, &addr);
	FAIL_IF(!r);
	FAIL_IF(dis_errno != DER_NOTFOUND);

	addr = 0;
	r = bytes_set_byte_class(bytes, 10, CLASS_CODE);
	FAIL_IF_ERR(r);
	r = bytes_last_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);
	addr = 0;
	r = bytes_set_byte_class(bytes, 11, CLASS_TAIL);
	FAIL_IF_ERR(r);
	r = bytes_last_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	addr = 0;
	r = bytes_set_byte_class(bytes, 33, CLASS_DATA);
	FAIL_IF_ERR(r);
	r = bytes_last_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 33);
	addr = 0;
	r = bytes_set_byte_class(bytes, 34, CLASS_TAIL);
	FAIL_IF_ERR(r);
	r = bytes_last_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 33);

	addr = 0;
	r = bytes_set_byte_class(bytes, 59, CLASS_DATA);
	FAIL_IF_ERR(r);
	r = bytes_last_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 59);

	free_bytes(bytes);
	return 0;
}


static int test_bytes_next_item(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);
	r = enable_bytes(bytes, 30, 39);
	FAIL_IF_ERR(r);
	r = enable_bytes(bytes, 50, 59);
	FAIL_IF_ERR(r);
	r = enable_bytes(bytes, 70, 79);
	FAIL_IF_ERR(r);

	r = 0;
	r |= bytes_set_byte_class(bytes, 11, CLASS_DATA);
	r |= bytes_set_byte_class(bytes, 12, CLASS_TAIL);
	r |= bytes_set_byte_class(bytes, 19, CLASS_CODE);
	r |= bytes_set_byte_class(bytes, 52, CLASS_CODE);
	r |= bytes_set_byte_class(bytes, 53, CLASS_TAIL);
	r |= bytes_set_byte_class(bytes, 57, CLASS_CODE);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_first_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 11);
	
	r = bytes_next_item(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 19);

	r = bytes_next_item(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 52);

	r = bytes_next_item(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 57);

	dis_errno = 0;	
	r = bytes_next_item(bytes, addr, &addr);
	FAIL_IF(!r);
	FAIL_IF(dis_errno != DER_NOTFOUND);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_prev_item(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);
	r = enable_bytes(bytes, 30, 39);
	FAIL_IF_ERR(r);
	r = enable_bytes(bytes, 50, 59);
	FAIL_IF_ERR(r);
	r = enable_bytes(bytes, 70, 79);
	FAIL_IF_ERR(r);

	r = 0;
	r |= bytes_set_byte_class(bytes, 31, CLASS_DATA);
	r |= bytes_set_byte_class(bytes, 32, CLASS_TAIL);
	r |= bytes_set_byte_class(bytes, 39, CLASS_CODE);
	r |= bytes_set_byte_class(bytes, 72, CLASS_CODE);
	r |= bytes_set_byte_class(bytes, 73, CLASS_TAIL);
	r |= bytes_set_byte_class(bytes, 77, CLASS_CODE);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_last_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 77);
	
	r = bytes_prev_item(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 72);
	
	r = bytes_prev_item(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 39);
	
	r = bytes_prev_item(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 31);

	dis_errno = 0;	
	r = bytes_prev_item(bytes, addr, &addr);
	FAIL_IF(!r);
	FAIL_IF(dis_errno != DER_NOTFOUND);

	free_bytes(bytes);
	return 0;
}


static int test_bytes_item_head_unknown(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_item_head(bytes, 10, &addr);
	FAIL_IF(r == 0);

	addr = 0;
	r = bytes_item_head(bytes, 14, &addr);
	FAIL_IF(r == 0);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_item_head_data(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	set_byte_fields(bytes, 10, set_class_field(0, CLASS_DATA));
	set_byte_fields(bytes, 11, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 12, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 13, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 14, set_class_field(0, CLASS_TAIL));

	addr = 0;
	r = bytes_item_head(bytes, 10, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	addr = 0;
	r = bytes_item_head(bytes, 14, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_item_head_code(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	set_byte_fields(bytes, 10, set_class_field(0, CLASS_CODE));
	set_byte_fields(bytes, 11, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 12, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 13, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 14, set_class_field(0, CLASS_TAIL));

	addr = 0;
	r = bytes_item_head(bytes, 10, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	addr = 0;
	r = bytes_item_head(bytes, 14, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_item_end(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	set_byte_fields(bytes, 10, set_class_field(0, CLASS_DATA));
	set_byte_fields(bytes, 11, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 12, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 13, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 14, set_class_field(0, CLASS_TAIL));

	addr = 0;
	r = bytes_item_end(bytes, 10, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	addr = 0;
	r = bytes_item_end(bytes, 14, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	set_byte_fields(bytes, 15, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 16, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 17, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 18, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 19, set_class_field(0, CLASS_TAIL));
	
	addr = 0;
	r = bytes_item_end(bytes, 10, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 19);
	
	addr = 0;
	r = bytes_item_end(bytes, 19, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 19);

	free_bytes(bytes);
	return 0;
}


static int test_bytes_first_not_tail(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = 0;
	r |= enable_bytes(bytes, 10, 19);
	r |= enable_bytes(bytes, 30, 39);
	r |= bytes_set_byte_class(bytes, 10, CLASS_DATA);
	r |= bytes_set_byte_class(bytes, 11, CLASS_UNKNOWN);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_first_not_tail(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_last_not_tail(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = 0;
	r |= enable_bytes(bytes, 10, 19);
	r |= enable_bytes(bytes, 70, 79);
	r |= bytes_set_byte_class(bytes, 79, CLASS_UNKNOWN);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_last_not_tail(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 79);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_next_not_tail(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	addr = 0;
	r = 0;
	r |= enable_bytes(bytes, 10, 19);
	r |= enable_bytes(bytes, 30, 39);
	r |= enable_bytes(bytes, 50, 59);
	r |= bytes_set_byte_class(bytes, 10, CLASS_DATA);
	r |= bytes_set_range_class(bytes, 11, 19, CLASS_TAIL);
	r |= bytes_set_range_class(bytes, 30, 39, CLASS_UNKNOWN);
	r |= bytes_set_byte_class(bytes, 52, CLASS_CODE);
	r |= bytes_set_range_class(bytes, 53, 59, CLASS_TAIL);
	r |= bytes_first_not_tail(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 30);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 31);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 32);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 33);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 34);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 35);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 36);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 37);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 38);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 39);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 50);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 51);
	r = bytes_next_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 52);
	dis_errno = 0;
	r = bytes_next_not_tail(bytes, addr, &addr);
	FAIL_IF(!r);
	FAIL_IF(dis_errno != DER_NOTFOUND);

	free_bytes(bytes);
	return 0;
}

static int test_bytes_prev_not_tail(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	addr = 0;
	r = 0;
	r |= enable_bytes(bytes, 10, 19);
	r |= enable_bytes(bytes, 30, 39);
	r |= enable_bytes(bytes, 50, 59);
	r |= bytes_set_byte_class(bytes, 10, CLASS_DATA);
	r |= bytes_set_range_class(bytes, 11, 19, CLASS_TAIL);
	r |= bytes_set_range_class(bytes, 30, 39, CLASS_UNKNOWN);
	r |= bytes_set_byte_class(bytes, 52, CLASS_CODE);
	r |= bytes_set_range_class(bytes, 53, 59, CLASS_TAIL);
	r |= bytes_last_not_tail(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 52);

	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 51);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 50);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 39);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 38);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 37);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 36);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 35);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 34);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 33);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 32);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 31);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 30);
	r = bytes_prev_not_tail(bytes, addr, &addr); FAIL_IF_ERR(r); FAIL_IF(addr != 10);
	dis_errno = 0;
	r = bytes_prev_not_tail(bytes, addr, &addr);
	FAIL_IF(!r);
	FAIL_IF(dis_errno != DER_NOTFOUND);

	free_bytes(bytes);
	return 0;
}

static int test_create_code_item(void){
	struct bytes *bytes;
	uint64_t addr;
	uint32_t class;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = 0;
	r |= enable_bytes(bytes, 10, 19);
	r |= enable_bytes(bytes, 30, 39);
	r |= enable_bytes(bytes, 50, 59);
	FAIL_IF_ERR(r);

	r = bytes_create_code_item(bytes, 34, 37);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_first_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 34);
	r = bytes_get_byte_class(bytes, addr, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_CODE);
	r = bytes_next_not_tail(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 38);

	free_bytes(bytes);
	return 0;
}

static int test_create_data_item_byte(void){
	struct bytes *bytes;
	uint64_t addr;
	uint32_t class;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = 0;
	r |= enable_bytes(bytes, 10, 19);
	r |= enable_bytes(bytes, 30, 39);
	r |= enable_bytes(bytes, 50, 59);
	FAIL_IF_ERR(r);

	r = bytes_create_data_item_byte(bytes, 34);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_first_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 34);
	r = bytes_get_byte_class(bytes, addr, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);
	r = bytes_next_not_tail(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 35);

	free_bytes(bytes);
	return 0;
}


static int test_create_data_item_word(void){
	struct bytes *bytes;
	uint64_t addr;
	uint32_t class;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = 0;
	r |= enable_bytes(bytes, 10, 19);
	r |= enable_bytes(bytes, 30, 39);
	r |= enable_bytes(bytes, 50, 59);
	FAIL_IF_ERR(r);

	r = bytes_create_data_item_word(bytes, 34);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_first_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 34);
	r = bytes_get_byte_class(bytes, addr, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);
	r = bytes_next_not_tail(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 36);

	free_bytes(bytes);
	return 0;
}


static int test_create_data_item_dword(void){
	struct bytes *bytes;
	uint64_t addr;
	uint32_t class;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = 0;
	r |= enable_bytes(bytes, 10, 19);
	r |= enable_bytes(bytes, 30, 39);
	r |= enable_bytes(bytes, 50, 59);
	FAIL_IF_ERR(r);

	r = bytes_create_data_item_dword(bytes, 34);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_first_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 34);
	r = bytes_get_byte_class(bytes, addr, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);
	r = bytes_next_not_tail(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 38);

	free_bytes(bytes);
	return 0;
}


static int test_create_data_item_qword(void){
	struct bytes *bytes;
	uint64_t addr;
	uint32_t class;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = 0;
	r |= enable_bytes(bytes, 10, 19);
	r |= enable_bytes(bytes, 30, 39);
	r |= enable_bytes(bytes, 50, 59);
	FAIL_IF_ERR(r);

	r = bytes_create_data_item_qword(bytes, 31);
	FAIL_IF_ERR(r);

	addr = 0;
	r = bytes_first_item(bytes, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 31);
	r = bytes_get_byte_class(bytes, addr, &class);
	FAIL_IF_ERR(r);
	FAIL_IF(class != CLASS_DATA);
	r = bytes_next_not_tail(bytes, addr, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 39);

	free_bytes(bytes);
	return 0;
}


static struct test tests[] = {
	{"newfree", test_newfree},
	{"enable_bytes", test_enable_bytes},
	{"enable_bytes-forward", test_enable_bytes_forward},
	{"enable_bytes-reverse", test_enable_bytes_reverse},
	{"enable_bytes-middle", test_enable_bytes_middle},
	{"enable_bytes-expand_up", test_enable_bytes_expand_up},
	{"enable_bytes-expand_down", test_enable_bytes_expand_down},
	{"enable_bytes-merge", test_enable_bytes_merge},
	{"get_byte_fields", test_get_byte_fields},
	{"set_byte_fields", test_set_byte_fields},
	/* Value */
	{"copy_from_bytes", test_copy_from_bytes},
	{"bytes_get_byte", test_bytes_get_byte},
	{"bytes_get_word", test_bytes_get_word},
	{"bytes_get_dword", test_bytes_get_dword},
	{"bytes_get_qword", test_bytes_get_qword},
	{"copy_to_bytes", test_copy_to_bytes},
	{"set_bytes", test_set_bytes},
	{"bytes_put_byte", test_bytes_put_byte},
	{"bytes_put_word", test_bytes_put_word},
	{"bytes_put_dword", test_bytes_put_dword},
	{"bytes_put_qword", test_bytes_put_qword},
	/* Class */
	{"bytes_get_byte_class", test_bytes_get_byte_class},
	{"bytes_set_byte_class", test_bytes_set_byte_class},
	{"bytes_set_range_class", test_bytes_set_range_class},
	/* Datatype */
	{"get_bytes_datatype",test_get_bytes_datatype},
	{"set_bytes_datatype", test_set_bytes_datatype},
	/* Items */
	{"bytes_first_addr", test_bytes_first_addr},
	{"bytes_last_addr", test_bytes_last_addr},
	{"bytes_next_addr", test_bytes_next_addr},
	{"bytes_prev_addr", test_bytes_prev_addr},
	{"bytes_first_item", test_bytes_first_item},
	{"bytes_last_item", test_bytes_last_item},
	{"bytes_next_item", test_bytes_next_item},
	{"bytes_prev_item", test_bytes_prev_item},
	{"bytes_item_head-unknown", test_bytes_item_head_unknown},
	{"bytes_item_head-data", test_bytes_item_head_data},
	{"bytes_item_head-code", test_bytes_item_head_code},
	{"bytes_item_end", test_bytes_item_end},
	{"bytes_first_not_tail", test_bytes_first_not_tail},
	{"bytes_last_not_tail", test_bytes_last_not_tail},
	{"bytes_next_not_tail", test_bytes_next_not_tail},
	{"bytes_prev_not_tail", test_bytes_prev_not_tail},

	{"bytes_create_code_item", test_create_code_item},
	{"bytes_create_data_item_byte", test_create_data_item_byte},
	{"bytes_create_data_item_word", test_create_data_item_word},
	{"bytes_create_data_item_dword", test_create_data_item_dword},
	{"bytes_create_data_item_qword", test_create_data_item_qword},

	{NULL, NULL},
};

void test_bytes_init(void) __attribute__ ((constructor));
void test_bytes_init(void){
	add_module_tests("bytes", tests);
}
