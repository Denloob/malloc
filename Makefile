SRC_DIR = src
BIN_DIR = bin
DEP_DIR = .deps

CC = g++
CFLAGS = -g -fsanitize=address -Wall -Wextra -pedantic -std=c++14 -I$(SRC_DIR)
LDFLAGS = -lm -fsanitize=address
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

TARGET = prog
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%.o, $(SRCS))
DEPS = $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(SRCS))

.PHONY: clean all run

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $(OBJS)) $(dir $(DEPS))
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(DEPS)

run: all
	@$(BIN_DIR)/$(TARGET)

clean:
	rm -rf $(BIN_DIR) $(DEP_DIR)
