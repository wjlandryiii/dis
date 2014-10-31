/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>

#include "bytes.h"




int create_bytes(char *filename, uint32_t base_addr, struct bytes *bytes){
	FILE *f;
	long fsize;

	uint32_t addr;
	uint32_t count;
	uint32_t *byte_flags;
	uint32_t flags;
	int i;
	int c;

	f = fopen(filename, "r");
	if(f == NULL){
		fprintf(stderr, "fopen(\"%s\")\n", filename);
		goto fail;
	}

	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	if(fsize <= 0){
		fprintf(stderr, "couldn't find file size\n");
		goto fail;
	}
	
	addr = base_addr;
	count = fsize;
	byte_flags = calloc(sizeof(*byte_flags), count);

	for(i = 0; i < count; i++){
		c = fgetc(f);
		if(c == EOF){
			fprintf(stderr, "unexpected end of file\n");
			goto fail;
		}
		flags = 0;
		flags = set_byte_value(flags, c);
		flags = set_has_value(flags);
		byte_flags[i] = flags;
	}
	fclose(f);

	if(bytes){
		bytes->addr = addr;
		bytes->count = count;
		bytes->byte_flags = byte_flags;
	} else {
		free(byte_flags);
	}
		
	return 0;
fail:
	return -1;
}


int load_bytes(char *filename, struct bytes *bytes){
	FILE *f;
	uint32_t addr;
	uint32_t count;
	uint32_t *byte_flags;
	size_t r;

	f = fopen(filename, "r");
	if(f == NULL){
		fprintf(stderr, "fopen(\"%s\")\n", filename);
		goto fail;
	}

	r = fread(&addr, sizeof(addr), 1, f);
	if(r != 1){
		fprintf(stderr, "fread()\n");
		goto fail;
	}

	r = fread(&count, sizeof(count), 1, f);
	if(r != 1){
		fprintf(stderr, "fread()\n");
		goto fail;
	}
	byte_flags = calloc(count, sizeof(uint32_t));
	if(byte_flags == NULL){
		fprintf(stderr, "callod()\n");
		goto fail;
	}

	r = fread(byte_flags, sizeof(uint32_t), count, f);
	if(r != count){
		fprintf(stderr, "fread()\n");
	}
	fclose(f);

	if(bytes){
		bytes->addr = addr;
		bytes->count = count;
		bytes->byte_flags = byte_flags;
	} else {
		free(byte_flags);
	}
	return 0;

fail:
	return -1;
}

int save_bytes(char *filename, struct bytes *bytes){
	FILE *f;
	size_t r;

	f = fopen(filename, "w+");
	if(f == NULL){
		fprintf(stderr, "fopen(\"%s\")\n", filename);
		goto fail;
	}

	r = fwrite(&bytes->addr, sizeof(bytes->addr), 1, f);
	if(r != 1){
		fprintf(stderr, "fwrite()\n");
		goto fail;
	}
	r = fwrite(&bytes->count, sizeof(bytes->count), 1, f);
	if(r != 1){
		fprintf(stderr, "fwrite()\n");
		goto fail;
	}
	r = fwrite(bytes->byte_flags, sizeof(*(bytes->byte_flags)),
			bytes->count, f);
	if(r != bytes->count){
		fprintf(stderr, "fwrite()\n");
		goto fail;
	}
	fclose(f);
	return 0;

fail:
	return -1;
}
