/*
 * Copyright 2014 Joseph Landry
 */

#ifndef BYTES_H
#define BYTES_H

#include <stdint.h>

struct bytes {
	uint32_t addr;
	uint32_t count;
	uint32_t *byte_flags;
};

#define BYTE_MASK     0x000000FF

#define VALUE_MASK    0x00000100
#define HAS_VALUE     0x00000100

#define TYPE_MASK     0x00000600
#define TYPE_UNKNOWN  0x00000000
#define TYPE_CODE     0x00000200
#define TYPE_DATA     0x00000400
#define TYPE_TAIL     0x00000600

#define DATATYPE_MASK     0x0000F000
#define DATATYPE_BYTE     0x00001000
#define DATATYPE_WORD     0x00002000
#define DATATYPE_DWORD    0x00003000
#define DATATYPE_QWORD    0x00004000

#define BYTE_VALUE(A) (A & BYTE_MASK)
#define TYPE(A) (A & TYPE_MASK)


static inline uint8_t
get_byte_value(uint32_t flags){
	return flags & BYTE_MASK;
}

static inline uint8_t
set_byte_value(uint32_t flags, uint8_t byte){
	return (flags & ~BYTE_MASK) | byte;
}

static inline uint32_t
get_has_value(uint32_t flags){
	return flags & VALUE_MASK;
}

static inline uint32_t
set_has_value(uint32_t flags){
	return (flags & ~VALUE_MASK) | HAS_VALUE;
}

static inline uint32_t
is_value(uint32_t flags){
	return get_has_value(flags) == HAS_VALUE;
}

static inline uint32_t
get_type(uint32_t flags){
	return flags & TYPE_MASK;
}

static inline int
is_unknown(uint32_t flags){
	return get_type(flags) == TYPE_UNKNOWN;
}

static inline int
set_unknown(uint32_t flags){
	return (flags & ~TYPE_MASK) | TYPE_UNKNOWN;
}

static inline int
is_code(uint32_t flags){
	return get_type(flags) == TYPE_CODE;
}

static inline int
set_code(uint32_t flags){
	return (flags & ~TYPE_MASK) | TYPE_CODE;
}

static inline int
is_data(uint32_t flags){
	return get_type(flags) == TYPE_DATA;
}

static inline uint32_t
set_data(uint32_t flags){
	return (flags & ~TYPE_MASK) | TYPE_DATA;
}

static inline int
is_tail(uint32_t flags){
	return get_type(flags) == TYPE_TAIL;
}

static inline uint32_t set_tail(uint32_t flags){
	return (flags & ~TYPE_MASK) | TYPE_TAIL;
}

static inline int
get_datatype(uint32_t flags){
	return (flags & DATATYPE_MASK);
}

static inline int
is_datatype_byte(uint32_t flags){
	return get_datatype(flags) == DATATYPE_BYTE;
}

static inline uint32_t
set_datatype_byte(uint32_t flags){
	return (flags & ~DATATYPE_MASK) | DATATYPE_BYTE;
}

static inline int
is_datatype_word(uint32_t flags){
	return get_datatype(flags) == DATATYPE_WORD;
}

static inline uint32_t
set_datatype_word(uint32_t flags){
	return (flags & ~DATATYPE_MASK) | DATATYPE_WORD;
}

static inline int
is_datatype_dword(uint32_t flags){
	return get_datatype(flags) == DATATYPE_DWORD;
}

static inline uint32_t
set_datatype_dword(uint32_t flags){
	return (flags & ~DATATYPE_MASK) | DATATYPE_DWORD;
}

static inline int
is_datatype_qword(uint32_t flags){
	return get_datatype(flags) == DATATYPE_QWORD;
}

static inline uint32_t
set_datatype_qword(uint32_t flags){
	return (flags & ~DATATYPE_MASK) | DATATYPE_QWORD;
}

extern int 
create_bytes(char *filename, uint32_t base_addr, struct bytes *bytes);

extern int
load_bytes(char *filename, struct bytes *bytes);

extern int
save_bytes(char *filename, struct bytes *bytes);

#endif
