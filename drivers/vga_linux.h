#ifndef VGA_LINUX_H
#define VGA_LINUX_H

#include <stddef.h>

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* str, size_t size);

#endif
