/*
 * Copyright 2014 Joseph Landry
 */


#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <stdint.h>

struct disassembler;

struct disassembler *
new_disassembler(int machine);

void
free_disassembler(struct disassembler *dis);

int
disassemble_instruction(struct disassembler *dis, uint8_t *code, size_t size,
		uint64_t addr, char *line, size_t line_size,
		size_t *inst_size_out);
#endif
