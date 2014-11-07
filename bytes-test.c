#include <stdio.h>
#include <string.h>

#include "bytes.h"
#include "testrunner.h"

static int
test_newfree(void){
	struct bytes *bytes;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	free_bytes(bytes);
	return 0;
}

int
test_enable_bytes(void){
	struct bytes *bytes;
	struct bytechunk *chunk;
	int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 100, 200);
	FAIL_IF(r != 0);

	chunk = first_chunk(bytes);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 100);
	FAIL_IF(chunk->bc_last != 200);
	FAIL_IF(chunk->bc_bytes == NULL);
	
	free_bytes(bytes);
	return 0;
}

int
test_enable_bytes_forward(){
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
	
	c = first_chunk(bytes);
	for(i = 0; i < 5; i++){
		first = i*5;
		last = first + 2;

		FAIL_IF(c == NULL);
		FAIL_IF(c->bc_first != first);
		FAIL_IF(c->bc_last != last);
		c = next_chunk(c);
	}
	FAIL_IF(c != NULL);
	free_bytes(bytes);
	return 0;
}

int test_enable_bytes_reverse(){
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

	c = first_chunk(bytes);
	for(i = 0; i < 5; i++){
		first = i*5;
		last = first+2;

		FAIL_IF(c == NULL);
		FAIL_IF(c->bc_first != first);
		FAIL_IF(c->bc_last != last);
		c = next_chunk(c);
	}
	FAIL_IF(c != NULL);
	free_bytes(bytes);
	return 0;
}

static int
test_enable_bytes_middle(void){
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
	
	c = first_chunk(bytes);
	for(i = 0; i < 10; i++){
		first = i*5;
		last = first+2;

		FAIL_IF(c == NULL);
		FAIL_IF(c->bc_first != first);
		FAIL_IF(c->bc_last != last);
		c = next_chunk(c);
	}
	FAIL_IF(c != NULL);
	free_bytes(bytes);
	return 0;
}


static int
test_enable_bytes_expand_up(void){
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

	chunk = first_chunk(bytes);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 0);
	FAIL_IF(chunk->bc_last != 49);
	chunk = next_chunk(chunk);
	FAIL_IF(chunk != NULL);
	free_bytes(bytes);
	return 0;
}

static int
test_enable_bytes_expand_down(void){
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

	chunk = first_chunk(bytes);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 0);
	FAIL_IF(chunk->bc_last != 49);
	chunk = next_chunk(chunk);
	FAIL_IF(chunk != NULL);
	free_bytes(bytes);
	return 0;
}

static int
test_enable_bytes_merge(void){
	struct bytes *bytes;
	struct bytechunk *chunk;
	uint64_t first;
	uint64_t last;
	int i;

	bytes = new_bytes();

	for(i = 0; i < 10; i++){
		first = i < 5 ? i*5*2 : (i-5)*5*2+5;
		last = first + 4;
		enable_bytes(bytes, first, last);
	}
	
	chunk = first_chunk(bytes);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 0);
	FAIL_IF(chunk->bc_last != 49);
	chunk = next_chunk(chunk);
	FAIL_IF(chunk != NULL);
	free_bytes(bytes);
	return 0;
}

static int
test_get_byte_fields(void){
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

	chunk = first_chunk(bytes);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 0);
	FAIL_IF(chunk->bc_last != 9);
	chunk->bc_bytes[0] = 0x11111111;
	chunk->bc_bytes[4] = 0x22222222;
	chunk->bc_bytes[9] = 0x33333333;

	chunk = next_chunk(chunk);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 20);
	FAIL_IF(chunk->bc_last != 29);
	chunk->bc_bytes[0] = 0x44444444;
	chunk->bc_bytes[4] = 0x55555555;
	chunk->bc_bytes[9] = 0x66666666;

	chunk = next_chunk(chunk);
	FAIL_IF(chunk == NULL);
	FAIL_IF(chunk->bc_first != 40);
	FAIL_IF(chunk->bc_last != 49);
	chunk->bc_bytes[0] = 0x77777777;
	chunk->bc_bytes[4] = 0x88888888;
	chunk->bc_bytes[9] = 0x99999999;

	chunk = next_chunk(chunk);
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


static int
test_set_byte_fields(void){
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

static int
test_copy_from_bytes(void){
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


static int
test_bytes_get_byte(void){
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


static int
test_bytes_get_word(void){
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

static int
test_bytes_get_dword(void){
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

static int
test_bytes_get_qword(void){
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

static int
test_set_bytes(void){
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

static int
test_bytes_put_byte(void){
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

static int
test_bytes_put_word(void){
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

static int
test_bytes_put_dword(void){
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

static int
test_bytes_put_qword(void){
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


static int
test_bytes_get_byte_class(void){
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

static int
test_item_head_unknown(void){
	struct bytes *bytes;
	uint64_t addr;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	addr = 0;
	r = item_head(bytes, 10, &addr);
	FAIL_IF(r == 0);

	addr = 0;
	r = item_head(bytes, 14, &addr);
	FAIL_IF(r == 0);

	free_bytes(bytes);
	return 0;
}

static int
test_item_head_data(void){
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
	r = item_head(bytes, 10, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	addr = 0;
	r = item_head(bytes, 14, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	free_bytes(bytes);
	return 0;
}

static int
test_item_head_code(void){
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
	r = item_head(bytes, 10, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	addr = 0;
	r = item_head(bytes, 14, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 10);

	free_bytes(bytes);
	return 0;
}

static int
test_item_end(void){
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
	r = item_end(bytes, 10, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	addr = 0;
	r = item_end(bytes, 14, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 14);

	set_byte_fields(bytes, 15, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 16, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 17, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 18, set_class_field(0, CLASS_TAIL));
	set_byte_fields(bytes, 19, set_class_field(0, CLASS_TAIL));
	
	addr = 0;
	r = item_end(bytes, 10, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 19);
	
	addr = 0;
	r = item_end(bytes, 19, &addr);
	FAIL_IF_ERR(r);
	FAIL_IF(addr != 19);

	free_bytes(bytes);
	return 0;
}

/*
static int
test_set_class_code(void){
	struct bytes *bytes;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = set_class_code(bytes, 10, 15);

	return -1;



	free_bytes(bytes);
	return 0;
}
*/

static int
test_get_bytes_datatype(void){
	struct bytes *bytes;
	uint32_t datatype;
	uint32_t fields;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);

	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	/* NOT DATA */
	datatype = 0;
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF(r == 0);

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
	r |= set_byte_fields(bytes, 11, set_class_field(0, CLASS_TAIL));
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
	r |= set_byte_fields(bytes, 11, set_class_field(0, CLASS_TAIL));
	r |= set_byte_fields(bytes, 12, set_class_field(0, CLASS_TAIL));
	r |= set_byte_fields(bytes, 13, set_class_field(0, CLASS_TAIL));
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
	r |= set_byte_fields(bytes, 11, set_class_field(0, CLASS_TAIL));
	r |= set_byte_fields(bytes, 12, set_class_field(0, CLASS_TAIL));
	r |= set_byte_fields(bytes, 13, set_class_field(0, CLASS_TAIL));
	r |= set_byte_fields(bytes, 14, set_class_field(0, CLASS_TAIL));
	r |= set_byte_fields(bytes, 15, set_class_field(0, CLASS_TAIL));
	r |= set_byte_fields(bytes, 16, set_class_field(0, CLASS_TAIL));
	r |= set_byte_fields(bytes, 17, set_class_field(0, CLASS_TAIL));
	FAIL_IF(r != 0);
	
	datatype = 0;
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_QWORD); 

	free_bytes(bytes);
	return 0;
}

static int
test_set_bytes_datatype(void){
	struct bytes *bytes;
	uint32_t datatype;
	register int r;

	bytes = new_bytes();
	FAIL_IF(bytes == NULL);
	r = enable_bytes(bytes, 10, 19);
	FAIL_IF_ERR(r);

	/* BYTE */
	r = set_class_unknown(bytes, 10, 19);
	r = set_bytes_datatype_byte(bytes, 10);
	FAIL_IF_ERR(r);
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_BYTE);

	/* WORD */
	r = set_class_unknown(bytes, 10, 19);
	r = set_bytes_datatype_word(bytes, 10);
	FAIL_IF_ERR(r);
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_WORD);

	/* DWORD */
	r = set_class_unknown(bytes, 10, 19);
	r = set_bytes_datatype_dword(bytes, 10);
	FAIL_IF_ERR(r);
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_DWORD);
	
	/* QWORD */
	r = set_class_unknown(bytes, 10, 19);
	r = set_bytes_datatype_qword(bytes, 10);
	FAIL_IF_ERR(r);
	r = get_bytes_datatype(bytes, 10, &datatype);
	FAIL_IF_ERR(r);
	FAIL_IF(datatype != DATATYPE_QWORD);

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
	{"copy_from_bytes", test_copy_from_bytes},
	{"bytes_get_byte", test_bytes_get_byte},
	{"bytes_get_word", test_bytes_get_word},
	{"bytes_get_dword", test_bytes_get_dword},
	{"bytes_get_qword", test_bytes_get_qword},
	{"set_bytes", test_set_bytes},
	{"bytes_put_byte", test_bytes_put_byte},
	{"bytes_put_word", test_bytes_put_word},
	{"bytes_put_dword", test_bytes_put_dword},
	{"bytes_put_qword", test_bytes_put_qword},
	{"bytes_get_byte_class", test_bytes_get_byte_class},
	{"item_head-unknown", test_item_head_unknown},
	{"item_head-data", test_item_head_data},
	{"item_head-code", test_item_head_code},
	{"item_end", test_item_end},
	/* {"set_class_code", test_set_class_code}, */
	{"get_bytes_datatype",test_get_bytes_datatype},
	{"set_bytes_datatype", test_set_bytes_datatype},
	{NULL, NULL},
};

void test_bytes_init(void) __attribute__ ((constructor));
void test_bytes_init(void){
	add_module_tests("bytes", tests);
}
