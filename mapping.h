#ifndef MAPPING_H
#define MAPPING_H

#include <stdint.h>

struct mapnode {
	struct mapnode *mn_next;
	uint64_t mn_key;
	uint64_t mn_value;
};

struct mapping {
	struct mapnode *map_list;
};


struct mapping *
new_mapping(void);

int
free_mapping(struct mapping *map);

int
set_mapping(struct mapping *map, uint64_t key, uint64_t value);

int
lookup_mapping(struct mapping *map, uint64_t key, uint64_t *value);

int
pack_mapping(struct mapping *map, FILE *f);

struct mapping *
unpack_mapping(FILE *f);

#endif
