SHDR_COMPILER	= $(PREBUILT_DIR)/sokol-shdc
SOURCES			= $(wildcard *.glsl)
HEADER_OUTPUTS	= $(patsubst %.glsl, %.glsl.h, $(SOURCES))

all:
	@echo ""
	@echo Compiling shaders.
	@echo ""
	@for shdr in $(SOURCES); do \
		$(SHDR_COMPILER) -i $$shdr -o $$shdr.h -l glsl300es; \
	done

.PHONY: clean 
clean:
	@echo ""
	@echo Cleaning shader output.
	@echo ""
	@rm -f $(HEADER_OUTPUTS)
