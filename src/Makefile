OUTPUT		:= $(OUTPUT_DIR)/minecraft
BEAR_OUTPUT := $(MAKE_DIR)/compile_commands.json
STATIC_LIBS := $(wildcard $(LIB_DIR)/*.a)
TRACE_FILES	:= $(wildcard $(MAKE_DIR)/*.tracing.json)

.PHONY: release
release: $(STATIC_LIBS)
	@echo ""
	@echo "Compiling final (release)."
	@echo ""
	@$(CC) $(C_FLAGS) $(RELEASE_FLAGS) $(LINK_FLAGS) $(MAIN_FILE) $^ -o $(OUTPUT)-release

.PHONY: compiledb
compiledb: $(STATIC_LIBS)
	@echo ""
	@echo "Compiling final (database, debug)."
	@echo ""
	@rm -f $(BEAR_OUTPUT) # Only refresh on call, do not delete during normal clean.
	@$(BEAR) --output $(BEAR_OUTPUT) -- $(CC) $(C_FLAGS) $(DEBUG_FLAGS) $(LINK_FLAGS) $(MAIN_FILE) $^ -o $(OUTPUT)-debug

.PHONY: debug
debug: $(STATIC_LIBS)
	@echo ""
	@echo "Compiling final (debug)."
	@echo ""
	@$(CC) $(C_FLAGS) $(DEBUG_FLAGS) $(LINK_FLAGS) $(MAIN_FILE) $^ -o $(OUTPUT)-debug

.PHONY: profiling
profiling: $(STATIC_LIBS)
	@echo ""
	@echo "Compiling final (profiling)."
	@echo ""
	@$(CC) $(C_FLAGS) $(PROFILING_FLAGS) $(LINK_FLAGS) $(MAIN_FILE) $^ -o $(OUTPUT)-profiling

.PHONY: clean
clean:
	@echo ""
	@echo Cleaning output dir.
	@echo ""
	@rm -f $(OUTPUT)*
	@echo ""
	@echo Cleaning profiling data.
	@echo ""
	@for file in $(TRACE_FILES); do\
		@rm -f $$file;\
	done
