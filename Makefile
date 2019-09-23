BUILD_DIR := build
# All source files except the main entry point(s)
SOURCE_DIR := src
INCLUDE_DIR := include
TEST_DIR := tests
DEPS_DIR := depends
MAIN_ENTRY_POINT := main.cpp

GTEST_BUILD_DIR := $(BUILD_DIR)/$(DEPS_DIR)/googletest
GTEST_DIR := $(DEPS_DIR)/googletest/googletest
GMOCK_DIR := $(DEPS_DIR)/googletest/googlemock

GTEST_LIB := libgtest.a
GTEST_HEADERS := $(GTEST_DIR)/include/gtest/*.h $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SOURCES_ := $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

GMOCK_LIB := libgmock.a
GMOCK_HEADERS := $(GMOCK_DIR)/include/gmock/*.h $(GMOCK_DIR)/include/gmock/internal/*.h $(GTEST_HEADERS)
GMOCK_SOURCES_ := $(GMOCK_DIR)/src/*.cc $(GMOCK_HEADERS)

INCLUDE_FLAGS := -I$(INCLUDE_DIR) -I$(DEPS_DIR)/clipp/include -I$(DEPS_DIR)/cppitertools -I$(DEPS_DIR)/GSL/include
# Prevent clang from complaining about warnings in clipp
INCLUDE_FLAGS += -isystem $(DEPS_DIR)/clipp/include -isystem $(DEPS_DIR)/cppitertools -isystem $(DEPS_DIR)/GSL/include
WARNING_FLAGS := -Wall -Wpedantic -Wextra -Werror -Wconversion -Wcast-align -Wcast-qual            \
				 -Wctor-dtor-privacy -Wdisabled-optimization -Wold-style-cast -Wformat=2           \
				 -Winit-self -Wmissing-declarations -Wmissing-include-dirs                         \
				 -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo    \
				 -Wstrict-overflow=5 -Wundef -Wzero-as-null-pointer-constant

TEST_TARGET := $(BUILD_DIR)/testsuite
TARGET := $(BUILD_DIR)/main

# Source files without the main entry point so I can link against the unit tests.
SRC := $(shell find $(SOURCE_DIR) -name '*.cpp')
OBJ := $(SRC:%.cpp=$(BUILD_DIR)/%.o)

# Unit test source files
TEST_SRC := $(shell find $(TEST_DIR) -name '*.cpp')
TEST_OBJ := $(TEST_SRC:%.cpp=$(BUILD_DIR)/%.o)

DEP := $(OBJ:%.o=%.d) $(TEST_OBJ:%.o=%.d) $(BUILD_DIR)/$(MAIN_ENTRY_POINT:%.o=%.d)

CXX := clang++
LINK := clang++

LINKFLAGS += -lm
CXXFLAGS += $(INCLUDE_FLAGS) $(WARNING_FLAGS) -O3 -std=c++17 -x c++

.DEFAULT_GOAL := all

## View this help message
.PHONY: help
help:
	@# Taken from https://gist.github.com/prwhite/8168133#gistcomment-2749866
	@awk '{ \
			if ($$0 ~ /^.PHONY: [a-zA-Z\-\_0-9]+$$/) { \
				helpCommand = substr($$0, index($$0, ":") + 2); \
				if (helpMessage) { \
					printf "\033[36m%-20s\033[0m %s\n", \
						helpCommand, helpMessage; \
					helpMessage = ""; \
				} \
			} else if ($$0 ~ /^[a-zA-Z\-\_0-9.]+:/) { \
				helpCommand = substr($$0, 0, index($$0, ":")); \
				if (helpMessage) { \
					printf "\033[36m%-20s\033[0m %s\n", \
						helpCommand, helpMessage; \
					helpMessage = ""; \
				} \
			# Handle multi-line comments \
			} else if ($$0 ~ /^##/) { \
				if (helpMessage) { \
					helpMessage = helpMessage"\n                     "substr($$0, 3); \
				} else { \
					helpMessage = substr($$0, 3); \
				} \
			# Handle section headings.\
			} else { \
				if (helpMessage) { \
					# Remove leading space \
					helpMessage = substr(helpMessage, 2); \
					print "\n"helpMessage \
				} \
				helpMessage = ""; \
			} \
		}' \
		$(MAKEFILE_LIST)

## Building the Application

## Build the main application
.PHONY: all
all: $(TARGET)

## Build applications with debug symbols and no optimization
.PHONY: debug
debug: clean-apps
debug: CXXFLAGS += -g -Og
debug: all

## Run the main application
.PHONY: run
run:
	@# Depends on *either* all or debug, so don't list explicitly.
	./$(TARGET)

$(TARGET): $(OBJ) $(BUILD_DIR)/$(MAIN_ENTRY_POINT:%.cpp=%.o)
	@mkdir -p $(@D)
	$(LINK) $^ -o $@ $(LINKFLAGS)

# Apparently the location of this -include is what broke it before.
-include $(DEP)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

## Building and Running Tests

## Build the unit and integration tests
.PHONY: tests
tests: $(TEST_TARGET)

## Run the unit and integration tests
.PHONY: check
check: tests
	./$(TEST_TARGET)

# $^ includes all prerequisites, except for order-only prerequisites
$(TEST_TARGET): | libgtest libgmock
$(TEST_TARGET): CXXFLAGS += -g -w -I$(GTEST_DIR)/include -I$(GMOCK_DIR)/include
$(TEST_TARGET): LINKFLAGS += -L$(GTEST_BUILD_DIR) -lgtest -lgmock -pthread
$(TEST_TARGET): $(OBJ) $(TEST_OBJ)
	@mkdir -p $(@D)
	$(LINK) $^ -o $@ $(LINKFLAGS)

## Build the gtest static library
.PHONY: libgtest
libgtest: $(GTEST_BUILD_DIR)/$(GTEST_LIB)

$(GTEST_BUILD_DIR)/$(GTEST_LIB): $(GTEST_BUILD_DIR)/gtest-all.o
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^

$(GTEST_BUILD_DIR)/gtest-all.o: CXXFLAGS += -g -w -I$(GTEST_DIR) -I$(GMOCK_DIR) -I$(GTEST_DIR)/include -I$(GMOCK_DIR)/include
$(GTEST_BUILD_DIR)/gtest-all.o: $(GTEST_SOURCES_)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc -o $@

## Build the gmock static library
.PHONY: libgmock
libgmock: $(GTEST_BUILD_DIR)/$(GMOCK_LIB)

$(GTEST_BUILD_DIR)/$(GMOCK_LIB): $(GTEST_BUILD_DIR)/gmock-all.o
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^

$(GTEST_BUILD_DIR)/gmock-all.o: CXXFLAGS += -g -w -I$(GTEST_DIR) -I$(GMOCK_DIR) -I$(GTEST_DIR)/include -I$(GMOCK_DIR)/include
$(GTEST_BUILD_DIR)/gmock-all.o: $(GMOCK_SOURCES_)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $(GMOCK_DIR)/src/gmock-all.cc -o $@

## Cleaning Artifacts

## Clean the application and test articfacts
.PHONY: clean
clean: clean-apps
clean: clean-tests

## Clean the application, test, documentation, and dependency artifacts
.PHONY: cleanall
cleanall: clean-apps
cleanall: clean-tests
cleanall: clean-docs
cleanall: clean-deps

## Clean the application artifacts
.PHONY: clean-apps
clean-apps:
	rm -rf $(TARGET)* $(BUILD_DIR)/$(SOURCE_DIR)/*

## Clean the test artifacts
.PHONY: clean-tests
clean-tests:
	rm -rf $(TEST_TARGET)* $(BUILD_DIR)/$(TEST_DIR)/*

## Clean the documentation artifacts
.PHONY: clean-docs
clean-docs:
	rm -rf $(BUILD_DIR)/html/* $(BUILD_DIR)/latex/*

## Clean the dependency artifacts
.PHONY: clean-deps
clean-deps:
	rm -rf $(GTEST_BUILD_DIR)/*

## Tools

## Run clang-format on project
.PHONY: format
format:
	find $(INCLUDE_DIR) $(SOURCE_DIR) $(TEST_DIR) $(MAIN_ENTRY_POINT) -name "*.cpp" -o -name "*.h" | xargs clang-format -style=file -i

## Run clang-tidy on project
.PHONY: lint
lint:
	clang-tidy $(shell find $(INCLUDE_DIR) $(SOURCE_DIR) -name "*.cpp" -o -name "*.h") $(MAIN_ENTRY_POINT) -- $(CXXFLAGS)

## Build documentation
.PHONY: docs
docs:
	doxygen .doxyfile

## Open documentation in Firefox
.PHONY: viewdocs
viewdocs:
	firefox $(BUILD_DIR)/html/index.html &
