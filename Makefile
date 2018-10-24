# Makefile for building multi-file application
# by Brian Fraser

# final executable to build:
TARGET= myWordCount
# .o files to link in (for all .c files)
OBJS = myWordCount.o

# Add support for pthread and real-time clock here:
CFLAGS =  -g -pthread -lrt -std=c99 -Werror -Wall -fopenmp -D_POSIX_C_SOURCE=199309L


# You should not need to change any of this:
CC_C = gcc

all: clean $(TARGET)

%.o : %.c
	$(CC) -c $(CFLAGS) $<

$(TARGET): $(OBJS)
	$(CC_C) $(CFLAGS) $(OBJS) -o $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)