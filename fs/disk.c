#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/stdio.h"

#define DISK_SIZE (10 * 1024 * 1024) // 10MB
#define DISK_FILENAME "ponos.disk"

static char* virtual_disk = NULL;

int disk_init(void) {
    virtual_disk = malloc(DISK_SIZE);
    if (!virtual_disk) {
        printk("disk: failed to allocate memory\n");
        return 0;
    }
    memset(virtual_disk, 0, DISK_SIZE);
    
    // Пробуем загрузить существующий диск
    FILE* f = fopen(DISK_FILENAME, "rb");
    if (f) {
        fread(virtual_disk, 1, DISK_SIZE, f);
        fclose(f);
        printk("disk: loaded from %s\n", DISK_FILENAME);
        return 1;
    } else {
        printk("disk: created new virtual disk\n");
        return 1;
    }
}

void disk_save(void) {
    if (!virtual_disk) return;
    
    FILE* f = fopen(DISK_FILENAME, "wb");
    if (f) {
        fwrite(virtual_disk, 1, DISK_SIZE, f);
        fclose(f);
    }
}

void* disk_get_sector(int sector) {
    if (!virtual_disk || sector < 0 || sector >= DISK_SIZE / 512) return NULL;
    return virtual_disk + (sector * 512);
}

int disk_write_sector(int sector, const void* data) {
    void* dest = disk_get_sector(sector);
    if (!dest) return 0;
    memcpy(dest, data, 512);
    return 1;
}

void disk_cleanup(void) {
    if (virtual_disk) {
        free(virtual_disk);
        virtual_disk = NULL;
    }
}
