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
	@$(CMAKE) --build $(BUILD_DIR)

test: build
	@$(CTEST) --test-dir $(BUILD_DIR)

clean:
	@$(CMAKE) --build $(BUILD_DIR) --target clean

rebuild: clean build
