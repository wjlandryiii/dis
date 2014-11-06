#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "bytes.h"

int
instruction_line(struct bytes *bytes, int i, char *buf, size_t size){
	int r;
	int next_i;
	uint32_t flags;

	flags = bytes->byte_flags[i];
	r = snprintf(buf, size, "INST %02x", get_byte_value(flags));
	if(r > size - 1){
		next_i = i + 1;
		goto truncate;
	} else if(r < 0){
		goto fail;
	}
	size -= r;
	buf += r;
	
	for(next_i = i + 1; next_i < bytes->count; next_i++){
		flags = bytes->byte_flags[next_i];
		if(!is_tail(flags)){
			break;
		} else {
			r = snprintf(buf,size, " %02x", get_byte_value(flags));
			if(r > size - 1){
				next_i++;
				goto truncate;
			} else if(r < 0){
				goto fail;
			}
			size -= r;
			buf += r;
		}
	}

	return next_i;

truncate:
	buf[size-1] = '.';
	buf[size-2] = '.';
	buf[size-3] = '.';
	return next_i;

fail:
	return -1;
}

int
data_line(struct bytes *bytes, int i, char *buf, size_t size){
	uint32_t flags;
	int next_i;
	int r;

	flags = bytes->byte_flags[i];
	r = snprintf(buf, size, "DATA %02x", get_byte_value(flags));
	for(next_i = i+1; next_i < bytes->count; next_i++){
		flags = bytes->byte_flags[next_i];
		if(!is_tail(flags)){
			break;
		} else {
			snprintf(buf, sizeof(buf), " %02x",
					get_byte_value(flags));
		}
	}
	return next_i;

truncate:
	buf[size-1] = '.';
	buf[size-2] = '.';
	buf[size-3] = '.';
	return next_i;

fail:
	return -1;
}


int printdb(struct bytes *bytes){
	uint32_t i;
	uint32_t j;
	uint32_t flags;
	char line[1024];
	uint32_t next_i;

	for(i = 0; i < bytes->count; i = next_i){
		flags = bytes->byte_flags[i];
		if(is_code(flags)){
			next_i = instruction_line(bytes, i, line, sizeof(line));
			if(next_i < 0){
				fprintf(stderr, "instruction_line()\n");
				goto fail;
			}
		} else if(is_data(flags)){
			next_i = data_line(bytes, i, line, sizeof(line));
			if(next_i < 0){
				fprintf(stderr, "data_line()\n");
				goto fail;
			}
		} else if(is_unknown(flags)){
			snprintf(line, sizeof(line), "db 0x%02x", flags & 0xFF);
			next_i = i + 1;
		} else {
			fprintf(stderr, "programmer error\n");
			goto fail;
		}

		printf("%08x %08x:      %s\n", i+bytes->addr, flags, line);
/*
		for(j = i+1; j < next_i; j++){
			printf("%08x %08x:\n",
					j+bytes->addr,  bytes->byte_flags[j]);
		}
*/
	}
	return 0;
fail:
	return -1;
}


int main(int argc, char *argv[]){
	char *filename;
	struct bytes bytes;
	int r;

	if(argc < 2){
		fprintf(stderr, "%s: [database file]\n", argv[0]);
		goto fail;
	} else {
		filename = argv[1];

		r = load_bytes(filename, &bytes);
		if(r){
			fprintf(stderr, "load_bytes()\n");
			goto fail;
		}

		printdb(&bytes);
		return 0;
	}

fail:
	return 1;
}
