# Makefile

# Compiler
CC := g++
# Compiler flags
CFLAGS := -Wall -Wextra -pedantic
# Source files
SRCS := ..\src\func.cpp
# Header files
HDRS := ..\include\funcpp.h
# Object files
OBJS := $(SRCS:.c=.o)
# Target
TARGET := mfrt

# Targets
all: $(OBJS)
	$(CC) $(CFLAGS) -o ..\bin\$(TARGET) ..\src\$(TARGET).c $(OBJS) $(HDRS)
	make clean

temp\%.o: src\%.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean: $(OBJS)
	del ..\src\$(OBJS)
	del ..\temp\src.c
	del ..\temp\src.frt
	
remove: clean
	del ..\bin\$(TARGET).exe
