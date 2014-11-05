/*
 * Copyright 2014 Joseph Landry
 */

#ifndef WORKSPACE_H
#define WORKSPACE_H

enum MACHINE {
	MACHINE_INVALID = 0,
	MACHINE_i386,
};

struct bytes;

struct workspace {
	int ws_machine;
	struct bytes *ws_bytes;
};

struct workspace *new_workspace(void);
void free_workspace(struct workspace *ws);
int initialize_machine(struct workspace *ws, int machine);

#endif
