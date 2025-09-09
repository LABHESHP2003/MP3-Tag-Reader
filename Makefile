CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -D_GNU_SOURCE
TARGET  := mp3tagreader
SRCS    := error_handling.c id3_reader.c id3_utils.c id3_writer.c main.c
OBJS    := $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
