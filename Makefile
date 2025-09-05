CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Source files
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/chatbot.c $(SRCDIR)/database_stub.c $(SRCDIR)/utils_stub.c $(SRCDIR)/intent_patterns.c $(SRCDIR)/enhanced_intent_patterns.c $(SRCDIR)/enhanced_response_generator.c
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/ingres_chatbot

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(OBJDIR) $(BINDIR)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ -lm

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Test compilation only
test-compile: directories $(OBJECTS)
	@echo "✅ Compilation successful!"

.PHONY: all clean run test-compile directories