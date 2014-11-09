/*
 * Copyright 2014 Joseph Landry
 */

#include "errno.h"

int dis_errno;

const char *dis_errstr[] = {
	"No error",
	"System error (libc or below; check libc errno)",
	"Bounds checking error",
	"Invalid address",
	"Byte doesn't have a value",
	"Invalid parameter value",
	"An operation requiring unclassified bytes was attempted on a known region",
	"item not found",
};
