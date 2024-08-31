BIN    = icc
BUILD  = build
SRC    = icc.c
TARGET = $(BUILD)/$(BIN)

CC     ?= cc
PREFIX ?= /usr/local/bin

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(BUILD)
	$(CC) -o $(TARGET) $(SRC)

clean:
	rm -rf $(BUILD)

install:
	mv $(TARGET) $(PREFIX)

run:
	./$(TARGET)

.PHONY: all install clean
