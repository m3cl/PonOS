#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdarg.h>

void printk(const char* format, ...);  // вместо printf
int strcmp(const char* s1, const char* s2);
void strcpy_s(char* dest, const char* src);  // вместо strcpy

#endif
