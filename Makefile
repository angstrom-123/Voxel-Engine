CFILES		= $(wildcard ./src/*.c) $(wildcard ./src/math/*.c)
SHADERS		= $(wildcard ./src/shaders/*.glsl)
SHDR_HEADS	= $(wildcard ./src/shaders/*.glsl.h)

CC			= clang
CFLAGS		= -Wall -Wextra -pthread
INCLUDE		= -I./lib -I.
LX_LIBS		= -lX11 -lXi -lXcursor -lGL -ldl -lm -lEGL
DEFS		= -DSOKOL_GLES3

OUTPUT		= ./build/minecraft

.PHONY: install
install: $(CFILES)
	rm -f $(OUTPUT)
	$(CC) $(CFILES) -o $(OUTPUT) $(INCLUDE) $(DEFS) $(CFLAGS) $(LX_LIBS) 

.PHONY: compile-database
compile-database: $(CFILES)
	rm -f compile_commands.json
	bear -- $(CC) $(CFILES) -o $(OUTPUT) $(INCLUDE) $(DEFS) $(CFLAGS) $(LX_LIBS) 

.PHONY: compile-shaders
compile-shaders: $(SHADERS)
	for shdr_h in $(SHDR_HEADS); do rm -f $$shdr_h; done

	for shdr in $(SHADERS); do \
		./sokol-shdc -i $$shdr -o $$shdr.h -l glsl300es; \
	done
