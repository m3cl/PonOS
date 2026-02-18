#ifndef VGA_H
#define VGA_H

#include <stddef.h>  // ДОБАВИТЬ ЭТУ СТРОЧКУ!

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* str, size_t size);

#endif
