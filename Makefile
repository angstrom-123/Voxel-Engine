MAKE_DIR		:= $(PWD)

OBJ_DIR			:= $(MAKE_DIR)/obj
SRC_DIR			:= $(MAKE_DIR)/src
INC_DIR 		:= $(MAKE_DIR)/include
LIB_DIR			:= $(MAKE_DIR)/lib
BIN_DIR			:= $(MAKE_DIR)/bin

OUTPUT_DIR		:= $(BIN_DIR)/linux/output
PREBUILT_DIR	:= $(BIN_DIR)/linux/prebuilt

ENGINE_DIR		:= $(SRC_DIR)/engine
APP_DIR			:= $(SRC_DIR)/app
SHDR_DIR		:= $(SRC_DIR)/shaders
TEST_DIR		:= $(SRC_DIR)/test

MAIN_FILE		:= $(SRC_DIR)/main.c

INCLUDE_PATHS	:= 
INCLUDE_PATHS	+= -I$(ENGINE_DIR)/
INCLUDE_PATHS	+= -I$(APP_DIR)/
INCLUDE_PATHS	+= -I$(SHDR_DIR)/
INCLUDE_PATHS	+= -I$(TEST_DIR)/
INCLUDE_PATHS	+= -I$(SRC_DIR)/
INCLUDE_PATHS	+= -I$(INC_DIR)/
INCLUDE_PATHS	+= -I$(INC_DIR)/sokol/
INCLUDE_PATHS	+= -I$(INC_DIR)/nuklear/
INCLUDE_PATHS	+= -I$(INC_DIR)/libem/

LIB_PATHS		:=

CC				= clang
BEAR			= bear

LINK_FLAGS		:= -pthread -lX11 -lXi -lXcursor -lGL -ldl -lm -lEGL

C_FLAGS			:= 
C_FLAGS			+= $(INCLUDE_PATHS) $(LIB_PATHS)
C_FLAGS			+= -Wall -Wextra -Wno-missing-braces -std=c11
C_FLAGS			+= -DSOKOL_GLES3 -DPLAT_LINUX

DEBUG_FLAGS		:=
DEBUG_FLAGS		+= -fsanitize=address -g -O0
DEBUG_FLAGS		+= -DEM_ENABLE_LOGGING -DDEBUG

RELEASE_FLAGS	:= 
RELEASE_FLAGS	+= -O2
RELEASE_FLAGS	+= -DRELEASE 

TEST_FLAGS		:=
TEST_FLAGS		+= -fsanitize=address -g -O0 
TEST_FLAGS		+= -DTEST

export MAKE_DIR OBJ_DIR SRC_DIR INC_DIR LIB_DIR BIN_DIR OUTPUT_DIR PREBUILT_DIR CC BEAR C_FLAGS LINK_FLAGS MAIN_FILE RELEASE_FLAGS DEBUG_FLAGS TEST_FLAGS

all:
	$(MAKE) -C src/shaders
	$(MAKE) -C src/engine release
	$(MAKE) -C src/app release

.PHONY: release
release:
	$(MAKE) -C src/shaders
	$(MAKE) -C src/engine release
	$(MAKE) -C src/app release
	$(MAKE) -C src release

.PHONY: compiledb
compiledb:
	$(MAKE) -C src/shaders
	$(MAKE) -C src/engine debug
	$(MAKE) -C src/app debug
	$(MAKE) -C src compiledb

.PHONY: debug
debug:
	$(MAKE) -C src/shaders
	$(MAKE) -C src/engine debug
	$(MAKE) -C src/app debug
	$(MAKE) -C src debug

.PHONY: test 
test:
	$(MAKE) -C src/test

.PHONY: clean
clean:
	$(MAKE) -C src/shaders clean
	$(MAKE) -C src/engine clean
	$(MAKE) -C src/app clean
	$(MAKE) -C src clean
