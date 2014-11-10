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
#include "loader.h"
#include "bytes.h"
#include "lines.h"

void dump_disassembly(struct workspace *ws){
	uint64_t addr;
	register int r;
	char line[1024];

	r = bytes_first_not_tail(ws->ws_bytes, &addr);
	if(r){ printf("bytes_first_not_tail()\n"); return; }

	while(!r){
		line_for_item(ws, addr, line, sizeof(line));
		printf("%" PRIx64 "    %s\n", addr, line);
		r = bytes_next_not_tail(ws->ws_bytes, addr, &addr);
	}
}


int main(int argc, char *argv[]){
	struct workspace *ws;
	struct loader *compat_loaders[10];
	struct loader *loader;
	int nloaders;
	FILE *f;
	int i;
	register int r;


	ws = new_workspace();

	f = fopen("data/helloworld.exe", "r");

	nloaders = find_compatible_loaders(f, compat_loaders, 10);


	loader = NULL;
	for(i = 0; i < nloaders; i++){
		if(strcmp(compat_loaders[i]->ld_name, "elf") == 0){
			loader = compat_loaders[i];
		}
	}
	if(loader == NULL){
		printf("elf loader was not found\n");
		return 1;
	}

	r = loader_load_file(loader, ws, f);
	if(r){
		printf("loader_load_file()\n");
		return 1;
	}

	r = pre_analysis_elf(ws);
	if(r){
		printf("pre_analysis_elf()\n");
	}

	dump_disassembly(ws);

	fclose(f);
	
	free_workspace(ws);
	return 0;
}
