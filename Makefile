BUILD_DIR ?= build
TEST_DIR ?= build/tests
TESTS = $(wildcard $(TEST_DIR)/*)
EXAMPLE_DIR ?= build/Examples
EXAMPLES = $(wildcard $(EXAMPLE_DIR)/*)

BUILD_TYPE ?= Release
CMAKE_CONFIGURE_FLAGS = -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

.PHONY: all
all: test examples

.PHONY: test
test: build
	@echo "Running all tests..."
	@for test_exec in $(TESTS); do \
		if [ -f "$$test_exec" ] && [ -x "$$test_exec" ]; then \
			echo "Running $$test_exec"; \
			$$test_exec; \
			echo "---"; \
		fi \
	done
	@echo "All done!"

.PHONY: examples
examples: build
	@echo "Running all examples..."
	@for test_exec in $(EXAMPLES); do \
		if [ -f "$$test_exec" ] && [ -x "$$test_exec" ]; then \
			echo "Running $$test_exec"; \
			$$test_exec; \
			echo "---"; \
		fi \
	done
	@echo "All done!"

.PHONY: build
build: generate configure just-build

.PHONY: just-build
just-build:
	cmake --build "$(BUILD_DIR)" -j8

.PHONY: configure
configure:
	cmake $(CMAKE_CONFIGURE_FLAGS) -B "$(BUILD_DIR)" -S .

.PHONY: generate
generate: FRONT/parser.cpp FRONT/scanner.cpp

FRONT/parser.cpp:
	bison -d -o FRONT/parser.cpp FRONT/parser.ypp

FRONT/scanner.cpp:
	flex -o FRONT/scanner.cpp FRONT/scanner.lpp

.PHONY: clean
clean:
	rm -f FRONT/parser.cpp FRONT/scanner.cpp
	rm -rf build