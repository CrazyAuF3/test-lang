CC=gcc

CFLAGS=-std=c11 -Wall -Wextra -pedantic

TARGET=test_lexer

OBJS=test_lexer.o lexer.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)