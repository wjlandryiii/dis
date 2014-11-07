/*
 * Copyright 2014 Joseph Landry
 */

#ifndef BYTEFIELDS_H
#define BYTEFIELDS_H

#include <stdint.h>

#define BYTE_VALUE_MASK         0x000000FF

#define BYTE_VALUE_FIELD(A)		(A & BYTE_VALUE_MASK)
#define SET_BYTE_VALUE_FIELD(A,B)	((A & ~BYTE_VALUE_MASK) | B)

static inline uint8_t
get_byte_value_field(uint32_t flags){
	return BYTE_VALUE_FIELD(flags);
}

static inline uint32_t
set_byte_value_field(uint32_t flags, uint8_t byte){
	return SET_BYTE_VALUE_FIELD(flags, byte);
}

#define VALUE_MASK        0x00000F00
#define VALUE_INVALID	  0x00000000
#define VALUE_VALID       0x00000100

#define VALUE_FIELD(A)		(A & VALUE_MASK)
#define SET_VALUE_FIELD(A,B)	((A & ~VALUE_MASK) | B)
#define TEST_VALUE_FIELD(A,B)	(VALUE_FIELD(A) == B)
#define IS_VALUE_INVALID(A)	(TEST_VALUE_FIELD(A, VALUE_INVALID))
#define IS_VALUE_VALID(A)	(TEST_VALUE_FIELD(A, VALUE_VALID))

static inline uint32_t
get_value_field(uint32_t flags){
	return VALUE_FIELD(flags);
}

static inline uint32_t
set_value_field(uint32_t flags, uint32_t value){
	return SET_VALUE_FIELD(flags, value);
}

static inline uint32_t
is_value_invalid(uint32_t flags){
	return IS_VALUE_INVALID(flags);
}

static inline uint32_t
is_value_valid(uint32_t flags){
	return IS_VALUE_VALID(flags);
}

#define CLASS_MASK        0x0000F000
#define CLASS_UNKNOWN     0x00000000
#define CLASS_CODE        0x00001000
#define CLASS_DATA        0x00002000
#define CLASS_TAIL        0x00003000

#define CLASS_FIELD(A)		(A & CLASS_MASK)
#define SET_CLASS_FIELD(A,B)	((A & ~CLASS_MASK) | B)
#define TEST_CLASS_FIELD(A,B)	(CLASS_FIELD(A) == B)
#define IS_CLASS_UNKNOWN(A)	(TEST_CLASS_FIELD(A, CLASS_UNKNOWN))
#define IS_CLASS_CODE(A)	(TEST_CLASS_FIELD(A, CLASS_CODE))
#define IS_CLASS_DATA(A)	(TEST_CLASS_FIELD(A, CLASS_DATA))
#define IS_CLASS_TAIL(A)	(TEST_CLASS_FIELD(A, CLASS_TAIL))

static inline uint32_t
get_class_field(uint32_t flags){
	return CLASS_FIELD(flags);
}

static inline uint32_t
set_class_field(uint32_t flags, uint32_t value){
	return SET_CLASS_FIELD(flags, value);
}

static inline int
is_class_unknown(uint32_t flags){
	return IS_CLASS_UNKNOWN(flags);
}

static inline int
is_class_code(uint32_t flags){
	return IS_CLASS_CODE(flags);
}

static inline int
is_class_data(uint32_t flags){
	return IS_CLASS_DATA(flags);
}

static inline int
is_class_tail(uint32_t flags){
	return IS_CLASS_TAIL(flags);
}

#define DATATYPE_MASK     0x000F0000
#define DATATYPE_BYTE     0x00000000
#define DATATYPE_WORD     0x00010000
#define DATATYPE_DWORD    0x00020000
#define DATATYPE_QWORD    0x00030000

#define DATATYPE_FIELD(A)	(A & DATATYPE_MASK)
#define SET_DATATYPE_FIELD(A,B)	((A & ~DATATYPE_MASK) | B)
#define TEST_DATATYPE_FIELD(A,B)(DATATYPE_FIELD(A) == B)
#define IS_DATATYPE_BYTE(A)	(TEST_DATATYPE_FIELD(A,DATATYPE_BYTE))
#define IS_DATATYPE_WORD(A)	(TEST_DATATYPE_FIELD(A,DATATYPE_WORD))
#define IS_DATATYPE_DWORD(A)	(TEST_DATATYPE_FIELD(A,DATATYPE_DWORD))
#define IS_DATATYPE_QWORD(A)	(TEST_DATATYPE_FIELD(A,DATATYPE_QWORD))

static inline uint32_t
get_datatype_field(uint32_t flags){
	return DATATYPE_FIELD(flags);
}

static inline uint32_t
set_datatype_field(uint32_t flags, uint32_t value){
	return SET_DATATYPE_FIELD(flags, value);
}

static inline int
is_datatype_byte(uint32_t flags){
	return get_datatype_field(flags) == DATATYPE_BYTE;
}

static inline int
is_datatype_word(uint32_t flags){
	return get_datatype_field(flags) == DATATYPE_WORD;
}

static inline int
is_datatype_dword(uint32_t flags){
	return get_datatype_field(flags) == DATATYPE_DWORD;
}

static inline int
is_datatype_qword(uint32_t flags){
	return get_datatype_field(flags) == DATATYPE_QWORD;
}

#endif
