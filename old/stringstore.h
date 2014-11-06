#ifndef STORESTRING_H
#define STORESTRING_H

#include <stdio.h>
#include <stdint.h>

struct stringnode {
	struct stringnode *sn_next;
	int32_t sn_id;
	char *sn_str;
};

struct stringstore {
	int32_t ss_nextid;
	struct stringnode *ss_list;
};

struct stringstore *
new_stringstore(void);

int
free_stringstore(struct stringstore *store);

int32_t
store_string(struct stringstore *store, char *s);

int
retrieve_string(struct stringstore *store, int32_t id, char *s, size_t size);

int
update_string(struct stringstore *store, int32_t id, char *s);

int
pack_stringstore(struct stringstore *store, FILE *fout);

struct stringstore *
unpack_stringstore(FILE *fin);

#endif
