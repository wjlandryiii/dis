/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>

#include "workspace.h"
#include "loader.h"

void dump_disassembly(struct workspace *ws){



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
		printf("%s\n", compat_loaders[i]->ld_name);
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
