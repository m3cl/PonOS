CC = gcc
CFLAGS = -I. -O0 -fno-stack-protector -D_LINUX_BUILD
LDFLAGS = 
SOURCES = \
    kernel/kernel.c \
    drivers/vga_linux.c \
    drivers/keyboard_linux.c \
    lib/stdio.c \
    lib/string.c \
    lib/stub.c \
    cli/cli.c \
    fs/fs.c \
    fs/disk.c

OBJS = $(SOURCES:.c=.o)
TARGET = ponos.bin

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Для отладки
debug: CFLAGS += -g
debug: $(TARGET)

.PHONY: all clean run debug
