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
build: configure
	cmake --build "$(BUILD_DIR)" -j8

.PHONY: configure
configure:
	cmake $(CMAKE_CONFIGURE_FLAGS) -B "$(BUILD_DIR)" -S .

.PHONY: clean
clean:
	rm -rf build