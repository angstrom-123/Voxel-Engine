OUTPUT		:= $(OUTPUT_DIR)/minecraft
BEAR_OUTPUT := $(MAKE_DIR)/compile_commands.json
STATIC_LIBS := $(wildcard $(LIB_DIR)/*.a)

.PHONY: release
release: $(STATIC_LIBS)
	$(CC) $(C_FLAGS) $(RELEASE_FLAGS) $(LINK_FLAGS) $(MAIN_FILE) $^ -o $(OUTPUT)

.PHONY: compiledb
compiledb: $(STATIC_LIBS)
	rm -f $(BEAR_OUTPUT) # Only refresh on call, do not delete during normal clean.
	$(BEAR) --output $(BEAR_OUTPUT) -- $(CC) $(C_FLAGS) $(DEBUG_FLAGS) $(LINK_FLAGS) $(MAIN_FILE) $^ -o $(OUTPUT)

.PHONY: debug
debug: $(STATIC_LIBS)
	$(CC) $(C_FLAGS) $(DEBUG_FLAGS) $(LINK_FLAGS) $(MAIN_FILE) $^ -o $(OUTPUT) -lrt

.PHONY: clean
clean:
	rm -f $(OUTPUT)
