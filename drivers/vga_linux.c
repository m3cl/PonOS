#include <stdio.h>
#include <string.h>

static char screen_buffer[2000];
static int cursor_pos = 0;

void terminal_initialize(void) {
    memset(screen_buffer, ' ', sizeof(screen_buffer));
    cursor_pos = 0;
    printf("\033[2J\033[H"); // Clear screen and move cursor to home
}

void terminal_putchar(char c) {
    if (c == '\n') {
        putchar('\n');
        fflush(stdout);
    } else {
        putchar(c);
        fflush(stdout);
    }
}

void terminal_write(const char* str, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(str[i]);
    }
}
