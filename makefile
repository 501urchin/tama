BUILD_DIR ?= build
CMAKE ?= cmake
CTEST ?= ctest

.PHONY: all configure deps build test clean rebuild

all: build

configure:
	@$(CMAKE) -S . -B $(BUILD_DIR)

build: configure
	@$(CMAKE) --build $(BUILD_DIR)

test: build
	@$(CTEST) --test-dir $(BUILD_DIR)

clean:
	@$(CMAKE) --build $(BUILD_DIR) --target clean

rebuild: clean build
