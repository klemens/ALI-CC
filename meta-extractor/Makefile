# Makefile by Klemens Schölhorn
BIN = meta-extractor
TEST_BIN = test-meta-extractor

# compiler/linker settings
CXX               = g++
override GFLAGS  +=
override CFLAGS  += -c -Wall -Wextra -Werror -pedantic --std=c++11 -O3 -march=native -Iinclude $(GFLAGS)
override LFLAGS  += $(GFLAGS)

# directories
SOURCE  = src/
TESTS   = tests/
HEADER  = include/
OBJ     = obj/
OBJTEST = obj/tests/

# gather files
SOURCE_FILES = $(wildcard $(SOURCE)*.cpp)
OBJ_FILES = $(SOURCE_FILES:$(SOURCE)%.cpp=$(OBJ)%.o)
SOURCE_DEP = $(OBJ_FILES:%.o=%.d)

TESTS_FILES = $(wildcard $(TESTS)*.cpp)
OBJTEST_FILES = $(TESTS_FILES:$(TESTS)%.cpp=$(OBJTEST)%.o)
TESTS_DEP = $(OBJTEST_FILES:%.o=%.d)

# build targets
$(BIN): $(OBJ_FILES)
	$(CXX) -o $@ $(LFLAGS) $^

$(TEST_BIN): $(OBJTEST_FILES) $(filter-out $(OBJ)main.o, $(OBJ_FILES))
	$(CXX) -o $@ $(LFLAGS) $^

tests: $(TEST_BIN)

all: $(BIN) $(TEST_BIN)

clean:
	$(RM) $(OBJ_FILES) $(OBJTEST_FILES) $(SOURCE_DEP) $(TESTS_DEP) $(BIN) $(TEST_BIN)

# source code
$(OBJ)%.o: $(SOURCE)%.cpp
	$(CXX) -o $@ -MMD $(CFLAGS) $<

# tests
$(OBJTEST)%.o: $(TESTS)%.cpp $(HEADER)catch.hpp
	$(CXX) -o $@ -MMD $(CFLAGS) $<

# code dependencies
-include $(SOURCE_DEP)
-include $(TESTS_DEP)
