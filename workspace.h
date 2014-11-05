/*
 * Copyright 2014 Joseph Landry
 */

#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "bytes.h"

struct workspace {
	struct bytes *ws_bytes;
};

struct workspace *new_workspace(void);
void free_workspace(struct workspace *ws);


#endif
