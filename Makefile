# Project configuration
PROJECT_NAME = DistanceMeasuresHMT
PROJECT_VERSION = 0.0.1
TARGET := bin/$(PROJECT_NAME)_$(PROJECT_VERSION)
OUTPUT_DIR = build

# Directories
INCLUDE_DIRS = -Iinclude -Iinclude/imgui -Iinclude/src/imgui/my_layers -Iinclude/glm -Iinclude/glew -Isrc -Isrc/algorithms -Isrc/app -Isrc/app/engine -Isrc/imgui -Isrc/misc -Isrc/opengl -Isrc/parsing -Isrc/tests -Ibenchmark-main/include
LIB_DIRS = -Llib -L/usr/local/lib -L/usr/lib -L/usr/lib64 -Lbenchmark-main/cmake-build-release/src

# Libraries
LIBS = -l:libbenchmark.a -l:libglfw3dll.a -lvulkan-1.dll -l:libimgui_docker.dll.a -lglew32 -lpthread -lopengl32 my.res

# Source files
SRC = $(wildcard imgui/*.cpp) $(shell find src -name '*.cpp' -not -path 'src/benchmarks/*')
OBJ = $(SRC:%.cpp=$(OUTPUT_DIR)/%.o)

# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -MMD -MP -std=c++23

# Dependency files
DEP = $(OBJ:.o=.d)

# Include dependency files
-include $(DEP)

# Ensure the OUTPUT_DIR exists
$(OUTPUT_DIR):
	@mkdir -p $(OUTPUT_DIR)

# Default target
default: $(TARGET)

all: default

debug: CXXFLAGS += -g
debug: $(TARGET)

# Build the project
$(TARGET): $(OUTPUT_DIR) $(OBJ)
	@echo "Building project..."
	$(CXX) $(OBJ) -o $@ $(LIB_DIRS) $(LIBS) $(CXXFLAGS)

# Build object files
$(OUTPUT_DIR)/%.o: %.cpp | $(OUTPUT_DIR)
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Run the project
run: $(TARGET)
	$(TARGET)

# Clean build files
clean:
	rm -rf $(OUTPUT_DIR) $(TARGET)

# Phony targets
.PHONY: default all run clean

# Debug print statements
print-debug:
	@echo "Source files: $(SRC)"
	@echo "Object files: $(OBJ)"
