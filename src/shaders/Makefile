SHDR_COMPILER	= $(PREBUILT_DIR)/sokol-shdc
SOURCES			= $(wildcard *.glsl)
HEADER_OUTPUTS	= $(patsubst %.glsl, %.glsl.h, $(SOURCES))

all:
	for shdr in $(SOURCES); do \
		$(SHDR_COMPILER) -i $$shdr -o $$shdr.h -l glsl300es; \
	done

.PHONY: clean 
clean:
	rm -f $(HEADER_OUTPUTS)
