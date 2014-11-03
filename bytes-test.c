#include <stdio.h>
#include <assert.h>
#define  __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "bytes.h"

int trace = 1;

int
trace_enable_bytes(struct bytes *bytes, uint64_t first, uint64_t last){
	int r;

	if(trace){
		printf("enable_bytes(%p, %3d, %3d)", bytes, first, last);
		fflush(stdout);
	}
	r = enable_bytes(bytes, first, last);
	if(trace){
		printf(": %d\n", r);
	}
	return r;
}

struct bytechunk *
trace_first_chunk(struct bytes *bytes){
	struct bytechunk *chunk;

	if(trace){
		printf("first_chunk(%p)", bytes);
		fflush(stdout);
	}
	chunk = first_chunk(bytes);
	
	if(trace){
		if(chunk != NULL){
			printf(": %p {bc_first: %3d, bc_last: %3d}\n",
					chunk,chunk->bc_first, chunk->bc_last);
		} else {
			printf(": %p\n", chunk);
		}
	}
	return chunk;
}

struct bytechunk *
trace_next_chunk(struct bytechunk *chunk){
	if(trace){
		printf("next_chunk(%p)", chunk);
		fflush(stdout);
	}

	chunk = next_chunk(chunk);

	if(trace){
		if(chunk != NULL){
			printf(": %p {bc_first: %3d, bc_last: %3d}\n",
					chunk,chunk->bc_first, chunk->bc_last);
		} else {
			printf(": %p\n", chunk);
		}
	}
	return chunk;
}



int test_enable(){
	struct bytes *bytes;
	struct bytechunk *c;
	int i;
	uint64_t first;
	uint64_t last;

	bytes = new_bytes();

	for(i = 0; i < 5; i++){
		first = i*5;
		last = first + 2;
		trace_enable_bytes(bytes, first, last);
	}
	
	c = trace_first_chunk(bytes);
	for(i = 0; i < 5; i++){
		first = i*5;
		last = first + 2;

		assert(c != NULL);
		assert(c->bc_first == first);
		assert(c->bc_last == last);
		c = trace_next_chunk(c);
	}
	assert(c == NULL);
	free_bytes(bytes);
	return 0;
}

int test_enable_reverse(){
	struct bytes *bytes;
	struct bytechunk *c;
	int i;
	uint64_t first;
	uint64_t last;

	bytes = new_bytes();

	for(i = 0; i < 5; i++){
		first = 20-i*5;
		last = first+2;
		trace_enable_bytes(bytes, first, last);
	}

	c = trace_first_chunk(bytes);
	for(i = 0; i < 5; i++){
		first = i*5;
		last = first+2;

		assert(c != NULL);
		assert(c->bc_first == first);
		assert(c->bc_last == last);
		c = trace_next_chunk(c);
	}
	assert(c == NULL);
	free_bytes(bytes);
	return 0;
}

int test_enable_middle(){
	struct bytes *bytes;
	struct bytechunk *c;
	int i;
	uint64_t first;
	uint64_t last;

	bytes = new_bytes();

	for(i = 0; i < 10; i++){
		first = i < 5 ? i*5*2 : (i-5)*5*2+5;
		last = first + 2;
		trace_enable_bytes(bytes, first, last);
	}
	
	c = trace_first_chunk(bytes);
	for(i = 0; i < 10; i++){
		first = i*5;
		last = first+2;

		assert(c != NULL);
		assert(c->bc_first == first);
		assert(c->bc_last == last);
		c = trace_next_chunk(c);
	}
	assert(c == NULL);
	free_bytes(bytes);
	return 0;
}


int test_expand_up(void){
	struct bytes *bytes;
	struct bytechunk *chunk;
	uint64_t first;
	uint64_t last;
	int i;

	bytes = new_bytes();

	for(i = 0; i < 10; i++){
		first = i*5;
		last = first + 4;
		trace_enable_bytes(bytes, first, last);
	}

	chunk = trace_first_chunk(bytes);
	assert(chunk != NULL);
	assert(chunk->bc_first == 0);
	assert(chunk->bc_last == 49);
	chunk = trace_next_chunk(chunk);
	assert(chunk == NULL);
	free_bytes(bytes);
	return 0;
}

int test_expand_down(void){
	struct bytes *bytes;
	struct bytechunk *chunk;
	uint64_t first;
	uint64_t last;
	int i;

	bytes = new_bytes();

	for(i = 0; i < 10; i++){
		first = (9*5)-i*5;
		last = first + 4;
		trace_enable_bytes(bytes, first, last);
	}

	chunk = trace_first_chunk(bytes);
	assert(chunk != NULL);
	assert(chunk->bc_first == 0);
	assert(chunk->bc_last == 49);
	chunk = trace_next_chunk(chunk);
	assert(chunk == NULL);
	free_bytes(bytes);
	return 0;
}

int test_merge(){
	struct bytes *bytes;
	struct bytechunk *chunk;
	uint64_t first;
	uint64_t last;
	int i;

	bytes = new_bytes();

	for(i = 0; i < 10; i++){
		first = i < 5 ? i*5*2 : (i-5)*5*2+5;
		last = first + 4;
		trace_enable_bytes(bytes, first, last);
	}
	
	chunk = trace_first_chunk(bytes);
	assert(chunk != NULL);
	assert(chunk->bc_first == 0);
	assert(chunk->bc_last == 49);
	chunk = trace_next_chunk(chunk);
	assert(chunk == NULL);
	free_bytes(bytes);
	return 0;
}

int main(int argc, char *argv[]){
	int ret;

	ret = 0;
	printf("test_enable():\n");
	ret |= test_enable();
	printf("\n\ntest_enable_reverse():\n");
	ret |= test_enable_reverse();
	printf("\n\ntest_enable_middle():\n");
	ret |= test_enable_middle();
	printf("\n\ntest_expand_up():\n");
	ret |= test_expand_up();
	printf("\n\ntest_expand_down():\n");
	ret |= test_expand_down();
	printf("\n\ntest_merge():\n");
	ret |= test_merge();
	return ret;
}
