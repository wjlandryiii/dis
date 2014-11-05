/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <capstone/capstone.h>


struct disassembler {
	csh dis_capstone;
};

struct disassembler *
new_disassembler(int machine){
	struct disassembler *dis;
	int r;

	dis = calloc(1, sizeof(*dis));
	if(dis){
		r = cs_open(CS_ARCH_X86, CS_MODE_32, &dis->dis_capstone);
		if(r){
			free(dis);
			dis = NULL;
		}
	}
	return dis;
}

void
free_disassembler(struct disassembler *dis){
	if(dis->dis_capstone){
		cs_close(&dis->dis_capstone);
	}
	free(dis);
}

int
disassemble_instruction(struct disassembler *dis,
		const uint8_t *code, size_t size,
		uint64_t addr, char *line, size_t line_size,
		size_t *inst_size_out){
	cs_insn insn;
	bool success;

	insn.detail = NULL;
	success = cs_disasm_iter(dis->dis_capstone,
			&code, &size, &addr, &insn);
	if(!success){
		return -1;
	}
	if(line){
		snprintf(line, line_size, "%-16s %s",
			       insn.mnemonic, insn.op_str);
	}
	if(inst_size_out){
		*inst_size_out = insn.size;
	}
	return 0;
}
