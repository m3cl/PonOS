#ifndef DISK_H
#define DISK_H

int disk_init(void);
void disk_save(void);
void disk_cleanup(void);
void* disk_get_sector(int sector);
int disk_write_sector(int sector, const void* data);

#endif
