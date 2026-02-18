#include <stdio.h>  // Добавляем для fflush/stdout
#include "stdio.h"
#include "../drivers/vga_linux.h"

void printk(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    const char* p = format;
    while (*p) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 's': {
                    char* str = va_arg(args, char*);
                    while (*str) {
                        terminal_putchar(*str++);
                    }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    terminal_putchar(c);
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    if (num < 0) {
                        terminal_putchar('-');
                        num = -num;
                    }
                    
                    char buffer[32];
                    int i = 0;
                    do {
                        buffer[i++] = '0' + (num % 10);
                        num /= 10;
                    } while (num > 0);
                    
                    while (--i >= 0) {
                        terminal_putchar(buffer[i]);
                    }
                    break;
                }
                case '%':
                    terminal_putchar('%');
                    break;
            }
        } else {
            terminal_putchar(*p);
        }
        p++;
    }
    
    va_end(args);
    fflush(stdout);  // Теперь stdout доступен
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void strcpy_s(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}
