# Makefile

SHELL = cmd.exe
# Base directory
BASE_DIR := ..
# Source directory
SRC_DIR := $(BASE_DIR)/src
# Object directory
OBJ_DIR := $(BASE_DIR)\obj
# Headers directory
HDR_DIR := $(BASE_DIR)/include
# Binary directory
BIN_DIR := $(BASE_DIR)/bin
# Source files
SRCS := $(wildcard $(SRC_DIR)/*.c)
# Header files
HDRS := $(wildcard $(HDR_DIR)/*.h)
# Object files
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
# Target
TARGET := $(BIN_DIR)/mfrt

# Compiler
CC := gcc
# Compiler flags
CFLAGS := -Wall -Wextra -pedantic -I$(HDR_DIR)

# Targets
all: target
	@echo "Done!"

target: $(OBJS)
	@echo $(BASE_DIR)
	@echo $(SRC_DIR)
	@echo $(OBJ_DIR)
	@echo $(HDR_DIR)
	@echo $(BIN_DIR)
	@echo $(SRCS)
	@echo $(OBJS)
	@echo $(HDRS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

../obj/%.o: ../src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist $(OBJ_DIR)\*.o del /Q $(OBJ_DIR)\*.o
	@if exist ..\temp\src.c del /Q ..\temp\src.c
	@if exist ..\temp\src.frt del /Q ..\temp\src.frt
	
remove: clean
	@if exist $(TARGET) del /Q $(TARGET)
