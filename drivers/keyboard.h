#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stddef.h>  // ДОБАВИТЬ!

void keyboard_handler(void);
int has_input(void);
int read_input(char* buffer, int max_len);

#endif
