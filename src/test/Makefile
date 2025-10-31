TEST_OUTPUT	= $(OUTPUT_DIR)/test
SOURCES		= $(MAIN_FILE) $(wildcard *.c) 

all: $(SOURCES)
	@echo ""
	@echo Compiling test.
	@echo ""
	@$(CC) $(C_FLAGS) $(TEST_FLAGS) $(LINK_FLAGS) $(SOURCES) -o $(TEST_OUTPUT)

.PHONY: clean 
clean:
	@echo ""
	@echo Cleaning test output.
	@echo ""
	@rm -f $(TEST_OUTPUT)
