CC = gcc
CFLAGS = -Wall -Werror -std=c11 -O2 -D_GNU_SOURCE
LDFLAGS = -lSDL3 -lm

SRC_DIR = src
LIB_DIR = lib
BIN_DIR = bin

# Graphics library paths
GRAPHICS_SRC = $(LIB_DIR)/graphics/src
GRAPHICS_INC = $(LIB_DIR)/graphics/include

GEOMETRY_SRC = $(LIB_DIR)/geometry/src
GEOMETRY_INC = $(LIB_DIR)/geometry/include

# Include paths
INCLUDES = -I$(GRAPHICS_INC) \
           -I$(GRAPHICS_SRC) \
					 -I$(GEOMETRY_INC) \
           -I$(GEOMETRY_SRC) \
           -I$(LIB_DIR)/utils \
           -I$(LIB_DIR)

TARGET = $(BIN_DIR)/engine

all: directories $(TARGET)

directories:
	@mkdir -p $(BIN_DIR)

$(TARGET): $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -rf $(BIN_DIR)

rebuild: clean all

debug: CFLAGS += -g -DDEBUG
debug: clean all

.PHONY: all clean run directories rebuild debug
