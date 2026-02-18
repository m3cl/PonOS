#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

static struct termios old_termios;
static char input_line[256];
static int cursor_pos = 0;

void keyboard_init(void) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON); // Оставляем ECHO включённым!
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void keyboard_restore(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

void keyboard_handler(void) {
    // Ничего не делаем - эхо теперь системное
}

int has_input(void) {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

int read_input(char* buffer, int max_len) {
    printf("> ");
    fflush(stdout);
    
    if (fgets(buffer, max_len, stdin)) {
        // Убираем \n в конце
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        return 1;
    }
    return 0;
}
