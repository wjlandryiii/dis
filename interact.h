/*
 * Copyright 2014 Joseph Landry
 */

#ifndef INTERACT_H
#define INTERACT_H

extern int interact_done;

struct command {
	char *c_name;
	int (*c_fn)(int argc, char *argv[]);
	char *c_description;	
};

int
interact(const struct command *cmds, char *prompt);

#endif
