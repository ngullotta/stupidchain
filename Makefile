CC = gcc

SRC_DIR = src

BUILD_DIR = build

TARGET = stupidchain

C_SOURCES = $(wildcard $(SRC_DIR)/*.c)
SOURCES = $(C_SOURCES)

C_OBJECTS = $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJECTS = $(C_OBJECTS)

CFLAGS = -Wall -g

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean