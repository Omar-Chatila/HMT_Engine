cmake_minimum_required(VERSION 3.27)
project(DistanceMeasuresHMT)

set(CMAKE_CXX_STANDARD 23)

# Add -O3 optimization flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Add the filesystem library for GCC and Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
endif ()

add_subdirectory(lib/glm)
add_subdirectory(lib/glfw-3.4)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define your project sources, excluding src/benchmarks
set(PROJ_SOURCES
        src/algorithms/dtw.cpp
        src/algorithms/edit_distance.cpp
        src/algorithms/frechet.cpp
        src/algorithms/lcss.cpp
        src/algorithms/trajectory_analysis.cpp
        src/app/main.cpp
        src/app/trajectory_analysis_manager.cpp
        src/app/motion_file_processor.cpp
        src/app/engine/engine.cpp
        src/imgui/layer.cpp
        src/imgui/image_loader.cpp
        src/imgui/imgui_layers.cpp
        src/imgui/my_layers/imgui_layer.cpp
        src/imgui/my_layers/classifier_layer.cpp
        src/imgui/my_layers/result_layer.cpp
        src/imgui/my_layers/setup_layer.cpp
        src/misc/enums.cpp
        src/opengl/shader.cpp
        src/opengl/sphere.cpp
        src/parsing/input_parser.cpp
        src/imgui_impl_opengl3.cpp
)

# Add ImGui and other libraries sources
set(LIBS_SOURCES
        imgui/imgui_impl_glfw.cpp
        imgui/implot.cpp
        imgui/implot_items.cpp
        imgui/example_implot.cpp
        imgui/implot_demo.cpp
        misc/cpp/imgui_stdlib.cpp
        include/imgui/imgui_neo_sequencer.cpp
        include/imgui/imgui_neo_internal.cpp
)

# Create the executable target
add_executable(${PROJECT_NAME} ${PROJ_SOURCES} ${LIBS_SOURCES})
#set_target_properties(${PROJECT_NAME} RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Specify include directories
target_include_directories(${PROJECT_NAME} PUBLIC
        include/imgui/
        include/
        include/Eigen/
        lib/
        src/
        src/app/
        src/data/
        src/algorithms/
        src/app/engine/
        src/imgui/
        src/opengl/
        src/parsing/
        src/misc/
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/libimgui_docker.dll.a)

# Link against OpenGL libraries
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# Link against GLEW
find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)

# Additional system libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_DL_LIBS} # For dlfcn.h
)

# Windows-specific libraries
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            opengl32 # Link against OpenGL32 library explicitly on Windows
    )
endif ()
