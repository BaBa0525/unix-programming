SRC_DIR = ./src
OBJ_DIR = ./obj
LOG_DIR = ./logs

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC = gcc
CFLAGS = -Wall -g -D_GNU_SOURCE -fPIC -I include
LDFLAGS = -shared -ldl

.PHONY: clean

sandbox.so: $(OBJ) | $(LOG_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $< $(CFLAGS) -o $@

$(OBJ_DIR) $(LOG_DIR):
	mkdir -p $@
	
clean:
	@rm -rv $(OBJ_DIR) $(LOG_DIR)
	@rm -v sandbox.so