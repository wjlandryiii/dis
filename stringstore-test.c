#include <stdio.h>
#include <stdint.h>

#include "stringstore.h"


void make_file(FILE *f){
	uint8_t buf[1024];
	struct stringstore *store;
	uint32_t id_A;
	uint32_t id_B;
	uint32_t id_C;

	store = new_stringstore();

	id_A = store_string(store, "Hello World!");
	id_B = store_string(store, "BEEFCAKE");
	id_C = store_string(store, "Once apon a midnight dreary, "
			"while I ponder, weak and weary");

	retrieve_string(store, id_A, buf, sizeof(buf));
	printf("%d: %s\n", id_A, buf);

	retrieve_string(store, id_B, buf, 5);
	printf("%d: %s\n", id_B, buf);

	retrieve_string(store, id_C, buf, 15);
	printf("%d: %s\n", id_C, buf);


	update_string(store, id_A, "Foo");
	retrieve_string(store, id_A, buf, sizeof(buf));
	printf("%d: %s\n", id_A, buf);

	update_string(store, id_A, "Foo Bar Bazzoooka");
	retrieve_string(store, id_A, buf, sizeof(buf));
	printf("%d: %s\n", id_A, buf);

	pack_stringstore(store, f);
	free_stringstore(store);
}

void load_file(FILE *f){
	uint8_t buf[1024];
	struct stringstore *store;
	uint32_t id_A = 1;
	uint32_t id_B = 2;
	uint32_t id_C = 3;

	store = unpack_stringstore(f);

	retrieve_string(store, id_A, buf, sizeof(buf));
	printf("%d: %s\n", id_A, buf);

	retrieve_string(store, id_B, buf, 5);
	printf("%d: %s\n", id_B, buf);

	retrieve_string(store, id_C, buf, 15);
	printf("%d: %s\n", id_C, buf);
	free_stringstore(store);
}


int main(int argc, char *argv[]){
	FILE *f;

	f = tmpfile();

	printf("MAKING FILE...\n");
	make_file(f);
	fseek(f, 0, SEEK_SET);
	printf("\nLOADING FILE...\n");
	load_file(f);
	
	return 0;
}
