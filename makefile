BUILD_DIR ?= build
CMAKE ?= cmake
CTEST ?= ctest

.PHONY: all configure deps build test clean rebuild help

all: build

help:
	@echo "Targets:"
	@echo "  help      - Show this help message"
	@echo "  all       - Build the project (default)"
	@echo "  configure - Run CMake configure step"
	@echo "  deps      - Fetch dependencies via CMake"
	@echo "  build     - Build the project"
	@echo "  test      - Run tests with CTest"
	@echo "  clean     - Clean build outputs"
	@echo "  rebuild   - Clean then build"

configure:
	@$(CMAKE) -S . -B $(BUILD_DIR) 

build: configure
	@$(CMAKE) -S . -B build -DTAMA_BUILD_TESTS=OFF -DTAMA_RUN_BUILD=OFF > /dev/null 2>&1
	@$(CMAKE) --build $(BUILD_DIR)

run: 
	@$(CMAKE) -S . -B build -DTAMA_RUN_BUILD=ON -DTAMA_BUILD_TESTS=OFF > /dev/null 2>&1
	@$(CMAKE) --build $(BUILD_DIR) > /dev/null 2>&1
	@./$(BUILD_DIR)/tama_run

test:
	@$(CMAKE) -S . -B build -DTAMA_BUILD_TESTS=ON > /dev/null 2>&1
	@$(CTEST) --test-dir $(BUILD_DIR)

clean:
	@$(CMAKE) --build $(BUILD_DIR) --target clean

rebuild: clean build
