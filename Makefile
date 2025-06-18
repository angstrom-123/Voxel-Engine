CFILES	= $(wildcard ./src/*.c)
CC		= clang
CFLAGS	= -Wall -Wextra
INCLUDE	= -I ./lib
TARGET	= ./target/minecraft

.PHONY: install
install: $(CFILES)
	$(CC) $(CFILES) -o $(TARGET) $(INCLUDE) -lm $(CFLAGS)

.PHONY: compile-database
compile-database: $(CFILES)
	bear -- $(CC) $(CFILES) -o $(TARGET) $(INCLUDE) -lm $(CFLAGS)
	
.PHONY: clean
clean:
	rm -f $(TARGET)
