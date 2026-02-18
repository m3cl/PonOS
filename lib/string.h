#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char* str);
void* memset(void* ptr, int value, size_t num);
int strcmp(const char* s1, const char* s2);

// Макрос для совместимости
#define strcpy_s(dest, src) strcpy(dest, src)

#endif
