#include <stdio.h>
#include <assert.h>
#define  __STDC_FORMAT_MACROS
#include <inttypes.h>

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

static struct test tests[] = {
	{"newfree", test_newfree},
	{"enable_bytes", test_enable_bytes},
	{"enable_bytes-forward", test_enable_bytes_forward},
	{"enable_bytes-reverse", test_enable_bytes_reverse},
	{"enable_bytes-middle", test_enable_bytes_middle},
	{"enable_bytes-expand_up", test_enable_bytes_expand_up},
	{"enable_bytes-expand_down", test_enable_bytes_expand_down},
	{"enable_bytes-merge", test_enable_bytes_merge},
	{NULL, NULL},
};

void test_bytes_init(void) __attribute__ ((constructor));
void test_bytes_init(void){
	add_module_tests("bytes", tests);
}
