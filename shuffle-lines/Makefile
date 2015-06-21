# Makefile by Klemens Schölhorn
BIN = shuffle-lines

# compiler/linker settings
CXX               = g++
override GFLAGS  +=
override CFLAGS  += -c -Wall -Wextra -Werror -pedantic --std=c++11 -O2 -Iinclude $(GFLAGS)
override LFLAGS  += $(GFLAGS)

# directories
SOURCE  = src/
OBJ     = obj/

# gather files
SOURCE_FILES = $(wildcard $(SOURCE)*.cpp)
OBJ_FILES = $(SOURCE_FILES:$(SOURCE)%.cpp=$(OBJ)%.o)
SOURCE_DEP = $(OBJ_FILES:%.o=%.d)

# build targets
$(BIN): $(OBJ_FILES)
	$(CXX) -o $@ $(LFLAGS) $^

clean:
	$(RM) $(OBJ_FILES) $(SOURCE_DEP) $(BIN)

# source code
$(OBJ)%.o: $(SOURCE)%.cpp
	$(CXX) -o $@ -MMD $(CFLAGS) $<

# code dependencies
-include $(SOURCE_DEP)
