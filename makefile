# Makefile

# Compiler
CC := gcc
# Compiler flags
CFLAGS := -Wall -Wextra -pedantic -std=c11
# Source files
SRCS := src\func.c
# Header files
HDRS := include\func.h
# Object files
OBJS := $(SRCS:.c=.o)
# Target
TARGET := mfrt

# Targets
all: $(OBJS)
	$(CC) $(CFLAGS) -o bin\$(TARGET) src\$(TARGET).c $(OBJS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean: $(OBJS)
	del $(OBJS)
	del temp\src.c
	del temp\src.frt
	
remove: clean
	del bin\$(TARGET).exe
