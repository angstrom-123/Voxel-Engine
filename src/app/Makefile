LIB_OUTPUT	= $(LIB_DIR)/app.a
SOURCES		= $(wildcard *.c)
OBJECTS		= $(patsubst %.c, %.o, $(SOURCES))

.PHONY: release
release: $(SOURCES)
	$(CC) $(C_FLAGS) $(RELEASE_FLAGS) -c $^
	$(AR) rcs $(LIB_OUTPUT) $(OBJECTS)

.PHONY: debug
debug: $(SOURCES)
	$(CC) $(C_FLAGS) $(DEBUG_FLAGS) -c $^
	$(AR) rcs $(LIB_OUTPUT) $(OBJECTS)

.PHONY: clean 
clean:
	rm -f $(LIB_OUTPUT)
	rm -f $(OBJECTS)
