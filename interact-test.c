/*
 * Copyright Joseph Landry 2014
 */

#include <stdio.h>

#include "interact.h"

struct command commands[];

int help_main(int argc, char *argv[]){
	struct command *cmd;

	cmd = commands;
	while(cmd->c_name){
		printf("%10s: %s\n", cmd->c_name, cmd->c_description);
		cmd++;
	}
	return 0;
}

int echo_main(int argc, char *argv[]){
	int i;

	for(i = 1; i < argc; i++){
		puts(argv[i]);
	}
	return 0;
}

int cat_main(int argc, char *argv[]){
	char buf[80];
	FILE *f;
	int i;

	if(argc < 2){
		printf("cat [file 1] ... [file n]");
	} else {
		for(i = 1; i < argc; i++){
			f = fopen(argv[i], "r");
			if(f == NULL){
				printf("Error opening file: %s\n", argv[i]);
				return 1;
			}
			while(fgets(buf, sizeof(buf), f)){
				fputs(buf, stdout);
			}
			fclose(f);
		}
	}
	return 0;
}

int exit_main(int argc, char *argv[]){
	interact_done = 1;
	return 0;
}


struct command commands[] = {
	{"help", help_main, "show this text"},
	{"echo", echo_main, "print stuff to the screen"},
	{"cat", cat_main, "print files to the screen"},
	{"exit", exit_main, "exit program"},
	{NULL, NULL, NULL},
};

int main(int argc, char *argv[]){
	return interact(commands, "test>");
}
