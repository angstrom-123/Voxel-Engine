TEST_OUTPUT	= $(OUTPUT_DIR)/test
SOURCES		= $(MAIN_FILE) $(wildcard *.c) 

all: $(SOURCES)
	$(CC) $(C_FLAGS) $(TEST_FLAGS) $(LINK_FLAGS) $(SOURCES) -o $(TEST_OUTPUT)

.PHONY: clean 
clean:
	rm -f $(TEST_OUTPUT)
