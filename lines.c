/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "workspace.h"
#include "bytes.h"
#include "bytefields.h"
#include "error.h"

static int line_for_unknown_invalid(char *buf, size_t size){
	return snprintf(buf, size, "%-16s %s", "db", "??");
}

static int line_for_unknown_valid(uint8_t byte, char *buf, size_t size){
	return snprintf(buf, size, "%-16s 0x%02x", "db", (unsigned char)byte);
}

int line_for_unknown(struct workspace *ws, uint64_t addr, char *buf, size_t size){
	uint32_t fields;
	uint8_t byte;
	register int r;

	r = bytes_get_byte_fields(ws->ws_bytes, addr, &fields);
	if(r){ return r; }

	if(is_value_valid(fields)){
		byte = get_byte_value_field(fields);
		return line_for_unknown_valid(byte, buf, size);
	} else {
		return line_for_unknown_invalid(buf, size);
	}
}


int line_for_code_item(struct workspace *ws, uint64_t addr, char *buf, size_t size){
	uint8_t inst_buf[16];
	int inst_nbytes;
	char inst_str[64];
	int size_left;
	uint64_t head;
	uint64_t end;
	register int r;
	int i;

	r = 0;
	r |= bytes_item_head(ws->ws_bytes, addr, &head);
	r |= bytes_item_end(ws->ws_bytes, addr, &end);
	if(r){ return r; }

	inst_nbytes = (end + 1) - head;
	if(inst_nbytes > 16){
		inst_nbytes = 16;
		end = head + 15;
	}

	r = copy_from_bytes(ws->ws_bytes, addr, inst_buf, inst_nbytes);
	if(r){ return r; }

	for(i = 0; i < inst_nbytes; i++){
		size_left = sizeof(inst_str) - (strlen(inst_str) + 1);
		snprintf(&inst_str[i*3], size_left, " %02x", inst_buf[i]);
	}
	return snprintf(buf, size, "%-16s%s", "INST", inst_str);
}

static int line_for_data_byte_invalid(char *buf, size_t size){
	return snprintf(buf, size, "%-16s %s", "db", "??");
}

static int line_for_data_byte_valid(uint8_t byte, char *buf, size_t size){
	return snprintf(buf, size, "%-16s 0x%02x", "db", (unsigned char)byte);
}

int line_for_data_byte(struct workspace *ws, uint64_t addr, char *buf, size_t size){
	uint32_t fields;
	uint8_t byte;
	register int r;

	r = bytes_get_byte_fields(ws->ws_bytes, addr, &fields);
	if(r){ return r; }

	if(is_value_valid(fields)){
		byte = get_byte_value_field(fields);
		return line_for_data_byte_valid(byte, buf, size);
	} else {
		return line_for_data_byte_invalid(buf, size);
	}
}

static int line_for_data_word_invalid(char *buf, size_t size){
	return snprintf(buf, size, "%-16s %s", "dw", "??");
}

static int line_for_data_word_valid(uint16_t word, char *buf, size_t size){
	return snprintf(buf, size, "%-16s 0x%04" PRIx16, "dw", word);
}

int line_for_data_word(struct workspace *ws, uint64_t addr, char *buf, size_t size){
	uint16_t word;
	register int r;

	r = bytes_get_word(ws->ws_bytes, addr, &word);

	if(!r){
		return line_for_data_word_valid(word, buf, size);
	} else if(r && dis_errno == DER_INVVALUE){
		return line_for_data_word_invalid(buf, size);
	} else {
		return r;
	}
}

static int line_for_data_dword_invalid(char *buf, size_t size){
	return snprintf(buf, size, "%-16s %s", "dd", "??");
}

static int line_for_data_dword_valid(uint32_t dword, char *buf, size_t size){
	return snprintf(buf, size, "%-16s 0x%08" PRIx32, "dd", dword);
}

int line_for_data_dword(struct workspace *ws, uint64_t addr, char *buf, size_t size){
	uint32_t dword;
	register int r;

	r = bytes_get_dword(ws->ws_bytes, addr, &dword);

	if(!r){
		return line_for_data_dword_valid(dword, buf, size);
	} else if(r && dis_errno == DER_INVVALUE){
		return line_for_data_dword_invalid(buf, size);
	} else {
		return r;
	}
}

static int line_for_data_qword_invalid(char *buf, size_t size){
	return snprintf(buf, size, "%-16s %s", "dq", "??");
}

static int line_for_data_qword_valid(uint64_t qword, char *buf, size_t size){
	return snprintf(buf, size, "%-16s 0x%016" PRIx64, "dq", qword);
}

int line_for_data_qword(struct workspace *ws, uint64_t addr, char *buf, size_t size){
	uint64_t qword;
	register int r;

	r = bytes_get_qword(ws->ws_bytes, addr, &qword);

	if(!r){
		return line_for_data_qword_valid(qword, buf, size);
	} else if(r && dis_errno == DER_INVVALUE){
		return line_for_data_qword_invalid(buf, size);
	} else {
		return r;
	}
}

int line_for_item(struct workspace *ws, uint64_t itemaddr, char *buf, size_t size){
	return -1;
}
