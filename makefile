# Makefile

# Compilador
CXX = g++

# Flags do compilador
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++11

# Diretórios
INCLUDE_DIR = include
SRC_DIR = src
MAIN_DIR = .
BUILD_DIR = build

# Arquivos fonte e objeto
SOURCES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(MAIN_DIR)/*.cpp)
OBJECTS_WITH_PATH = $(SOURCES:.cpp=.o)
OBJECTS_NAME = $(notdir $(OBJECTS_WITH_PATH))

# Nome do executável
EXEC = myprogram

# Regra padrão
all: $(BUILD_DIR)/$(EXEC)

$(BUILD_DIR)/$(EXEC): $(OBJECTS_WITH_PATH) $(BUILD_DIR)
	$(CXX) $(patsubst %, $(BUILD_DIR)/%, $(OBJECTS_NAME)) -o $(BUILD_DIR)/$(EXEC) -lncurses

%.o: $(BUILD_DIR) %.cpp
	$(CXX) $(CXXFLAGS) -c $(word 2, $^) -o $(BUILD_DIR)/$(notdir $@)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(OBJECTS_WITH_PATH) $(EXEC) $(BUILD_DIR)

run: clean all
	./$(BUILD_DIR)/$(EXEC)

rebuild: clean all