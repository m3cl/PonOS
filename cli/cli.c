#include "../drivers/vga_linux.h"
#include "../drivers/keyboard_linux.h"
#include "../fs/fs.h"
#include "../lib/stdio.h"
#include "../lib/string.h"
#include <stdlib.h>
#include <string.h>

int parse_command(char* cmd, char** args) {
    int argc = 0;
    char* token = cmd;
    
    while (*cmd && argc < 10) {
        if (*cmd == ' ') {
            *cmd = '\0';
            if (token[0] != '\0') {
                args[argc++] = token;
            }
            token = cmd + 1;
        }
        cmd++;
    }
    
    if (token[0] != '\0') {
        args[argc++] = token;
    }
    
    return argc;
}

void show_memory_info(void) {
    printk("memory info:\n");
    printk("  process size: ~2mb\n");
    printk("  heap: 1.5mb\n"); 
    printk("  stack: 512kb\n");
    printk("  filesystem cache: 32kb\n");
}

void show_disk_info(void) {
    int used = fs_get_used_slots();
    int total = fs_get_total_slots();
    int free = total - used;
    
    printk("disk info:\n");
    printk("  total files: %d slots\n", total);
    printk("  used: %d files\n", used);
    printk("  free: %d slots\n", free);
    printk("  current dir: %d items\n", fs_get_current_dir_file_count());
}

void execute_command(char* cmd) {
    if (strlen(cmd) == 0) return;
    
    char* args[10];
    int argc = parse_command(cmd, args);
    
    if (strcmp(args[0], "help") == 0) {
        printk("ponos commands:\n");
        printk("  help          - show this help\n");
        printk("  ls [dir]      - list directory contents\n");
        printk("  cat <file>    - display file content\n");
        printk("  touch <file>  - create empty file\n");
        printk("  mkdir <dir>   - create directory\n");
        printk("  rm <file>     - delete file/directory\n");
        printk("  cd <dir>      - change directory\n");
        printk("  pwd           - show current directory\n");
        printk("  mem           - show memory info\n");
        printk("  disk          - show disk info\n");
        printk("  whoami        - show current user\n");
        printk("  chmod mode file - change permissions\n");
        printk("  save          - save disk state\n");
        printk("  clear         - clear screen\n");
        printk("  exit          - exit ponos\n");
    }
    else if (strcmp(args[0], "ls") == 0) {
        fs_list_directory(argc > 1 ? args[1] : NULL);
    }
    else if (strcmp(args[0], "cat") == 0) {
        if (argc > 1) fs_read_file(args[1]);
        else printk("usage: cat file\n");
    }
    else if (strcmp(args[0], "touch") == 0) {
        if (argc > 1) fs_create_file(args[1], "");
        else printk("usage: touch file\n");
    }
    else if (strcmp(args[0], "mkdir") == 0) {
        if (argc > 1) fs_create_directory(args[1]);
        else printk("usage: mkdir name\n");
    }
    else if (strcmp(args[0], "rm") == 0) {
        if (argc > 1) fs_delete_file(args[1]);
        else printk("usage: rm name\n");
    }
    else if (strcmp(args[0], "cd") == 0) {
        if (argc > 1) fs_change_directory(args[1]);
        else fs_change_directory("/");
    }
    else if (strcmp(args[0], "pwd") == 0) {
        fs_print_working_directory();
    }
    else if (strcmp(args[0], "mem") == 0) {
        show_memory_info();
    }
    else if (strcmp(args[0], "disk") == 0) {
        show_disk_info();
    }
    else if (strcmp(args[0], "whoami") == 0) {
        fs_whoami();
    }
    else if (strcmp(args[0], "chmod") == 0) {
        if (argc > 2) {
            int mode = strtol(args[1], NULL, 8);
            fs_chmod(args[2], mode);
        } else {
            printk("usage: chmod mode file\n");
        }
    }
    else if (strcmp(args[0], "save") == 0) {
        fs_save_command();
    }
    else if (strcmp(args[0], "clear") == 0) {
        terminal_initialize();
        printk("=== ponos 3.5.5 ===\n");
    }
    else if (strcmp(args[0], "exit") == 0) {
        fs_shutdown();
        printk("goodbye!\n");
        exit(0);
    }
    else {
        printk("unknown command: %s\n", args[0]);
    }
}

void cli_process_input(void) {
    char input[256];
    if (read_input(input, sizeof(input))) {
        execute_command(input);
        printk("> ");
    }
}
