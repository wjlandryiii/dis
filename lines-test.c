/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "workspace.h"
#include "bytes.h"
#include "lines.h"

#include "testrunner.h"

struct workspace *blank_workspace(void){
	struct workspace *ws;

	ws = new_workspace();
	if(ws){
		initialize_machine(ws, MACHINE_i386);
	}
	return ws;
}


static int test_line_for_unknown_invalid(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = enable_bytes(ws->ws_bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = line_for_unknown(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }
	FAIL_IF(strcmp(line, "db               ??") != 0);
	free_workspace(ws);
	return 0;
}


static int test_line_for_unknown_valid(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = enable_bytes(ws->ws_bytes, 10, 19);
	r = bytes_put_byte(ws->ws_bytes, 10, 0x90);
	FAIL_IF_ERR(r);

	r = line_for_unknown(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }
	FAIL_IF(strcmp(line, "db               0x90") != 0);
	free_workspace(ws);
	return 0;
}

static int test_line_for_code_item(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = 0;
	r |= enable_bytes(ws->ws_bytes, 10, 19);
	r |= copy_to_bytes(ws->ws_bytes, 10, (uint8_t *) "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19", 10);
	r |= bytes_create_code_item(ws->ws_bytes, 10, 19);
	FAIL_IF_ERR(r);

	r = line_for_code_item(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }

	FAIL_IF(strcmp(line, "INST             10 11 12 13 14 15 16 17 18 19") != 0);

	free_workspace(ws);
	return 0;
}


static int test_line_for_data_bytes_invalid(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = 0;
	r |= enable_bytes(ws->ws_bytes, 10, 19);
	r |= bytes_create_data_item_byte(ws->ws_bytes, 10);
	FAIL_IF_ERR(r);

	r = line_for_data_byte(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }

	FAIL_IF(strcmp(line, "db               ??") != 0);
	free_workspace(ws);
	return 0;
}
static int test_line_for_data_bytes_valid(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = 0;
	r |= enable_bytes(ws->ws_bytes, 10, 19);
	r |= bytes_put_byte(ws->ws_bytes, 10, 0x90);
	r |= bytes_create_data_item_byte(ws->ws_bytes, 10);
	FAIL_IF_ERR(r);

	r = line_for_data_byte(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }
	FAIL_IF(strcmp(line, "db               0x90") != 0);
	free_workspace(ws);
	return 0;
}
static int test_line_for_data_word_invalid(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = 0;
	r |= enable_bytes(ws->ws_bytes, 10, 19);
	r |= bytes_create_data_item_word(ws->ws_bytes, 10);
	FAIL_IF_ERR(r);

	r = line_for_data_word(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }

	FAIL_IF(strcmp(line, "dw               ??") != 0);
	free_workspace(ws);
	return 0;
}
static int test_line_for_data_word_valid(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = 0;
	r |= enable_bytes(ws->ws_bytes, 10, 19);
	r |= bytes_put_word(ws->ws_bytes, 10, 0x1234);
	r |= bytes_create_data_item_word(ws->ws_bytes, 10);
	FAIL_IF_ERR(r);

	r = line_for_data_word(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }
	FAIL_IF(strcmp(line, "dw               0x1234") != 0);
	free_workspace(ws);
	return 0;
}



static int test_line_for_data_dword_invalid(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = 0;
	r |= enable_bytes(ws->ws_bytes, 10, 19);
	r |= bytes_create_data_item_dword(ws->ws_bytes, 10);
	FAIL_IF_ERR(r);

	r = line_for_data_dword(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }

	FAIL_IF(strcmp(line, "dd               ??") != 0);
	free_workspace(ws);
	return 0;
}

static int test_line_for_data_dword_valid(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = 0;
	r |= enable_bytes(ws->ws_bytes, 10, 19);
	r |= bytes_put_dword(ws->ws_bytes, 10, 0x12345678);
	r |= bytes_create_data_item_dword(ws->ws_bytes, 10);
	FAIL_IF_ERR(r);

	r = line_for_data_dword(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }
	FAIL_IF(strcmp(line, "dd               0x12345678") != 0);
	free_workspace(ws);
	return 0;
}


static int test_line_for_data_qword_invalid(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = 0;
	r |= enable_bytes(ws->ws_bytes, 10, 19);
	r |= bytes_create_data_item_qword(ws->ws_bytes, 10);
	FAIL_IF_ERR(r);

	r = line_for_data_qword(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }

	FAIL_IF(strcmp(line, "dq               ??") != 0);
	free_workspace(ws);
	return 0;
}

static int test_line_for_data_qword_valid(void){
	struct workspace *ws;
	register int r;
	char line[1024];

	ws = blank_workspace();
	FAIL_IF(ws == NULL);

	r = 0;
	r |= enable_bytes(ws->ws_bytes, 10, 19);
	r |= bytes_put_qword(ws->ws_bytes, 10, 0x0123456789ABCDEF);
	r |= bytes_create_data_item_qword(ws->ws_bytes, 10);
	FAIL_IF_ERR(r);

	r = line_for_data_qword(ws, 10, line, sizeof(line));
	if(r < 0){ FAIL_IF_ERR(r); }
	FAIL_IF(strcmp(line, "dq               0x0123456789abcdef") != 0);
	free_workspace(ws);
	return 0;
}


static struct test tests[] = {
	{"line_for_unknown-invalid", test_line_for_unknown_invalid},
	{"line_for_unknown-valid", test_line_for_unknown_valid},
	{"line_for_code_item", test_line_for_code_item},
	{"line_for_data_byte-invalid", test_line_for_data_bytes_invalid},
	{"line_for_data_byte-valid", test_line_for_data_bytes_valid},
	{"line_for_data_word-invalid", test_line_for_data_word_invalid},
	{"line_for_data_word-valid", test_line_for_data_word_valid},
	{"line_for_data_dword-invalid", test_line_for_data_dword_invalid},
	{"line_for_data_dword-valid", test_line_for_data_dword_valid},
	{"line_for_data_qword-invalid", test_line_for_data_qword_invalid},
	{"line_for_data_qword-valid", test_line_for_data_qword_valid},
	{NULL, NULL},
};

void test_lines_init(void) __attribute__ ((constructor));
void test_lines_init(void){
	add_module_tests("lines", tests);
}
