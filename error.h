/*
 * Copyright 2014 Joseph Landry
 */

#ifndef ERROR_H
#define ERROR_H

extern int dis_errno;

enum error_numbers {
	DER_OK = 0,
	DER_SYS,
	DER_BOUNDS,
	DER_INVADDR,
	DER_INVVALUE,
	DER_INVPARAM,
	DER_NOTUNKNOWN,
	DER_NOTFOUND,
	DER_NERRORS,
};

extern const char *dis_errstr[];

#endif
