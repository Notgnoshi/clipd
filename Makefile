# Build and install artifact locations for this project and its dependencies.
BUILD_DIR := build
INSTALL_DIR := $(BUILD_DIR)/sysroot/usr
INSTALL_LIB_DIR := $(INSTALL_DIR)/lib
INSTALL_INCLUDE_DIR := $(INSTALL_DIR)/include

# All source files except the main entry point(s)
SOURCE_DIR := src
INCLUDE_DIR := include
TEST_DIR := tests
DEPS_DIR := depends
MAIN_ENTRY_POINT := main.cpp

# Location to build the googletest libraries.
GTEST_BUILD_DIR := $(BUILD_DIR)/$(DEPS_DIR)/googletest
GTEST_DIR := $(DEPS_DIR)/googletest/googletest
GMOCK_DIR := $(DEPS_DIR)/googletest/googlemock

GTEST_LIB := $(INSTALL_LIB_DIR)/libgtest.a
GTEST_HEADERS := $(GTEST_DIR)/include/gtest/*.h $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SOURCES_ := $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

GMOCK_LIB := $(INSTALL_LIB_DIR)/libgmock.a
GMOCK_HEADERS := $(GMOCK_DIR)/include/gmock/*.h $(GMOCK_DIR)/include/gmock/internal/*.h $(GTEST_HEADERS)
GMOCK_SOURCES_ := $(GMOCK_DIR)/src/*.cc $(GMOCK_HEADERS)

# Location to build libzmq.
LIBZMQ_BUILD_DIR := $(BUILD_DIR)/$(DEPS_DIR)/libzmq
# NOTE: Do *not* specify multiple objects here, even if you want shared and static builds.
LIBZMQ_LIBS := $(INSTALL_LIB_DIR)/libzmq.a

ZMQPP_BUILD_DIR := $(BUILD_DIR)/$(DEPS_DIR)/zmqpp
# NOTE: Do *not* specify multiple objects here, even if you want shared and static builds.
ZMQPP_LIBS := $(INSTALL_LIB_DIR)/libzmqpp.a

CLIP_BUILD_DIR := $(BUILD_DIR)/$(DEPS_DIR)/clip
CLIP_LIB := $(INSTALL_LIB_DIR)/libclip.a

INCLUDE_FLAGS := -I$(INCLUDE_DIR) -I$(INSTALL_INCLUDE_DIR) -I$(DEPS_DIR)/clipp/include -I$(DEPS_DIR)/cppitertools -I$(DEPS_DIR)/GSL/include -I$(DEPS_DIR)/clip
# Prevent clang from complaining about warnings in clipp
INCLUDE_FLAGS += -isystem $(INSTALL_INCLUDE_DIR) -isystem $(DEPS_DIR)/clipp/include -isystem $(DEPS_DIR)/cppitertools -isystem $(DEPS_DIR)/GSL/include -isystem $(DEPS_DIR)/clip
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

LINKFLAGS += -L$(INSTALL_LIB_DIR) -lm -pthread -lclip -lX11 -lxcb
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
debug: CXXFLAGS += -g -Og
debug: all

$(TARGET): | depends
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
$(TEST_TARGET): LINKFLAGS += -lgtest -lgmock -pthread
# Exclude the application main entry point.
$(TEST_TARGET): $(OBJ) $(TEST_OBJ)
	$(LINK) $^ -o $@ $(LINKFLAGS)

## Building project dependencies

## Build all project dependencies.
.PHONY: depends
depends: libgtest
depends: libgmock
depends: libzmq
depends: zmqpp
depends: clip

$(BUILD_DIR):
	mkdir -p $@
$(INSTALL_DIR):
	mkdir -p $@
$(INSTALL_LIB_DIR):
	mkdir -p $@
$(INSTALL_INCLUDE_DIR):
	mkdir -p $@
$(GTEST_BUILD_DIR):
	mkdir -p $@
$(LIBZMQ_BUILD_DIR):
	mkdir -p $@
$(ZMQPP_BUILD_DIR):
	mkdir -p $@
$(CLIP_BUILD_DIR):
	mkdir -p $@

## Build the gtest static library
.PHONY: libgtest
libgtest: $(GTEST_LIB)

$(GTEST_LIB): | $(INSTALL_LIB_DIR)
$(GTEST_LIB): $(GTEST_BUILD_DIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(GTEST_BUILD_DIR)/gtest-all.o: | $(GTEST_BUILD_DIR)
$(GTEST_BUILD_DIR)/gtest-all.o: CXXFLAGS += -g -w -I$(GTEST_DIR) -I$(GMOCK_DIR) -I$(GTEST_DIR)/include -I$(GMOCK_DIR)/include
$(GTEST_BUILD_DIR)/gtest-all.o: $(GTEST_SOURCES_)
	$(CXX) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc -o $@

## Build the gmock static library
.PHONY: libgmock
libgmock: $(GMOCK_LIB)

$(GMOCK_LIB): | $(INSTALL_LIB_DIR)
$(GMOCK_LIB): $(GTEST_BUILD_DIR)/gmock-all.o
	$(AR) $(ARFLAGS) $@ $^

$(GTEST_BUILD_DIR)/gmock-all.o: | $(GTEST_BUILD_DIR)
$(GTEST_BUILD_DIR)/gmock-all.o: CXXFLAGS += -g -w -I$(GTEST_DIR) -I$(GMOCK_DIR) -I$(GTEST_DIR)/include -I$(GMOCK_DIR)/include
$(GTEST_BUILD_DIR)/gmock-all.o: $(GMOCK_SOURCES_)
	$(CXX) $(CXXFLAGS) -c $(GMOCK_DIR)/src/gmock-all.cc -o $@

## Build the libzmq shared and static libraries
.PHONY: libzmq
libzmq: DEPS_DIR := $(shell readlink -f $(DEPS_DIR))
libzmq: LIBZMQ_BUILD_DIR := $(shell readlink -m $(LIBZMQ_BUILD_DIR))
libzmq: INSTALL_DIR := $(shell readlink -m $(INSTALL_DIR))
libzmq: | $(LIBZMQ_BUILD_DIR) $(INSTALL_DIR)
libzmq: $(LIBZMQ_LIBS)

# This is an inherently serial task, so why complicate it by breaking it up into separate targets?
$(LIBZMQ_LIBS):
	cd $(DEPS_DIR)/libzmq; \
	./autogen.sh

	cd $(LIBZMQ_BUILD_DIR); \
	$(DEPS_DIR)/libzmq/configure --enable-shared --enable-static --prefix=$(INSTALL_DIR)

	cd $(LIBZMQ_BUILD_DIR); \
	$(MAKE)

	cd $(LIBZMQ_BUILD_DIR); \
	$(MAKE) install

## Build the zmqpp C++ bindings for libzmq
.PHONY: zmqpp
zmqpp: libzmq
zmqpp: INSTALL_DIR := $(shell readlink -m $(INSTALL_DIR))
zmqpp: INSTALL_INCLUDE_DIR := $(shell readlink -m $(INSTALL_INCLUDE_DIR))
zmqpp: INSTALL_LIB_DIR := $(shell readlink -m $(INSTALL_LIB_DIR))
zmqpp: | $(ZMQPP_BUILD_DIR) $(INSTALL_DIR)
zmqpp: $(ZMQPP_LIBS)

$(ZMQPP_LIBS): $(LIBZMQ_LIBS)
	cd $(DEPS_DIR)/zmqpp; \
	$(MAKE) PREFIX=$(INSTALL_DIR) CXXFLAGS=-I$(INSTALL_INCLUDE_DIR) LDFLAGS=-L$(INSTALL_LIB_DIR)

	cd $(DEPS_DIR)/zmqpp; \
	$(MAKE) PREFIX=$(INSTALL_DIR) CXXFLAGS=-I$(INSTALL_INCLUDE_DIR) LDFLAGS=-L$(INSTALL_LIB_DIR) install

## Build the clip X11 clipboard library.
.PHONY: clip
clip: DEPS_DIR := $(shell readlink -f $(DEPS_DIR))
clip: | $(CLIP_BUILD_DIR) $(INSTALL_LIB_DIR)
clip: $(CLIP_LIB)

$(CLIP_LIB):
	@# Fix a pesky race condition where the cd starts before the mkdir finishes...
	@mkdir -p $(CLIP_BUILD_DIR)
	cd $(CLIP_BUILD_DIR); \
	cmake -DCMAKE_INSTALL_PREFIX=$(INSTALL_DIR) $(DEPS_DIR)/clip

	cd $(CLIP_BUILD_DIR); \
	$(MAKE)

	cp $(CLIP_BUILD_DIR)/libclip.a $(INSTALL_LIB_DIR)

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
	rm -rf $(BUILD_DIR)/$(DEPS_DIR)/* $(INSTALL_DIR)/*
	rm -f $(DEPS_DIR)/libzmq/configure
	cd $(DEPS_DIR)/zmqpp; \
	$(MAKE) clean

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

## Open the zmqpp documentation in Firefox
.PHONY: viewdocs-zmqpp
viewdocs-zmqpp:
	firefox https://zeromq.github.io/zmqpp/ &

## Show the contents of the build directory
.PHONY: list
list:
	tree -C -I "libzmq|clip|html|latex" $(BUILD_DIR)
