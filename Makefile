CMAKE      ?= cmake
NINJA      ?= ninja

# extra arguments passed to every cmake configure
CMAKE_ARGS ?=

# restrict a build to one ninja target (empty means all)
TARGET     ?=

DEBUG_DIR     := build/debug
RELEASE_DIR   := build/release
DEBUG_NINJA   := $(DEBUG_DIR)/build.ninja
RELEASE_NINJA := $(RELEASE_DIR)/build.ninja

.DEFAULT_GOAL := debug
.PHONY: all build debug release test format clean fclean re

all: build

build: debug release

debug: $(DEBUG_NINJA)
	$(NINJA) -C $(DEBUG_DIR) $(TARGET)
	@ln -sf $(DEBUG_DIR)/compile_commands.json compile_commands.json

release: $(RELEASE_NINJA)
	$(NINJA) -C $(RELEASE_DIR) $(TARGET)

$(DEBUG_NINJA): CMakeLists.txt
	$(CMAKE) -S . -B $(DEBUG_DIR) -G Ninja \
		-DCMAKE_BUILD_TYPE=Debug \
		$(CMAKE_ARGS)

$(RELEASE_NINJA): CMakeLists.txt
	$(CMAKE) -S . -B $(RELEASE_DIR) -G Ninja \
		-DCMAKE_BUILD_TYPE=Release \
		$(CMAKE_ARGS)

test: debug
	cd $(DEBUG_DIR) && ctest --output-on-failure

format:
	clang-format -i $(shell find src -name '*.c' -or -name '*.h')
	clang-format -i $(shell find include -name '*.c' -or -name '*.h')
	clang-format -i $(shell find test -name '*.c' -or -name '*.h')

clean:
	@for d in $(DEBUG_DIR) $(RELEASE_DIR); do \
		if [ -f $$d/build.ninja ]; then $(NINJA) -C $$d clean; fi; \
	done

fclean:
	rm -rf build

re: fclean
	$(MAKE) debug

.PHONY: b t f
b: build
t: test
f: format
