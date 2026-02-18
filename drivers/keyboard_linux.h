#ifndef KEYBOARD_LINUX_H
#define KEYBOARD_LINUX_H

void keyboard_init(void);
void keyboard_restore(void);
void keyboard_handler(void);
int has_input(void);
int read_input(char* buffer, int max_len);

#endif
