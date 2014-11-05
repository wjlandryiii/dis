/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "workspace.h"
#include "bytes.h"
#include "loader.h"
#include "testrunner.h"
#include "elf.h"

const char *linux_raw_filename = "data/helloworld.bin";
const char *linux_elf32_filename = "data/helloworld.exe";


int test_can_load_file_raw(void){
	FILE *f;
	int can;

	f = fopen(linux_raw_filename, "r");
	FAIL_IF(f == NULL);
	can = can_load_file_raw(f);
	fclose(f);
	FAIL_IF(!can);
	return 0;
}


int test_can_load_file_elf_pass(void){
	FILE *f;
	int can;

	f = fopen(linux_elf32_filename, "r");
	FAIL_IF(f == NULL);
	can = can_load_file_elf(f);
	fclose(f);
	FAIL_IF(!can);
	return 0;
}

int test_can_load_file_elf_fail(void){
	FILE *f;
	int can;

	f = fopen(linux_raw_filename, "r");
	FAIL_IF(f == NULL);
	can = can_load_file_elf(f);
	fclose(f);
	FAIL_IF(can);
	return 0;
}

int test_load_file_raw(void){
	FILE *f;
	struct workspace *ws;
	int r;
	uint8_t buf[4] ;

	ws = new_workspace();
	FAIL_IF(!ws);

	f = fopen(linux_raw_filename, "r");
	r = load_file_raw(ws, f);
	fclose(f);
	FAIL_IF(r);
	FAIL_IF(copy_from_bytes(ws->ws_bytes, 0x10000, buf, 4));
	FAIL_IF(buf[0] != 0xeb);
	FAIL_IF(buf[1] != 0x31);
	FAIL_IF(buf[2] != 0x5e);
	FAIL_IF(buf[3] != 0x89);
	return 0;
}

int test_load_file_elf(void){
	FILE *f;
	struct workspace *ws;
	int r;
	uint8_t buf[4] ;

	ws = new_workspace();
	FAIL_IF(!ws);

	f = fopen(linux_elf32_filename, "r");
	r = load_file_elf(ws, f);
	fclose(f);
	FAIL_IF(r);
	FAIL_IF(copy_from_bytes(ws->ws_bytes, 0x08048000, buf, 4));
	FAIL_IF(buf[0] != ELFMAG0);
	FAIL_IF(buf[1] != ELFMAG1);
	FAIL_IF(buf[2] != ELFMAG2);
	FAIL_IF(buf[3] != ELFMAG3);
	return 0;
}

struct test tests[] = {
	{"can_load_file_raw", test_can_load_file_raw},
	{"can_load_file_elf-pass", test_can_load_file_elf_pass},
	{"can_load_file_elf-fail", test_can_load_file_elf_fail},
	{"load_file_raw", test_load_file_raw},
	{"load_file_elf", test_load_file_elf},
	{NULL,NULL},
};

int main(int argc, char *argv[]){
	return run_tests(argc, argv, tests);
}
