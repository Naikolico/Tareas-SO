TARGET = T2SO

CC = gcc
CFLAGS = -Wall -Wextra

SRC_T2SO = T2SO.c

OBJ_T2SO = $(SRC_T2SO:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ_T2SO)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_T2SO) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run