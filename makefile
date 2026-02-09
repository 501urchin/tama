BUILD_DIR ?= build
CMAKE ?= cmake
CTEST ?= ctest

.PHONY: all configure build run test clean rebuild help

all: build

help:
	@echo "Targets:"
	@echo "  help      - Show this help message"
	@echo "  configure - Configure CMake build directory"
	@echo "  build     - Build the project (Release by default)"
	@echo "  run       - Build & run the example program"
	@echo "  test      - Build & run tests"
	@echo "  clean     - Clean build outputs"
	@echo "  rebuild   - Clean then build"

configure:
	@$(CMAKE) -S . -B $(BUILD_DIR) > /dev/null 2>&1

# Default build: Release, no tests, no run
build: configure
	@$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -DTAMA_BUILD_TESTS=OFF -DTAMA_RUN_BUILD=OFF
	@$(CMAKE) --build $(BUILD_DIR) --parallel

# Build & run the example program
run: configure
	@$(CMAKE) -S . -B $(BUILD_DIR)  -DTAMA_RUN_BUILD=ON -DTAMA_BUILD_TESTS=OFF > /dev/null 2>&1
	@$(CMAKE) --build $(BUILD_DIR) --parallel 
	@./$(BUILD_DIR)/tama_run

# Build & run tests
test: configure
	@$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DTAMA_BUILD_TESTS=ON -DTAMA_RUN_BUILD=OFF > /dev/null 2>&1
	@$(CMAKE) --build $(BUILD_DIR) --parallel > /dev/null 2>&1
	@$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

clean:
	@$(CMAKE) --build $(BUILD_DIR) --target clean

rebuild: clean build
