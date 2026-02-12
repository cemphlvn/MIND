# Copyright 2026 The MIND Contributors
# Licensed under the Apache License, Version 2.0

CC ?= cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -fno-fast-math -O2
LDFLAGS = -lm

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
EXAMPLES_DIR = examples
TESTS_DIR = tests

# Source files
SRCS = $(SRC_DIR)/cr_runtime.c \
       $(SRC_DIR)/cr_state.c \
       $(SRC_DIR)/cr_query.c \
       $(SRC_DIR)/cr_temporal.c \
       $(SRC_DIR)/cr_persist.c

OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Library
LIB = $(BUILD_DIR)/libmind.a

# Targets
.PHONY: all clean example test

all: $(LIB)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -I$(SRC_DIR) -c $< -o $@

$(LIB): $(OBJS)
	ar rcs $@ $^

# Example
example: $(BUILD_DIR)/mind_example
	./$(BUILD_DIR)/mind_example

$(BUILD_DIR)/mind_example: $(EXAMPLES_DIR)/minimal.c $(LIB)
	$(CC) $(CFLAGS) -I$(INC_DIR) $< -L$(BUILD_DIR) -lmind $(LDFLAGS) -o $@

# Tests
test: $(BUILD_DIR)/test_basic
	./$(BUILD_DIR)/test_basic

$(BUILD_DIR)/test_basic: $(TESTS_DIR)/test_basic.c $(LIB)
	$(CC) $(CFLAGS) -I$(INC_DIR) $< -L$(BUILD_DIR) -lmind $(LDFLAGS) -o $@

clean:
	rm -rf $(BUILD_DIR)

# Install (prefix defaults to /usr/local)
PREFIX ?= /usr/local

install: $(LIB)
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/include
	install -m 644 $(LIB) $(PREFIX)/lib/
	install -m 644 $(INC_DIR)/cr.h $(PREFIX)/include/
