# Project configuration
PROJECTNAME = project
OUTPUT_DIR = build

# Directories
INCLUDE_DIRS = -Iinclude -Iinclude/imgui -Iinclude/glm -Iinclude/glew
LIB_DIRS = -Llib -L/usr/local/lib -L/usr/lib -L/usr/lib64

# Libraries
LIBS = -l:libglfw3dll.a -lvulkan-1.dll -l:libimgui_docker.dll.a -lglew32 -lopengl32 my.res

# Source files
SRC = $(wildcard src/*.cpp) $(wildcard imgui/*.cpp)

# Compiler and flags
CXX = g++
CXXFLAGS = -O3

# Default target
default: $(OUTPUT_DIR)/$(PROJECTNAME)

# Build the project
$(OUTPUT_DIR)/$(PROJECTNAME): $(SRC)
	@mkdir -p $(OUTPUT_DIR)
	$(CXX) $(SRC) -o $@ $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(CXXFLAGS)

# Run the project
run: $(OUTPUT_DIR)/$(PROJECTNAME)
	$(OUTPUT_DIR)/$(PROJECTNAME)

# Clean build files
clean:
	rm -rf $(OUTPUT_DIR)

# Phony targets
.PHONY: default run clean
