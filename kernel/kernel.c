#include "../drivers/vga_linux.h"
#include "../drivers/keyboard_linux.h"
#include "../fs/fs.h"
#include "../cli/cli.h"
#include "../lib/stdio.h"
#include "../lib/string.h"
#include <stdio.h>

void kernel_early(void) {
    terminal_initialize();
    keyboard_init();
    fs_init();
}

int main(void) {
    kernel_early();
    
    printk("=== ponos 3.5.5 ===\n");
    printk("type 'help' for commands\n");
    
    while(1) {
        // Прямой вызов без сложной логики
        cli_process_input();
    }
    
    keyboard_restore();
    return 0;
}
