/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "workspace.h"
#include "loader.h"

#include "elf.h"


int
can_load_file_elf(FILE *f){
	Elf32_Ehdr ehdr;
	unsigned char e_ident[EI_NIDENT] = { 0 };
	int flag;
	int n;

	n = fread(e_ident, sizeof(e_ident), 1, f);
	if(n != 1){
		return 0;
	}

	flag = 0;
	flag |= e_ident[EI_MAG0] != ELFMAG0;
	flag |= e_ident[EI_MAG1] != ELFMAG1;
	flag |= e_ident[EI_MAG2] != ELFMAG2;
	flag |= e_ident[EI_MAG3] != ELFMAG3;
	flag |= e_ident[EI_CLASS] != ELFCLASS32;
	flag |= e_ident[EI_DATA] != ELFDATA2LSB;
	if(flag != 0){
		return 0;
	}

	fseek(f, 0, SEEK_SET);
	n = fread(&ehdr, sizeof(ehdr), 1, f);
	if(n != 1){
		return 0;
	}

	flag = 0;
	flag |= ehdr.e_type != ET_EXEC;
	flag |= ehdr.e_machine != EM_386;
	return flag == 0;
}

int
load_file_elf(struct workspace *ws, FILE *f){
	Elf32_Ehdr ehdr;
	Elf32_Phdr phdr;
	int perms;
	int i;
	int n;

	if(!can_load_file_elf(f)){
		return -1;
	}

	fseek(f, 0, SEEK_SET);

	n = fread(&ehdr, sizeof(ehdr), 1, f);
	if(n != 1){
		return -1;
	}

	for(i = 0; i < ehdr.e_phnum; i++){
		fseek(f, ehdr.e_phoff + (i * ehdr.e_phentsize), SEEK_SET);
		n = fread(&phdr, sizeof(phdr), 1, f);
		if(n != 1){
			return -1;
		}
		if(phdr.p_type == PT_LOAD){
			perms = 0;
			perms |= phdr.p_flags & PF_X;
			perms |= phdr.p_flags & PF_W;
			perms |= phdr.p_flags & PF_R;
			loader_load_segment(ws, f,
					phdr.p_offset, phdr.p_filesz,
					phdr.p_paddr,
					phdr.p_paddr + phdr.p_memsz - 1,
					perms);
		}
	}
	return 0;
}

int
pre_analysis_elf(struct workspace *ws){
	uint64_t base_addr = 0x08048000;
	register int r;
	r = 0;
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_byte(ws->ws_bytes, base_addr++);
	r |= bytes_create_data_item_word(ws->ws_bytes, base_addr); base_addr += 2; // e_type
	r |= bytes_create_data_item_word(ws->ws_bytes, base_addr); base_addr += 2; // e_machine
	r |= bytes_create_data_item_dword(ws->ws_bytes, base_addr); base_addr += 4;// e_version
	r |= bytes_create_data_item_dword(ws->ws_bytes, base_addr); base_addr += 4;// e_entry
	r |= bytes_create_data_item_dword(ws->ws_bytes, base_addr); base_addr += 4;// e_phoff
	r |= bytes_create_data_item_dword(ws->ws_bytes, base_addr); base_addr += 4;// e_shoff
	r |= bytes_create_data_item_dword(ws->ws_bytes, base_addr); base_addr += 4;// e_flags
	r |= bytes_create_data_item_word(ws->ws_bytes, base_addr); base_addr += 2;// e_ehsize
	r |= bytes_create_data_item_word(ws->ws_bytes, base_addr); base_addr += 2;// e_phentsize
	r |= bytes_create_data_item_word(ws->ws_bytes, base_addr); base_addr += 2;// e_phnum
	r |= bytes_create_data_item_word(ws->ws_bytes, base_addr); base_addr += 2;// e_shentsize
	r |= bytes_create_data_item_word(ws->ws_bytes, base_addr); base_addr += 2;// e_shnum
	r |= bytes_create_data_item_word(ws->ws_bytes, base_addr); base_addr += 2;// e_shstrndx
	return r;
}
