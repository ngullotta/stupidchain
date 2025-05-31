CC = gcc

SRC_DIR = src
BUILD_DIR = build

SERVER_BIN = server
CLIENT_BIN = client

SERVER_MAIN_SRC = $(SRC_DIR)/main.c
CLIENT_MAIN_SRC = $(SRC_DIR)/client.c

COMMON_C_SOURCES = $(filter-out $(SERVER_MAIN_SRC) $(CLIENT_MAIN_SRC), $(wildcard $(SRC_DIR)/*.c))

COMMON_OBJECTS = $(COMMON_C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

CFLAGS = -Wall -g

LDFLAGS =

.PHONY: all server client clean

all: $(BUILD_DIR)/$(SERVER_BIN) $(BUILD_DIR)/$(CLIENT_BIN)

$(BUILD_DIR)/$(SERVER_BIN): $(BUILD_DIR)/$(notdir $(SERVER_MAIN_SRC:.c=.o)) $(COMMON_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(BUILD_DIR)/$(notdir $(SERVER_MAIN_SRC:.c=.o)) $(COMMON_OBJECTS) $(LDFLAGS) -o $@
	@echo "Built server application: $@"

$(BUILD_DIR)/$(CLIENT_BIN): $(BUILD_DIR)/$(notdir $(CLIENT_MAIN_SRC:.c=.o)) $(COMMON_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(BUILD_DIR)/$(notdir $(CLIENT_MAIN_SRC:.c=.o)) $(COMMON_OBJECTS) $(LDFLAGS) -o $@
	@echo "Built client application: $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

server: $(BUILD_DIR)/$(SERVER_BIN)
client: $(BUILD_DIR)/$(CLIENT_BIN)

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
	@echo "Clean complete."