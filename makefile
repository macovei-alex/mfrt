# Makefile

SHELL = cmd.exe
# Source directory
SRC_DIR := src
# Object directory
OBJ_DIR := obj
# Headers directory
HDR_DIR := include
# Binary directory
BIN_DIR := bin
# Source files
SRCS := $(wildcard $(SRC_DIR)/*.c)
# Header files
HDRS := $(wildcard $(HDR_DIR)/*.h)
# Object files
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
# Temporary directory
TEMP_DIR := temp
# Target
TARGET := $(BIN_DIR)/mfrt

# Compiler
CC := gcc
# Compiler flags
CFLAGS := -Wall -Wextra -pedantic -I$(HDR_DIR)

# Targets
all: target
	@echo Done!

target: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist $(OBJ_DIR)\*.o del /Q $(OBJ_DIR)\*.o
	@if exist $(TEMP_DIR)/src.c del /Q $(TEMP_DIR)/src.c
	@if exist $(TEMP_DIR)/src.frt del /Q $(TEMP_DIR)/src.frt
	
remove: clean
	@if exist $(TARGET) del /Q $(TARGET)
