#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <stdint.h>

// Базовые функции ФС
void fs_init(void);
void fs_shutdown(void);

// Команды ФС
void fs_list_directory(const char* path);
void fs_read_file(const char* filename);
int fs_create_file(const char* name, const char* data);
int fs_create_directory(const char* dirname);
int fs_delete_file(const char* filename);
int fs_change_directory(const char* path);
void fs_print_working_directory(void);

// Информация
int fs_get_used_slots(void);
int fs_get_total_slots(void);
int fs_get_current_dir_file_count(void);

// Пользователи и права
void fs_whoami(void);
int fs_chmod(const char* filename, uint16_t mode);

// Персистентность
void fs_save_command(void);  // ДОБАВИТЬ ЭТУ СТРОЧКУ

#endif
