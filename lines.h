/*
 * Copyright 2014 Joseph Landry
 */

#ifndef LINES_H
#define LINES_H

#include <stdint.h>

struct workspace;

int line_for_unknown(struct workspace *ws, uint64_t addr, char *buff, size_t size);

int line_for_code_item(struct workspace *ws, uint64_t addr, char *buf, size_t size);
int line_for_data_byte(struct workspace *ws, uint64_t addr, char *buf, size_t size);
int line_for_data_word(struct workspace *ws, uint64_t addr, char *buf, size_t size);
int line_for_data_dword(struct workspace *ws, uint64_t addr, char *buf, size_t size);
int line_for_data_qword(struct workspace *ws, uint64_t addr, char *buf, size_t size);
int line_for_item(struct workspace *ws, uint64_t itemaddr, char *buf, size_t size);

#endif
