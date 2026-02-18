#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/stdio.h"
#include "../lib/string.h"
#include "disk.h"
#include "fs.h"

#define MAX_FILES 256
#define MAX_FILE_SIZE 65536
#define MAX_FILENAME 32
#define MAX_PATH 128

// Системные UID/GID
#define ROOT_UID 0
#define ROOT_GID 0
#define DEFAULT_UID 1000

typedef enum {
    FILE_REGULAR,
    FILE_DIRECTORY
} file_type_t;

typedef struct {
    uint32_t uid;
    uint32_t gid;
    uint16_t permissions;
} file_meta_t;

typedef struct file_entry {
    char name[MAX_FILENAME];
    char data[MAX_FILE_SIZE];
    uint32_t size;
    file_type_t type;
    uint8_t used;
    file_meta_t meta;
    int parent_index;  // Храним индексы вместо указателей!
    int children[MAX_FILES];
    int child_count;
} file_entry_t;

static file_entry_t filesystem[MAX_FILES];
static int root_dir_index = 0;
static int current_dir_index = 0;
static int fs_initialized = 0;

// Прототипы
int find_free_entry(void);
int find_file_in_dir(int dir_index, const char* name);
int add_child_to_dir(int parent_index, int child_index);
void init_basic_fs(void);
void fs_save_state(void);

int find_free_entry(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!filesystem[i].used) {
            return i;
        }
    }
    return -1;
}

int find_file_in_dir(int dir_index, const char* name) {
    file_entry_t* dir = &filesystem[dir_index];
    for (int i = 0; i < dir->child_count; i++) {
        int child_index = dir->children[i];
        if (strcmp(filesystem[child_index].name, name) == 0) {
            return child_index;
        }
    }
    return -1;
}

int add_child_to_dir(int parent_index, int child_index) {
    file_entry_t* parent = &filesystem[parent_index];
    if (parent->child_count >= MAX_FILES) return 0;
    parent->children[parent->child_count++] = child_index;
    filesystem[child_index].parent_index = parent_index;
    return 1;
}

void init_basic_fs(void) {
    memset(filesystem, 0, sizeof(filesystem));
    
    // Создаём корневую директорию
    root_dir_index = find_free_entry();
    file_entry_t* root_dir = &filesystem[root_dir_index];
    strcpy(root_dir->name, "/");
    root_dir->type = FILE_DIRECTORY;
    root_dir->used = 1;
    root_dir->meta.uid = ROOT_UID;
    root_dir->meta.gid = ROOT_GID;
    root_dir->meta.permissions = 0755;
    root_dir->parent_index = root_dir_index; // сам на себя
    
    current_dir_index = root_dir_index;
    
    // Системные файлы
    int readme_index = find_free_entry();
    file_entry_t* readme = &filesystem[readme_index];
    strcpy(readme->name, "readme.txt");
    strcpy(readme->data, "welcome to ponos 3.5.5\nfiles persist between sessions!");
    readme->size = strlen(readme->data);
    readme->type = FILE_REGULAR;
    readme->used = 1;
    readme->meta.uid = ROOT_UID;
    readme->meta.gid = ROOT_GID;
    readme->meta.permissions = 0644;
    add_child_to_dir(root_dir_index, readme_index);
    
    int home_index = find_free_entry();
    file_entry_t* home = &filesystem[home_index];
    strcpy(home->name, "home");
    home->type = FILE_DIRECTORY;
    home->used = 1;
    home->meta.uid = ROOT_UID;
    home->meta.gid = ROOT_GID;
    home->meta.permissions = 0755;
    add_child_to_dir(root_dir_index, home_index);
    
    fs_initialized = 1;
}

void fs_save_state(void) {
    void* sector = disk_get_sector(0);
    if (sector) {
        // Сохраняем только индексы и данные
        memcpy(sector, &root_dir_index, sizeof(root_dir_index));
        memcpy(sector + sizeof(root_dir_index), &current_dir_index, sizeof(current_dir_index));
        memcpy(sector + sizeof(root_dir_index) + sizeof(current_dir_index), &fs_initialized, sizeof(fs_initialized));
        memcpy(sector + sizeof(root_dir_index) + sizeof(current_dir_index) + sizeof(fs_initialized), filesystem, sizeof(filesystem));
        
        disk_save();
    }
}

void fs_load_state(void) {
    void* sector = disk_get_sector(0);
    if (sector) {
        // Загружаем индексы и данные
        memcpy(&root_dir_index, sector, sizeof(root_dir_index));
        memcpy(&current_dir_index, sector + sizeof(root_dir_index), sizeof(current_dir_index));
        memcpy(&fs_initialized, sector + sizeof(root_dir_index) + sizeof(current_dir_index), sizeof(fs_initialized));
        memcpy(filesystem, sector + sizeof(root_dir_index) + sizeof(current_dir_index) + sizeof(fs_initialized), sizeof(filesystem));
        
        printk("fs: loaded from disk\n");
    }
}

void fs_init(void) {
    if (fs_initialized) return;
    
    if (!disk_init()) {
        printk("fs: failed to initialize disk\n");
        return;
    }
    
    // Всегда создаём новую ФС для теста
    init_basic_fs();
    printk("fs: created new filesystem\n");
}

void fs_shutdown(void) {
    fs_save_state();
    disk_cleanup();
    printk("fs: shutdown complete\n");
}

void fs_list_directory(const char* path) {
    int target_dir_index = current_dir_index;
    
    if (path && strcmp(path, "/") == 0) {
        target_dir_index = root_dir_index;
    } else if (path && strcmp(path, "..") == 0) {
        if (current_dir_index != root_dir_index) {
            target_dir_index = filesystem[current_dir_index].parent_index;
        }
    } else if (path) {
        int found_index = find_file_in_dir(current_dir_index, path);
        if (found_index != -1 && filesystem[found_index].type == FILE_DIRECTORY) {
            target_dir_index = found_index;
        } else if (found_index != -1) {
            printk("not a directory: %s\n", path);
            return;
        } else {
            printk("directory not found: %s\n", path);
            return;
        }
    }
    
    file_entry_t* target_dir = &filesystem[target_dir_index];
    printk("contents of %s:\n", target_dir->name);
    printk("type  name        size\n");
    printk("----------------------\n");
    
    int total_files = 0;
    int total_dirs = 0;
    
    for (int i = 0; i < target_dir->child_count; i++) {
        int child_index = target_dir->children[i];
        file_entry_t* entry = &filesystem[child_index];
        if (entry->type == FILE_DIRECTORY) {
            printk("dir   %s\n", entry->name);
            total_dirs++;
        } else {
            printk("file  %s %d bytes\n", entry->name, entry->size);
            total_files++;
        }
    }
    
    printk("----------------------\n");
    printk("total: %d files, %d directories\n", total_files, total_dirs);
}

void fs_read_file(const char* filename) {
    int file_index = find_file_in_dir(current_dir_index, filename);
    if (file_index == -1) {
        printk("file not found: %s\n", filename);
        return;
    }
    
    file_entry_t* file = &filesystem[file_index];
    if (file->type == FILE_DIRECTORY) {
        printk("is a directory: %s\n", filename);
        return;
    }
    
    printk("=== %s (%d bytes) ===\n", filename, file->size);
    printk("%s\n", file->data);
    printk("=== end of file ===\n");
}

int fs_create_file(const char* filename, const char* data) {
    if (find_file_in_dir(current_dir_index, filename) != -1) {
        printk("file already exists: %s\n", filename);
        return 0;
    }
    
    int new_file_index = find_free_entry();
    if (new_file_index == -1) {
        printk("no free file slots\n");
        return 0;
    }
    
    file_entry_t* new_file = &filesystem[new_file_index];
    strcpy(new_file->name, filename);
    if (data) {
        strcpy(new_file->data, data);
        new_file->size = strlen(data);
    } else {
        new_file->data[0] = '\0';
        new_file->size = 0;
    }
    new_file->type = FILE_REGULAR;
    new_file->used = 1;
    new_file->meta.uid = DEFAULT_UID;
    new_file->meta.gid = DEFAULT_UID;
    new_file->meta.permissions = 0644;
    
    if (!add_child_to_dir(current_dir_index, new_file_index)) {
        printk("directory is full\n");
        return 0;
    }
    
    fs_save_state();
    printk("created file: %s\n", filename);
    return 1;
}

int fs_create_directory(const char* dirname) {
    if (find_file_in_dir(current_dir_index, dirname) != -1) {
        printk("directory already exists: %s\n", dirname);
        return 0;
    }
    
    int new_dir_index = find_free_entry();
    if (new_dir_index == -1) {
        printk("no free file slots\n");
        return 0;
    }
    
    file_entry_t* new_dir = &filesystem[new_dir_index];
    strcpy(new_dir->name, dirname);
    new_dir->type = FILE_DIRECTORY;
    new_dir->used = 1;
    new_dir->meta.uid = DEFAULT_UID;
    new_dir->meta.gid = DEFAULT_UID;
    new_dir->meta.permissions = 0755;
    new_dir->child_count = 0;
    
    if (!add_child_to_dir(current_dir_index, new_dir_index)) {
        printk("directory is full\n");
        return 0;
    }
    
    fs_save_state();
    printk("created directory: %s\n", dirname);
    return 1;
}

int fs_delete_file(const char* filename) {
    int file_index = find_file_in_dir(current_dir_index, filename);
    if (file_index == -1) {
        printk("file not found: %s\n", filename);
        return 0;
    }
    
    file_entry_t* file = &filesystem[file_index];
    if (file->type == FILE_DIRECTORY && file->child_count > 0) {
        printk("directory not empty: %s\n", filename);
        return 0;
    }
    
    // Удаляем из родительской директории
    int parent_index = file->parent_index;
    file_entry_t* parent = &filesystem[parent_index];
    for (int i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == file_index) {
            for (int j = i; j < parent->child_count - 1; j++) {
                parent->children[j] = parent->children[j + 1];
            }
            parent->child_count--;
            break;
        }
    }
    
    memset(file, 0, sizeof(file_entry_t));
    
    fs_save_state();
    printk("deleted: %s\n", filename);
    return 1;
}

int fs_change_directory(const char* path) {
    if (!path || strcmp(path, "") == 0) {
        current_dir_index = root_dir_index;
    } else if (strcmp(path, "/") == 0) {
        current_dir_index = root_dir_index;
    } else if (strcmp(path, "..") == 0) {
        if (current_dir_index != root_dir_index) {
            current_dir_index = filesystem[current_dir_index].parent_index;
        }
    } else if (strcmp(path, ".") == 0) {
        // stay in current directory
    } else {
        int target_index = find_file_in_dir(current_dir_index, path);
        if (target_index == -1) {
            printk("directory not found: %s\n", path);
            return 0;
        }
        if (filesystem[target_index].type != FILE_DIRECTORY) {
            printk("not a directory: %s\n", path);
            return 0;
        }
        current_dir_index = target_index;
    }
    return 1;
}

void fs_print_working_directory(void) {
    char path[MAX_PATH] = "";
    int dir_index = current_dir_index;
    
    while (dir_index != root_dir_index) {
        char temp[MAX_PATH];
        strcpy(temp, "/");
        strcpy(temp + 1, filesystem[dir_index].name);
        strcpy(temp + strlen(temp), path);
        strcpy(path, temp);
        dir_index = filesystem[dir_index].parent_index;
    }
    
    if (strlen(path) == 0) {
        strcpy(path, "/");
    }
    
    printk("%s\n", path);
}

int fs_get_used_slots(void) {
    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used) {
            count++;
        }
    }
    return count;
}

int fs_get_total_slots(void) {
    return MAX_FILES;
}

int fs_get_current_dir_file_count(void) {
    return filesystem[current_dir_index].child_count;
}

void fs_whoami(void) {
    printk("user: user (uid=1000, gid=1000)\n");
}

int fs_chmod(const char* filename, uint16_t mode) {
    int file_index = find_file_in_dir(current_dir_index, filename);
    if (file_index == -1) {
        printk("file not found: %s\n", filename);
        return 0;
    }
    
    filesystem[file_index].meta.permissions = mode & 0777;
    
    fs_save_state();
    printk("changed mode of %s to %o\n", filename, mode);
    return 1;
}

void fs_save_command(void) {
    fs_save_state();
    printk("filesystem state saved\n");
}
