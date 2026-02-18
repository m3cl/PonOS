#include <stdint.h>
#include "../lib/stdio.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// ПЕРЕНЕСИ inb/outb В НАЧАЛО ФАЙЛА!
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void outb(unsigned short port, unsigned char data) {
    __asm__ volatile ("outb %0, %1" : : "a" (data), "Nd" (port));
}

// Остальной код как был...
static char scancode_to_ascii[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
    0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char input_buffer[256];
static int buffer_pos = 0;
static int buffer_ready = 0;

unsigned char keyboard_read_scancode(void) {
    while (1) {
        if (inb(KEYBOARD_STATUS_PORT) & 0x01) {
            return inb(KEYBOARD_DATA_PORT);
        }
    }
}

void keyboard_handler(void) {
    unsigned char scancode = keyboard_read_scancode();
    
    if (scancode < 0x80) {
        char c = scancode_to_ascii[scancode];
        
        if (c == '\n') {
            input_buffer[buffer_pos] = '\0';
            buffer_ready = 1;
            buffer_pos = 0;
            printf("\n");
        } else if (c == '\b') {
            if (buffer_pos > 0) {
                buffer_pos--;
                printf("\b \b");
            }
        } else if (c && buffer_pos < 255) {
            input_buffer[buffer_pos++] = c;
            printf("%c", c);
        }
    }
}

int has_input(void) {
    return buffer_ready;
}

int read_input(char* buffer, int max_len) {
    if (!buffer_ready) return 0;
    
    int len = 0;
    while (len < max_len - 1 && input_buffer[len]) {
        buffer[len] = input_buffer[len];
        len++;
    }
    buffer[len] = '\0';
    
    buffer_ready = 0;
    return 1;
}
