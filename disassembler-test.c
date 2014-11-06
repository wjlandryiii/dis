/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disassembler.h"
#include "testrunner.h"

static int
test_newfree(void){
	struct disassembler *dis;

	dis = new_disassembler(0);
	FAIL_IF(dis == NULL);
	free_disassembler(dis);
	return 0;
}

static int
test_disassemble_instruction_x86(void){
	struct disassembler *dis;
	uint8_t inst[8];
	char line[512];
	size_t inst_size;

	dis = new_disassembler(0);
	FAIL_IF(dis == NULL);

	inst[0] = 0x90;
	disassemble_instruction(dis, inst, sizeof(inst), 0x100,
			line, sizeof(line),
			&inst_size);
	FAIL_IF(inst_size != 1);
	FAIL_IF(strncmp(line, "nop", 3) != 0);
	free_disassembler(dis);
	return 0;
}

static struct test tests[] = {
	{"newfree", test_newfree},
	{"disassemble_instruction_x86", test_disassemble_instruction_x86},
	{NULL, NULL},
};

void test_disassembler_init(void) __attribute__ ((constructor));
void test_disassembler_init(void){
	add_module_tests("disassembler", tests);
}
