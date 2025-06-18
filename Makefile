CFILES	= $(wildcard ./src/*.c)
CC		= clang
CFLAGS	= -Wall -Wextra -pthread
INCLUDE	= -I./lib -I.
LX_LIBS	= -lX11 -lXi -lXcursor -lGL -ldl -lm -lEGL
DEFS	= -DSOKOL_GLES3
TARGET	= ./target/minecraft

.PHONY: install
install: $(CFILES)
	$(CC) $(CFILES) -o $(TARGET) $(INCLUDE) $(DEFS) $(CFLAGS) $(LX_LIBS) 

.PHONY: compile-database
compile-database: $(CFILES)
	bear -- $(CC) $(CFILES) -o $(TARGET) $(INCLUDE) $(DEFS) $(CFLAGS) $(LX_LIBS) 
	
.PHONY: clean
clean:
	rm -f $(TARGET)
