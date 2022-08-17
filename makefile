CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c17
INCLUDE = -Iinclude
LIBS = 

SRC = $(wildcard src/*.c) $(wildcard src/**/*.c)
OBJ = $(SRC:.c=.o)
BUILD = build

all: dirs hcs-server

dirs:
	mkdir -p $(BUILD) src include tests

run: hcs-server
	$(BUILD)/hcs-server

hcs-server: $(OBJ)
	$(CC) $? $(CFLAGS) $(INCLUDE) $(LIBS) -o $(BUILD)/$@

%.o: %.c
	$(CC) $< $(CFLAGS) $(INCLUDE) $(LIBS) -o $@ -c

clean:
	rm -Rf $(BUILD)/hcs-server $(OBJ)