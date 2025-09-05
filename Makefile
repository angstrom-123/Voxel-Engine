TESTFILES	= ./src/test.c ./src/main.c
CFILES		= $(filter-out ./src/test.c,  $(wildcard ./src/*.c))
LIBSOKOL	= $(wildcard ./lib/sokol/*)
LIBEM		= $(wildcard ./lib/libem/*)
LIBNK		= ./lib/nuklear/nuklear.h
SHADERS		= $(wildcard ./src/shaders/*.glsl)
SHDR_HEADS	= $(wildcard ./src/shaders/*.glsl.h)

CC			= clang
CFLAGS		= -Wall -Wextra -Wno-missing-braces -pthread
INCLUDE		= -I./lib -I.
LX_LIBS		= -lX11 -lXi -lXcursor -lGL -ldl -lm -lEGL
DEFS		= -DSOKOL_GLES3
TEST_DEFS	= -DTEST

OUTPUT		= ./build/minecraft
TEST_OUTPUT	= ./build/test_minecraft

.PHONY: release
release: $(CFILES) $(LIBSOKOL) $(LIBEM) $(LIBNK) $(SHDR_HEADS)
	rm -f $(OUTPUT)
	$(CC) $(CFILES) -o $(OUTPUT) $(INCLUDE) $(DEFS) $(CFLAGS) -O1 $(LX_LIBS)

.PHONY: install
install: $(CFILES) $(LIBSOKOL) $(LIBEM) $(LIBNK) $(SHDR_HEADS)
	rm -f $(OUTPUT)
	$(CC) -g $(CFILES) -o $(OUTPUT) $(INCLUDE) $(DEFS) $(CFLAGS) $(LX_LIBS) 

.PHONY: test
test: $(TESTFILES) $(LIBEM)
	rm -f $(TEST_OUTPUT)
	$(CC) -g $(TESTFILES) -o $(TEST_OUTPUT) $(INCLUDE) $(TEST_DEFS) $(CFLAGS) $(LX_LIBS)

.PHONY: database
database: $(CFILES) $(LIBSOKOL) $(LIBEM) $(LIBNK)
	rm -f compile_commands.json
	bear -- $(CC) $(CFILES) -o $(OUTPUT) $(INCLUDE) $(DEFS) $(CFLAGS) $(LX_LIBS) 

.PHONY: shaders
shaders: $(SHADERS)
	for shdr_h in $(SHDR_HEADS); do rm -f $$shdr_h; done

	for shdr in $(SHADERS); do \
		./sokol-shdc -i $$shdr -o $$shdr.h -l glsl300es; \
	done
