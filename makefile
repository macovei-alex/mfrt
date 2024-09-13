CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -Wpedantic

SRC = src
OBJ = obj
BIN = bin
TMP = temp

SRCS = $(SRC)/mfrt.c $(SRC)/func.c

OBJS = $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))

TARGET = $(BIN)/mfrt

INCLUDE = -Iinclude

all: target run

target: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -f $(OBJ)/*.o $(TMP)/src.c $(TMP)/src.frt
	
remove: clean
	rm -f $(TARGET)

run:
	$(TARGET) example.frt
	example
