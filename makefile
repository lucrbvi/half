# Compiler settings
CXX = clang++
CC = clang
CXXFLAGS = -Wall -Wextra
CFLAGS = -Wall -Wextra

# Directories
SRC_DIR = .
DIST_DIR = ./dist
OBJ_DIR = $(DIST_DIR)

# Source files
LIB_SRC = libhalf.cpp
TEST_SRC = test.c
MAIN_SRC = main.cpp

# Object files
LIB_OBJ = $(OBJ_DIR)/libhalf.o

# Output binaries
TEST_BIN = $(DIST_DIR)/test
MAIN_BIN = $(DIST_DIR)/main

# Default target
all: $(DIST_DIR) $(TEST_BIN) $(MAIN_BIN)

# Create dist directory
$(DIST_DIR):
	mkdir -p $(DIST_DIR)

# Compile libhalf.cpp to object file
$(LIB_OBJ): $(LIB_SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/$(LIB_SRC) -o $@

# Build test binary (C program using C++ library)
$(TEST_BIN): $(TEST_SRC) $(LIB_OBJ) | $(DIST_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/$(TEST_SRC) $(LIB_OBJ) -o $@ $(LDFLAGS) -lstdc++

# Build main binary (C++ program)
$(MAIN_BIN): $(MAIN_SRC) $(LIB_OBJ) | $(DIST_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_DIR)/$(MAIN_SRC) $(LIB_OBJ) -o $@ $(LDFLAGS)

# Clean build artifacts
clean:
	rm -rf $(DIST_DIR)

# Rebuild everything
rebuild: clean all

# Install (copy binaries to a standard location)
install: all
	@echo "Binaries installed in $(DIST_DIR)/"
	@echo "  - test"
	@echo "  - main"

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Build all binaries (default)"
	@echo "  clean    - Remove all build artifacts"
	@echo "  rebuild  - Clean and rebuild everything"
	@echo "  install  - Show installation location"
	@echo "  help     - Show this help message"

.PHONY: all clean rebuild install help
