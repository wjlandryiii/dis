/*
 * Copyright Joseph Landry 2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "interact.h"


int interact_done = 0;
static struct command *commands;


char *skipwhite(char *s){
	while(isspace(*s)){
		s++;
	}
	return s;
}


static int
count_tokens(char *line){
	int count;

	count = 0;
	while(*line){
		if(isspace(*line)){
			while(*line && isspace(*line)){
				line++;
			}
		}
		if(*line){
			count++;
			while(*line && !isspace(*line)){
				line++;
			}
		}
	}
	return count;
}


static int
tokenize_line(char *line, int argc, char **argv){
	int count;

	count = 0;
	while(*line){
		if(isspace(*line)){
			while(*line && isspace(*line)){
				*line++ = 0;
			}
		}
		if(*line){
			argv[count++] = line;
			while(*line && !isspace(*line)){
				line++;
			}
		}
	}
	argv[count] = 0;
	return count;
}


struct command *
find_command(struct command *cmds, char *name){
	struct command *cmd;

	cmd = cmds;
	while(cmd->c_name){
		if(strcmp(name, cmd->c_name) == 0){
			return cmd;
		}
		cmd++;
	}
	return NULL;
}


static int
execute_line(struct command *cmds, char *line){
	int i;
	int argc;
	char **argv;
	struct command *cmd;

	argc = count_tokens(line);
	if(argc == 0){
		return 0;
	}
	argv = alloca(argc * sizeof(char *) + 1);
	tokenize_line(line, argc, argv);

	cmd = find_command(cmds, argv[0]);
	if(cmd){
		if(cmd->c_fn){
			return cmd->c_fn(argc, argv);
		} else {
			printf("%s: not implemented\n", argv[0]);
			return 0;
		}
	} else {
		printf("%s: command not found\n", argv[0]);
	}

	return 0;
}


char *cmd_generator(const char *text, int state){
	static int i, len;
	char *name;
	char *s;

	if(state == 0){
		i = 0;
		len = strlen(text);
	}

	name = commands[i].c_name;
	while(name != NULL){
		if(strncmp(name, text, len) == 0){
			s = malloc(strlen(name) + 1);
			strcpy(s, name);
			i += 1;
			return s;
		}
		i += 1;
		name = commands[i].c_name;
	}
	return NULL;
}

char **cmd_completion(const char *text, int start, int end){
	char **matches;

	if(start == 0){
		return rl_completion_matches(text, cmd_generator);
	} else {
		return NULL;
	}
}


int
interact(struct command *cmds, char *prompt){
	char *line;
	char *s;

	commands = cmds;
	rl_attempted_completion_function = cmd_completion;

	while(!interact_done){
		line = readline(prompt);
		if(!line){
			break;
		}
		s = skipwhite(line);
		add_history(s);
		execute_line(cmds, s);
		free(line);
	}
	return 0;
}
