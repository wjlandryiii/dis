/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "mapping.h"

struct mapping *
new_mapping(void){
	struct mapping *map;

	map = calloc(1, sizeof(*map));
	if(map){
	}
	return map;
}

int
free_mapping(struct mapping *map){
	struct mapnode *node;
	struct mapnode *next;

	for(node = map->map_list; node != NULL; node = next){
		next = node->mn_next;
		free(node);
	}
	free(map);
	return 0;
}

static struct mapnode *
new_mapnode(uint64_t key, uint64_t value){
	struct mapnode *node;

	node = calloc(1, sizeof(*node));
	if(node){
		node->mn_key = key;
		node->mn_value = value;
	}
	return node;
}

static struct mapnode *
find_mapnode(struct mapping *map, uint64_t key){
	struct mapnode *node;

	for(node = map->map_list; node != NULL; node = node->mn_next){
		if(node->mn_key == key){
			return node;
		}
	}
	return NULL;
}

static int
insert_mapnode(struct mapping *map, struct mapnode *node){
	node->mn_next = map->map_list;
	map->map_list = node;
	return 0;
}

int
set_mapping(struct mapping *map, uint64_t key, uint64_t value){
	struct mapnode *node;
	int32_t sid;

	node = find_mapnode(map, key);
	if(node){
		node->mn_value = value;
	} else {
		node = new_mapnode(key, value);
		insert_mapnode(map, node);
	}
	return 0;
}

int
lookup_mapping(struct mapping *map, uint64_t key, uint64_t *value){
	struct mapnode *node;

	node = find_mapnode(map, key);
	if(node){
		if(value){
			*value = node->mn_value;
		}
		return 0;
	} else {
		return -1;
	}
}

int
pack_mapping(struct mapping *map, FILE *f){
	struct mapnode *node;
	int64_t count;
	int r;

	count = 0;
	for(node = map->map_list; node != NULL; node = node->mn_next){
		count++;
	}

	r = fwrite(&count, sizeof(count), 1, f);
	if(r != 1){
		fprintf(stderr, "fwrite()\n");
		goto fail;
	}
	for(node = map->map_list; node != NULL; node = node->mn_next){
		r = fwrite(&(node->mn_key), sizeof(node->mn_key), 1, f);
		if(r != 1){
			fprintf(stderr, "fwrite()\n");
			goto fail;
		}
		r = fwrite(&(node->mn_value), sizeof(node->mn_value), 1, f);
		if(r != 1){
			fprintf(stderr, "fwrite()\n");
			goto fail;
		}
	}
	return 0;

fail:
	return -1;
}

struct mapping *
unpack_mapping(FILE *f){
	struct mapping *map;
	uint64_t key;
	uint64_t value;
	int64_t count;
	int64_t i;
	int r;

	map = new_mapping();

	r = fread(&count, sizeof(count), 1, f);
	if(r != 1){
		fprintf(stderr, "fread()\n");
		goto fail;
	}
	for(i = 0; i < count; i++){
		r = fread(&key, sizeof(key), 1, f);
		if(r != 1){
			fprintf(stderr, "fread()\n");
			goto fail;
		}
		r = fread(&value, sizeof(value), 1, f);
		if(r != 1){
			fprintf(stderr, "fread()\n");
			goto fail;
		}
		set_mapping(map, key, value);
	}
	return map;

fail:
	return NULL;
}
