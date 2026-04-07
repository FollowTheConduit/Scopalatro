
CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Ideps/TLOT/deps -Ideps/TLOT/deps/glad/include -Ideps/TLOT/include -Iinclude
CXXFLAGS += -O0 -g

BIN_WINDOWS := bin/scopalatro.exe
LIBS_WINDOWS:= -ltetoEngine -lglfw3 -lgdi32 -lopengl32 -lstdc++exp


SRC := $(wildcard src/*.cpp)

ifeq ($(OS), Windows_NT)
LIBDIRS	:= -Ldeps/TLOT/libs/windows/glfw3 -Ldeps/TLOT/bin/lib
LIBS	:= $(LIBS_WINDOWS)
BIN	    := $(BIN_WINDOWS)
else
LIBDIRS	:= # TODO
LIBS	:= $(LIBS_LINUX) #TODO
BIN 	:= $(BIN_LINUX)
endif

OBJ := $(SRC:.cpp=.o)

all : $(BIN)

$(BIN) : $(OBJ)
	$(CXX) $(OBJ) $(LIBDIRS) $(LIBS) -o $(BIN)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
ifeq ($(OS), Windows_NT)
	del /q /s src\*.o scopalatro.exe
else
	rm -rf $(OBJ) $(BIN)
endif