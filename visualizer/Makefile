CC = gcc
CFLAGS = -g -I "../common"
RM = rm -rf

OBJ_FILES = init.o
BUILD_DIR = build

default: all

all: $(OBJ_FILES)

$(OBJ_FILES): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(OBJ_FILES)
	$(RM) $(OBJ_FILES).dSYM