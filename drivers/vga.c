#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
size_t terminal_row = 0;
size_t terminal_column = 0;
uint8_t terminal_color = 0x0F; // белый на черном

void terminal_initialize(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = (uint16_t)' ' | (uint16_t)terminal_color << 8;
        }
    }
    terminal_row = 0;
    terminal_column = 0;
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
        return;
    }
    
    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    vga_buffer[index] = (uint16_t)c | (uint16_t)terminal_color << 8;
    
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}

void terminal_write(const char* str, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(str[i]);
    }
}
