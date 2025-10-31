LIB_OUTPUT	= $(LIB_DIR)/engine.a
SOURCES		= $(wildcard *.c)
OBJECTS		= $(patsubst %.c, %.o, $(SOURCES))

.PHONY: release
release: $(SOURCES)
	@echo ""
	@echo "Compiling engine static library (release)."
	@echo ""
	@$(CC) $(C_FLAGS) $(RELEASE_FLAGS) -c $^
	@$(AR) rcs $(LIB_OUTPUT) $(OBJECTS)

.PHONY: debug
debug: $(SOURCES)
	@echo ""
	@echo "Compiling engine static library (debug)."
	@echo ""
	@$(CC) $(C_FLAGS) $(DEBUG_FLAGS) -c $^
	@$(AR) rcs $(LIB_OUTPUT) $(OBJECTS)

.PHONY: profiling
profiling: $(SOURCES)
	@echo ""
	@echo "Compiling engine static library (profiling)."
	@echo ""
	@$(CC) $(C_FLAGS) $(PROFILING_FLAGS) -c $^
	@$(AR) rcs $(LIB_OUTPUT) $(OBJECTS)

.PHONY: clean 
clean:
	@echo ""
	@echo Cleaning engine output.
	@echo ""
	@rm -f $(LIB_OUTPUT)
	@echo ""
	@echo Cleaning engine intermediate files.
	@echo ""
	@rm -f $(OBJECTS)
