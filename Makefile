# Copyright 2026 The MIND Contributors
# Licensed under the Apache License, Version 2.0

CC ?= cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -fno-fast-math -O2
LDFLAGS = -lm

BUILD_DIR = build

#=============================================================================
# FOUNDATION (Layer 0) - Pure math, never changes
#=============================================================================

FOUNDATION_SRC = foundation/src/mind_vec.c
FOUNDATION_INC = foundation/include
FOUNDATION_OBJ = $(BUILD_DIR)/foundation/mind_vec.o
FOUNDATION_LIB = $(BUILD_DIR)/libmind_foundation.a

#=============================================================================
# CORE (Layer 1) - Cognitive memory
#=============================================================================

CORE_SRC = core/src/cr_runtime.c \
           core/src/cr_state.c \
           core/src/cr_query.c \
           core/src/cr_temporal.c \
           core/src/cr_persist.c

CORE_INC = core/include
CORE_OBJ = $(patsubst core/src/%.c,$(BUILD_DIR)/core/%.o,$(CORE_SRC))
CORE_LIB = $(BUILD_DIR)/libmind_core.a

#=============================================================================
# COMBINED LIBRARY
#=============================================================================

MIND_LIB = $(BUILD_DIR)/libmind.a

# Shared library (for Python ctypes)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    SHARED_EXT = dylib
    SHARED_FLAGS = -dynamiclib -install_name @rpath/libmind.dylib
else
    SHARED_EXT = so
    SHARED_FLAGS = -shared -fPIC
endif
MIND_SHARED = $(BUILD_DIR)/libmind.$(SHARED_EXT)

#=============================================================================
# TARGETS
#=============================================================================

.PHONY: all clean foundation core shared example test install python-test

all: $(MIND_LIB)

# Create build directories
$(BUILD_DIR)/foundation:
	mkdir -p $@

$(BUILD_DIR)/core:
	mkdir -p $@

#-----------------------------------------------------------------------------
# Foundation
#-----------------------------------------------------------------------------

foundation: $(FOUNDATION_LIB)

$(BUILD_DIR)/foundation/%.o: foundation/src/%.c | $(BUILD_DIR)/foundation
	$(CC) $(CFLAGS) -I$(FOUNDATION_INC) -c $< -o $@

$(FOUNDATION_LIB): $(FOUNDATION_OBJ)
	ar rcs $@ $^

#-----------------------------------------------------------------------------
# Core (depends on Foundation)
#-----------------------------------------------------------------------------

core: $(CORE_LIB)

$(BUILD_DIR)/core/%.o: core/src/%.c | $(BUILD_DIR)/core
	$(CC) $(CFLAGS) -I$(CORE_INC) -I$(FOUNDATION_INC) -c $< -o $@

$(CORE_LIB): $(CORE_OBJ)
	ar rcs $@ $^

#-----------------------------------------------------------------------------
# Combined library (static)
#-----------------------------------------------------------------------------

$(MIND_LIB): $(FOUNDATION_LIB) $(CORE_LIB)
	rm -f $@
	ar rcs $@ $(FOUNDATION_OBJ) $(CORE_OBJ)

#-----------------------------------------------------------------------------
# Shared library (for Python/FFI)
#-----------------------------------------------------------------------------

FOUNDATION_OBJ_PIC = $(BUILD_DIR)/foundation/mind_vec.pic.o
CORE_OBJ_PIC = $(patsubst core/src/%.c,$(BUILD_DIR)/core/%.pic.o,$(CORE_SRC))

shared: $(MIND_SHARED)

$(BUILD_DIR)/foundation/%.pic.o: foundation/src/%.c | $(BUILD_DIR)/foundation
	$(CC) $(CFLAGS) -fPIC -I$(FOUNDATION_INC) -c $< -o $@

$(BUILD_DIR)/core/%.pic.o: core/src/%.c | $(BUILD_DIR)/core
	$(CC) $(CFLAGS) -fPIC -I$(CORE_INC) -I$(FOUNDATION_INC) -c $< -o $@

$(MIND_SHARED): $(FOUNDATION_OBJ_PIC) $(CORE_OBJ_PIC)
	$(CC) $(SHARED_FLAGS) -o $@ $^ $(LDFLAGS)

#-----------------------------------------------------------------------------
# Example
#-----------------------------------------------------------------------------

example: $(BUILD_DIR)/mind_example
	./$(BUILD_DIR)/mind_example

$(BUILD_DIR)/mind_example: examples/minimal.c $(MIND_LIB)
	$(CC) $(CFLAGS) -I$(CORE_INC) -I$(FOUNDATION_INC) $< -L$(BUILD_DIR) -lmind $(LDFLAGS) -o $@

#-----------------------------------------------------------------------------
# Tests
#-----------------------------------------------------------------------------

test: $(BUILD_DIR)/test_basic
	./$(BUILD_DIR)/test_basic

$(BUILD_DIR)/test_basic: tests/test_basic.c $(MIND_LIB)
	$(CC) $(CFLAGS) -I$(CORE_INC) -I$(FOUNDATION_INC) $< -L$(BUILD_DIR) -lmind $(LDFLAGS) -o $@

#-----------------------------------------------------------------------------
# Python tests (requires shared library)
#-----------------------------------------------------------------------------

python-test: $(MIND_SHARED)
	MIND_LIB_PATH=$(CURDIR)/$(MIND_SHARED) PYTHONPATH=external/bindings/python python3 -m mind.tests.test_basic

#-----------------------------------------------------------------------------
# Clean
#-----------------------------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR)

#-----------------------------------------------------------------------------
# Install
#-----------------------------------------------------------------------------

PREFIX ?= /usr/local

install: $(MIND_LIB)
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/include/mind
	install -m 644 $(MIND_LIB) $(PREFIX)/lib/
	install -m 644 $(FOUNDATION_INC)/mind_vec.h $(PREFIX)/include/mind/
	install -m 644 $(CORE_INC)/cr.h $(PREFIX)/include/mind/
