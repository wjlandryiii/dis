/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "stringstore.h"

struct stringstore *
new_stringstore(void){
	struct stringstore *ss;

	ss = calloc(1, sizeof(*ss));
	if(ss){
		ss->ss_nextid = 1;
	}
	return ss;
}

static struct stringnode *
new_stringnode(int32_t id, const char *s){
	struct stringnode *node;

	node = calloc(1, sizeof(*node));
	if(!node){
		fprintf(stderr, "calloc()");
		goto fail;
	}

	node->sn_str = malloc(strlen(s)+1);
	if(!node->sn_str){
		fprintf(stderr, "malloc()\n");
		goto fail_node;
	}

	node->sn_next = NULL;
	node->sn_id = id;
	strcpy(node->sn_str, s);
	return node;

fail_node:
	free(node);
fail:
	return NULL;	
}

int
free_stringstore(struct stringstore *store){
	struct stringnode *node;
	struct stringnode *next;

	for(node = store->ss_list; node != NULL; node = next){
		next = node->sn_next;
		free(node->sn_str);
		free(node);
	}
	free(store);
	return 0;
}

int insert_node(struct stringstore *store, struct stringnode *node){
	if(node){
		node->sn_next = store->ss_list;
		store->ss_list = node;
		return 0;
	} else {
		return -1;
	}
}

static int
insert_string(struct stringstore *store, int32_t id, uint8_t *s){
	struct stringnode *node;

	node = new_stringnode(id, s);
	if(node){
		insert_node(store, node);
		return 0;
	} else {
		return -1;
	}
}

int32_t
store_string(struct stringstore *store, char *s){
	int r;

	r = insert_string(store, store->ss_nextid, s);
	if(!r){
		return store->ss_nextid++;
	} else {
		return -1;
	}
}

static struct stringnode *
get_node_by_id(struct stringstore *ss, uint32_t id){
	struct stringnode *node;

	node = ss->ss_list;
	while(node){
		if(node->sn_id == id){
			break;
		}
		node = node->sn_next;
	}
	return node;
}

int
retrieve_string(struct stringstore *ss, int32_t id, char *s, size_t size){
	struct stringnode *node;
	size_t len;

	if(id >= 0){
		node = get_node_by_id(ss, id);
		if(node){
			len = strlen(node->sn_str);
			if(len < size){
				strcpy(s, node->sn_str);
			} else {
				strncpy(s, node->sn_str, size);
				if(size - 1 >= 0)
					s[size-1] = 0;
				if(size - 2 >= 0)
					s[size-2] = '.';
			       	if(size - 3 >= 0)
					s[size-3] = '.';
				if(size - 4 >= 0)
					s[size-4] = '.';
			}
			return len + 1;
		} else {
			goto fail;
		}
	} else {
		goto fail;
	}

fail:
	return -1;
}

int
update_string(struct stringstore *store, int32_t id, char *s){
	struct stringnode *node;
	size_t len;
	uint8_t *buf;

	node = get_node_by_id(store, id);
	if(node){
		len = strlen(s);
		if(strlen(s) < strlen(node->sn_str)){
			strcpy(node->sn_str, s);
		} else {
			buf = realloc(node->sn_str, len+1);
			if(buf){
				strcpy(buf, s);
				node->sn_str = buf;
			} else {
				fprintf(stderr, "realloc()\n");
				goto fail;
			}
		}
	} else {
		goto fail;
	}

	return 0;
fail:
	return -1;
}


int
pack_stringstore(struct stringstore *store, FILE *f){
	int r;
	struct stringnode *node;
	int32_t size;

	r = fwrite(&(store->ss_nextid), sizeof(store->ss_nextid), 1, f);
	if(r != 1){
		fprintf(stderr, "fwrite()\n");
		goto fail;
	}

	for(node = store->ss_list; node != NULL; node = node->sn_next){
		r = fwrite(&(node->sn_id), sizeof(node->sn_id), 1, f);
		if(r != 1){
			fprintf(stderr, "fwrite()\n");
			goto fail;
		}

		size = strlen(node->sn_str) + 1;
		r = fwrite(&size, sizeof(size), 1, f);
		if(r != 1){
			fprintf(stderr, "fwrite()\n");
			goto fail;
		}

		r = fwrite(node->sn_str, 1, size, f);
		if(r != size){
			fprintf(stderr, "fwrite()\n");
			goto fail;
		}
	}
	size = -1;
	r = fwrite(&size, sizeof(size), 1, f);
	if(r != 1){
		fprintf(stderr, "fwrite()\n");
		goto fail;
	}
	return 0;

fail:
	return -1;
}


struct stringstore *
unpack_stringstore(FILE *f){
	struct stringstore *store;
	int r;
	int32_t id;
	int32_t size;
	uint8_t *buf;

	store = new_stringstore();
	r = fread(&(store->ss_nextid), sizeof(store->ss_nextid), 1, f);
	if(r != 1){
		fprintf(stderr, "fread()\n");
		goto fail;
	}
	for(;;){
		r = fread(&id, sizeof(id), 1, f);
		if(r != 1){
			fprintf(stderr, "fread()\n");
			goto fail;
		}

		if(id < 0){
			break;
		}

		r = fread(&size, sizeof(size), 1, f);
		if(r != 1){
			fprintf(stderr, "fread()\n");
			goto fail;
		}
	
		buf = malloc(size);
		if(!buf){
			fprintf(stderr, "malloc()\n");
			goto fail;
		}
		r = fread(buf, sizeof(uint8_t), size, f);
		if(r != size){
			fprintf(stderr, "fread()\n");
			goto fail;
		}
		buf[size-1] = 0;

		insert_string(store, id, buf);
	}

	return store;
fail:
	return NULL;
}
