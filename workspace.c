/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "workspace.h"
#include "bytes.h"

struct workspace *
new_workspace(){
	struct workspace *ws;

	ws = calloc(1, sizeof(*ws));
	if(ws){
		ws->ws_bytes = new_bytes();
	}
	return ws;
}

void
free_workspace(struct workspace *ws){
	if(ws->ws_bytes){
		free_bytes(ws->ws_bytes);
	}
	free(ws);
}

int initialize_machine(struct workspace *ws, int machine){
	ws->ws_machine = machine;
	return 0;
}
