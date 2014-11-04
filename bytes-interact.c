/*
 * Copyright 2014 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <stdarg.h>
#define  __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "interact.h"
#include "bytes.h"

int tracing = 1;

int
trace_set_class_code(struct bytes *bytes, uint64_t first, uint64_t last){
	int r;

	if(tracing){
		printf("set_class_code(%p, 0x%" PRIx64 ", 0x%" PRIx64 ") : ",
				bytes, first, last);
		fflush(stdout);
	}
	r = set_class_code(bytes, first, last);
	if(tracing){
		printf("%d\n", r);
	}
	return r;
}

const struct command commands[];

int help_main(int argc, char *argv[]){
	const struct command *cmd;

	cmd = commands;
	while(cmd->c_name){
		printf("%10s: %s\n", cmd->c_name, cmd->c_description);
		cmd++;
	}
	return 0;
}

int exit_main(int argc, char *argv[]){
	interact_done = 1;
	return 0;
}

struct bytes *bytes = NULL;

int new_main(int argc, char *argv[]){
	if(bytes){
		free_bytes(bytes);
		bytes = NULL;
	}
	bytes = new_bytes();
	printf("new bytes at %p\n", bytes);
	return 0;
}

int free_main(int argc, char *argv[]){
	if(bytes){
		free_bytes(bytes);
		bytes = NULL;
		printf("bytes: %p\n", bytes);
	} else {
		printf("already freed\n");
	}
	return 0;
}

int print_main(int argc, char *argv[]){
	struct bytechunk *chunk;
	int size;
	int i;

	printf("bytes: %p\n", bytes);
	printf("chunks:\n");
	chunk = first_chunk(bytes);
	while(chunk){
		size = (chunk->bc_last - chunk->bc_first) + 1;
		printf("\tchunk: %p\n", chunk);
		printf("\t\tfirst: 0x%1$" PRIx64 " (%1$" PRIu64 ")\n", chunk->bc_first);
		printf("\t\t last: 0x%1$" PRIx64 " (%1$" PRIu64 ")\n", chunk->bc_last);
		printf("\t\tbytes:\n");
		for(i = 0; i < size; i++){
			printf("\t\t\t%02" PRIx64 ": %08x\n",
					chunk->bc_first + i,
					chunk->bc_bytes[i]);
		}
		printf("\n");
		chunk = next_chunk(chunk);
	}
	printf("\tchunk: %p\n", chunk);

	return 0;
}

int enable_main(int argc, char *argv[]){
	uint64_t first;
	uint64_t last;
	char *p;
	int r;

	if(argc < 3){
		printf("%s: [first] [last]\n", argv[0]);
		return 1;
	} else {
		first = strtoull(argv[1], &p, 0);
		if(*p != 0){
			printf("not an integer: %s\n", argv[1]);
			return 1;
		}
		last = strtoull(argv[2], &p, 0);
		if(*p != 0){
			printf("not an integer: %s\n", argv[2]);
			return 1;
		}
		r = enable_bytes(bytes, first, last);
		printf("enable_bytes(%p, 0x%" PRIx64 ", 0x%" PRIx64 ") : %d\n",
				bytes, first, last, r);
		return 0;
	}
}

int copy_to_bytes_main(int argc, char *argv[]){
	uint64_t addr;
	size_t len;
	char *s;
	char *p;
	int r;

	if(argc < 3){
		printf("%s: [addr] [string]\n", argv[0]);
		return 1;
	} else {
		addr = strtoull(argv[1], &p, 0);
		if(*p != 0){
			printf("not an integer: %s\n", argv[1]);
			return 1;
		}
		s =  argv[2];
		len = strlen(s);
		r = copy_to_bytes(bytes, addr, (uint8_t *)s, len);
		printf("copy_to_bytes(%p, 0x%" PRIx64 ", \"%.20s\", %3" PRIu64 ") : %d\n",
				bytes, addr, s, len, r);
		return 0;
	}
}

int copy_from_bytes_main(int argc, char *argv[]){
	uint64_t addr;
	size_t size;
	uint8_t *buf;
	char *p;
	int i;
	int j;
	int r;

	if(argc < 3){
		printf("%s: [addr] [size]\n", argv[0]);
		return 1;
	} else {
		addr = strtoull(argv[1], &p, 0);
		if(*p != 0){
			printf("not an integer: %s\n", argv[1]);
			return 1;
		}
		size = strtoul(argv[2], &p, 0);
		if(*p != 0){
			printf("not an integer: %s\n", argv[2]);
			return 1;
		}
		buf = alloca(size);
		r = copy_from_bytes(bytes, addr, (uint8_t *)buf, size);
		printf("copy_from_bytes(%p, 0x%" PRIx64 ", %p, %zu) : %d\n",
				bytes, addr, buf, size, r);
		if(r < 0){
			return 1;
		}
		for(i = 0; i < size; i+= 16){
			printf("\t%" PRIx64 ":  ", addr + i);
			for(j = 0; j < 16 && j + i < size; j++){
				printf(" %02x", buf[i+j]);
			}
			printf("\n");
		}
		return 0;
	}
}

int unpack_args(int argc, char *argv[], char *usage,  char *fmt, ...){
	va_list ap;
	int len;
	int i;
	char **s;
	uint64_t *Q;
	char *p;

	len = strlen(fmt);
	if(len != argc){
		printf("%s: %s\n", argv[0], usage);
		return 0;
	}

	va_start(ap, fmt);
	for(i = 0; i < len; i++){
		if(fmt[i] == 's'){
			s = va_arg(ap, char **);
			*s = argv[i];
		} else if(fmt[i] == 'Q'){
			Q = va_arg(ap, uint64_t *);
			*Q = strtoull(argv[i], &p, 0);
			if(*p != 0){
				printf("invalid integer: %s\n", argv[i]);
				return i;
			}
		}
	}
	return i;
}

unsigned char ok_bin[] = {
	0x68, 0x4f, 0x4b, 0x0a, 0x41, 0x31, 0xc0, 0xb0, 0x04, 0x31, 0xdb, 0x43,
	0x89, 0xe1, 0x89, 0xc2, 0x4a, 0xcd, 0x80, 0x31, 0xc0, 0x40, 0x31, 0xdb,
	0xcd, 0x80, 0xf4
};
unsigned int ok_bin_len = 27;

unsigned char helloworld_bin[] = {
	0xeb, 0x31, 0x5e, 0x89, 0xf0, 0x51, 0x89, 0xc1, 0x80, 0x39, 0x00, 0x74,
	0x05, 0x83, 0xc1, 0x01, 0xeb, 0xf6, 0x29, 0xc1, 0x89, 0xc8, 0x59, 0x89,
	0xc2, 0x89, 0xf1, 0xbb, 0x01, 0x00, 0x00, 0x00, 0xb8, 0x04, 0x00, 0x00,
	0x00, 0xcd, 0x80, 0xbb, 0x00, 0x00, 0x00, 0x00, 0xb8, 0x01, 0x00, 0x00,
	0x00, 0xcd, 0x80, 0xe8, 0xca, 0xff, 0xff, 0xff, 0x48, 0x65, 0x6c, 0x6c,
	0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0a, 0x00
};
unsigned int helloworld_bin_len = 70;

int load_main(int argc, char *argv[]){
	char *usage = "[ address ] [ ok | hello ]";
	char *name;
	uint64_t addr;
	char *which;
	int n;

	uint8_t *buf;
	size_t size;
	int r;

	n = unpack_args(argc, argv, usage, "sQs", &name, &addr, &which);
	if(n != 3){
		return 1;
	}
	if(strcmp(which, "ok") == 0){
		buf = ok_bin;
		size = ok_bin_len;
	} else if(strcmp(which, "hello") == 0){
		buf = helloworld_bin;
		size = helloworld_bin_len;
	} else {
		printf("%s: invalid\n", which);
		return 1;
	}

	r = enable_bytes(bytes, addr, addr+size-1);
	if(r){
		printf("enable_bytes(): %d\n", r);
		return 1;
	}

	r = copy_to_bytes(bytes, addr, buf, size);
	if(r){
		printf("copy_to_bytes(): %d\n", r);
		return 1;
	}
	return 0;
}


int code_main(int argc, char *argv[]){
	char *usage = "[addr] [len]";
	char *name;
	uint64_t addr;
	uint64_t len;
	int n;
	int r;

	n = unpack_args(argc, argv, usage, "sQQ", &name, &addr, &len);
	if(n != 3){
		return 1;
	}

	r = trace_set_class_code(bytes, addr, (addr+len)-1);
	if(r){
		printf("set_class_code(): %d\n", r);
		return 1;
	}
	return 0;
}

int data_main(int argc, char *argv[]){
	char *usage = "[addr] [len]";
	char *name;
	uint64_t addr;
	uint64_t len;
	int n;
	int r;

	n = unpack_args(argc, argv, usage, "sQQ", &name, &addr, &len);
	if(n != 3){
		return 1;
	}

	r = set_class_data(bytes, addr, (addr+len)-1);
	if(r){
		printf("set_class_data(): %d\n", r);
		return 1;
	}
	return 0;
}

int unknown_main(int argc, char *argv[]){
	char *usage = "[addr] [len]";
	char *name;
	uint64_t addr;
	uint64_t len;
	int n;
	int r;

	n = unpack_args(argc, argv, usage, "sQQ", &name, &addr, &len);
	if(n != 3){
		return 1;
	}

	r = set_class_unknown(bytes, addr, (addr+len)-1);
	if(r){
		printf("set_class_unknown(): %d\n", r);
		return 1;
	}
	return 0;
}

int get_main(int argc, char *argv[]){
	char *usage = "[db|dw|dd|dq] [addr]";
	char *name;
	char *type;
	uint64_t addr;
	int n;
	int r;

	uint8_t B;
	uint16_t W;
	uint32_t D;
	uint64_t Q;

	n = unpack_args(argc, argv, usage, "ssQ", &name, &type, &addr);
	if(n != 3){
		return 1;
	}

	if(strcmp(type, "db") == 0){
		r = bytes_get_byte(bytes, addr, &B);
		if(r){
			goto fail;
		}
		printf("0x%02x\n", B);
	} else if(strcmp(type, "dw") == 0){
		r = bytes_get_word(bytes, addr, &W);
		if(r){
			goto fail;
		}
		printf("0x%04x\n", W);

	} else if(strcmp(type, "dd") == 0){
		r = bytes_get_dword(bytes, addr, &D);
		if(r){
			goto fail;
		}
		printf("0x%08x\n", D);

	} else if(strcmp(type, "dq") == 0){
		r = bytes_get_qword(bytes, addr, &Q);
		if(r){
			goto fail;
		}
		printf("0x%016" PRIx64 "\n", Q);

	} else {
		printf("invalid type: %s\n", type);
		return 1;
	}

	return 0;
fail:
	printf("Failed: %d\n", r);
	return 1;
}

int put_main(int argc, char *argv[]){
	char *usage = "[db|dw|dd|dq] [addr] [value]";
	char *name;
	char *type;
	uint64_t addr;
	uint64_t value;
	int n;
	int r;

	uint8_t B;
	uint16_t W;
	uint32_t D;
	uint64_t Q;

	n = unpack_args(argc, argv, usage, "ssQQ", &name, &type, &addr, &value);
	if(n != 4){
		return 1;
	}

	if(strcmp(type, "db") == 0){
		if(value > 0xFF){
			printf("invalid value for byte: 0x%016" PRIx64 "\n", value);
			goto fail;
		}
		B = value;
		r = bytes_put_byte(bytes, addr, B);
		if(r){
			goto fail;
		}
	} else if(strcmp(type, "dw") == 0){
		if(value > 0xFFFF){
			printf("invalid value for word: 0x%016" PRIx64 "\n", value);
			goto fail;
		}
		W = value;
		r = bytes_put_word(bytes, addr, W);
		if(r){
			goto fail;
		}
	} else if(strcmp(type, "dd") == 0){
		if(value > 0xFFFFFFFF){
			printf("invalid value for dword: 0x%016" PRIx64 "\n", value);
			goto fail;
		}
		D = value;
		r = bytes_put_dword(bytes, addr, D);
		if(r){
			goto fail;
		}
	} else if(strcmp(type, "dq") == 0){
		Q = value;
		printf("bytes_put_qword(%p, 0x%" PRIx64 ", 0x%" PRIx64 ") : ",
				bytes, addr, Q);
		fflush(stdout);
		r = bytes_put_qword(bytes, addr, Q);
		printf("%d\n", r);
		if(r){
			goto fail;
		}

	} else {
		printf("invalid type: %s\n", type);
		return 1;
	}

	return 0;
fail:
	printf("Failed: %d\n", r);
	return 1;
}

int head_main(int argc, char *argv[]){
	char *usage = "[addr]";
	char *name;
	uint64_t addr;
	uint64_t head;
	int r;
	int n;

	n = unpack_args(argc, argv, usage, "sQ", &name, &addr);
	if(n != 2){
		return 1;
	}

	r = item_head(bytes, addr, &head);
	if(!r){
		printf("head: 0x%" PRIx64 "\n", head);
		return 0;
	} else {
		printf("error: %d\n", r);
		return 1;
	}
}

int end_main(int argc, char *argv[]){
	char *usage = "[addr]";
	char *name;
	uint64_t addr;
	uint64_t end;
	int r;
	int n;

	n = unpack_args(argc, argv, usage, "sQ", &name, &addr);
	if(n != 2){
		return 1;
	}
	
	r = item_end(bytes, addr, &end);
	if(!r){
		printf("head: 0x%" PRIx64 "\n", end);
		return 0;
	} else {
		printf("error: %d\n", r);
		return 1;
	}
}

int getdt_main(int argc, char *argv[]){
	char *usage = "[addr]";
	char *name;
	uint64_t addr;
	uint32_t datatype;
	int n;
	int r;

	n = unpack_args(argc, argv, usage, "sQ", &name, &addr);
	if(n != 2){
		return 1;
	}

	r = get_bytes_datatype(bytes, addr, &datatype);
	if(r){
		printf("error: %d\n", r);
		return 1;
	}
	if(datatype == DATATYPE_BYTE){
		printf("BYTE\n");
	} else if(datatype == DATATYPE_WORD){
		printf("WORD\n");
	} else if(datatype == DATATYPE_DWORD){
		printf("DWORD\n");
	} else if(datatype == DATATYPE_QWORD){
		printf("QWORD\n");
	} else {
		printf("unknown datatype: %" PRIu32 "\n", datatype);
	}
	return 0;
}

int setdt_main(int argc, char *argv[]){
	char *usage = "[addr] [db|dw|dd|dq]";
	char *name;
	uint64_t addr;
	char *type;
	int n;
	int r;

	n = unpack_args(argc, argv, usage, "sQs", &name, &addr, &type);
	if(n != 3){
		return 1;
	}

	if(strcmp(type, "db") == 0){
		r = set_bytes_datatype_byte(bytes, addr);
		if(r){
			goto fail;
		}
	} else if(strcmp(type, "dw") == 0){
		r = set_bytes_datatype_word(bytes, addr);
		if(r){
			goto fail;
		}
	} else if(strcmp(type, "dd") == 0){
		r = set_bytes_datatype_dword(bytes, addr);
		if(r){
			goto fail;
		}
	} else if(strcmp(type, "dq") == 0){
		r = set_bytes_datatype_qword(bytes, addr);
		if(r){
			goto fail;
		}
	} else {
		printf("Unknown type: %s\n", type);
		return 1;
	}
	return 0;
fail:
	printf("error: %d\n", r);
	return 1;
}

const struct command commands[] = {
	{"help", help_main, "show this text"},
	{"exit", exit_main, "go back to DOS"},
	{"quit", exit_main, "go back to DOS"},
	{"new", new_main, "create new bytes structure"},
	{"free", free_main, "delete current bytes structure"},
	{"print", print_main, "print current bytes structure"},
	{"enable", enable_main, "enable region of bytes"},
	{"copy_to", copy_to_bytes_main, "copy to bytes"},
	{"copy_from", copy_from_bytes_main, "copy from bytes"},
	{"load", load_main, "load [address] [ok|hello]"},
	{"code", code_main, "mark region as code"},
	{"data", data_main, "mark region as data"},
	{"unknown", unknown_main, "mark region as unknown"},
	{"get", get_main, "read a primative type from bytes"},
	{"put", put_main, "write a primative type from bytes"},
	{"head", head_main, "find the first address of an item"},
	{"end", end_main, "find the end of an item"},
	{"getdt", getdt_main, "get the datatype a data item"},
	{"setdt", setdt_main, "make a data item and set the datatype"},
	{NULL, NULL, NULL},
};

int main(int argc, char *argv[]){
	new_main(0, NULL);
	return interact(commands, "bytes>");
}
