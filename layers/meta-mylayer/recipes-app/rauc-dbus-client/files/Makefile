# Makefile for RAUC GDBus Client

# Project name
PROJECT = rauc-gdbus-client
VERSION = 1.0

# Directories
SRCDIR = src
BUILDDIR = build
INSTALLDIR = /usr/bin

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TARGET = $(BUILDDIR)/$(PROJECT)

# Compiler and flags
CC ?= gcc
CFLAGS += -Wall -Wextra -std=c99
CFLAGS += $(shell pkg-config --cflags gio-2.0)
LDFLAGS += $(shell pkg-config --libs gio-2.0)

# Debug/Release flags
ifdef DEBUG
    CFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

# Default target
all: $(TARGET)

# Create build directory
$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS)
	@echo "LINK $@"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Install
install: $(TARGET)
	@echo "Installing $(PROJECT) to $(DESTDIR)$(INSTALLDIR)"
	@install -d $(DESTDIR)$(INSTALLDIR)
	@install -m 755 $(TARGET) $(DESTDIR)$(INSTALLDIR)/$(PROJECT)

# Clean
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILDDIR)

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@pkg-config --exists gio-2.0 || (echo "Error: gio-2.0 not found" && exit 1)
	@echo "All dependencies satisfied"

# Debug build
debug:
	$(MAKE) DEBUG=1

# Phony targets
.PHONY: all clean install check-deps help debug