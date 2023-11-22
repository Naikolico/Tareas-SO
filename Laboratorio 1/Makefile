TARGET = t1
TARGET_OP = t1op

CC = gcc
CFLAGS = -Wall -Wextra

SRC_T1 = T1SO.c
SRC_T1OP = T1optimo.c

OBJ_T1 = $(SRC_T1:.c=.o)
OBJ_T1OP = $(SRC_T1OP:.c=.o)

all: $(TARGET) $(TARGET_OP)

$(TARGET): $(OBJ_T1)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_OP): $(OBJ_T1OP)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_T1) $(OBJ_T1OP) $(TARGET) $(TARGET_OP)

run_t1: $(TARGET)
	./$(TARGET)

run_t1op: $(TARGET_OP)
	./$(TARGET_OP)

.PHONY: clean run_t1 run_t1op