#include <stdio.h>
#include <stdlib.h>

#include "bytes.h"


int fake_ok(char *filename){
	struct bytes bytes;
	uint32_t *byte_flags;
	int r;
	
	r = load_bytes(filename, &bytes);
	if(r){
		fprintf(stderr, "load_bytes()\n");
		goto fail;
	}

	byte_flags = bytes.byte_flags;

	byte_flags[0] = set_code(byte_flags[0]);
	byte_flags[1] = set_tail(byte_flags[1]);
	byte_flags[2] = set_tail(byte_flags[2]);
	byte_flags[3] = set_tail(byte_flags[3]);
	byte_flags[4] = set_tail(byte_flags[4]);

	byte_flags[5] = set_code(byte_flags[5]);
	byte_flags[6] = set_tail(byte_flags[6]);

	byte_flags[7] = set_code(byte_flags[7]);
	byte_flags[8] = set_tail(byte_flags[8]);

	byte_flags[9] = set_code(byte_flags[9]);
	byte_flags[10] = set_tail(byte_flags[10]);
	
	byte_flags[11] = set_code(byte_flags[11]);
	
	byte_flags[12] = set_code(byte_flags[12]);
	byte_flags[13] = set_tail(byte_flags[13]);

	byte_flags[14] = set_code(byte_flags[14]);
	byte_flags[15] = set_tail(byte_flags[15]);
	
	byte_flags[16] = set_code(byte_flags[16]);
	
	byte_flags[17] = set_code(byte_flags[17]);
	byte_flags[18] = set_tail(byte_flags[18]);

	byte_flags[19] = set_code(byte_flags[19]);
	byte_flags[20] = set_tail(byte_flags[20]);
	
	byte_flags[21] = set_code(byte_flags[21]);
	
	byte_flags[22] = set_code(byte_flags[22]);
	byte_flags[23] = set_tail(byte_flags[23]);

	byte_flags[24] = set_code(byte_flags[24]);
	byte_flags[25] = set_tail(byte_flags[25]);

	byte_flags[26] = set_code(byte_flags[26]);

	r = save_bytes(filename, &bytes);
	if(r){
		fprintf(stderr, "save_bytes()\n");
		goto fail;
	}
	return 0;
fail:
	printf("failed\n");
	return -1;
}

int fake_hello(char *filename){
	struct bytes bytes;
	uint32_t *bf;
	int r;
	int i;
	
	r = load_bytes(filename, &bytes);
	if(r){
		fprintf(stderr, "load_bytes()\n");
		goto fail;
	}

	i = 0;
	bf = bytes.byte_flags;

	// start
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);

	// begin	
	bf[i] = set_code(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);

	//strlen
	bf[i] = set_code(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);

	//strlen_loop
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);

	//strlen_done
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);

	//strlen_end
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);

	//exit
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);

	//end
	bf[i] = set_code(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);
	bf[i] = set_tail(bf[i++]);

	//hello_string
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));
	bf[i] = set_datatype_byte(set_data(bf[i++]));

	r = save_bytes(filename, &bytes);
	if(r){
		fprintf(stderr, "save_bytes()\n");
		goto fail;
	}
	return 0;
fail:
	printf("failed\n");
	return -1;
}

int main(int argc, char *argv[]){
	fake_ok("data/ok.bin.db");
	fake_hello("data/helloworld.bin.db");
}
