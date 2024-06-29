cmake_minimum_required(VERSION 3.27)
project(DistanceMeasuresHMT)

add_subdirectory(lib/glm)
add_subdirectory(lib/glfw-3.4)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJ_SOURCES
        src/algorithms/dtw.cpp src/algorithms/edit_distance.cpp src/algorithms/frechet.cpp src/algorithms/lcss.cpp
        src/algorithms/trajectory_analysis.cpp
        src/app/main.cpp src/app/trajectory_analysis_manager.cpp src/app/motion_file_processor.cpp src/app/engine/display_requirments.cpp
        src/app/engine/engine.cpp
        src/imgui/image_loader.cpp src/imgui/imgui_layers.cpp src/imgui/my_layers/imgui_layer.cpp src/imgui/my_layers/classifier_layer.cpp
        src/imgui/my_layers/result_layer.cpp src/imgui/my_layers/setup_layer.cpp src/misc/enums.cpp src/opengl/shader.cpp src/opengl/sphere.cpp
        src/parsing/input_parser.cpp
        src/imgui_impl_opengl3.cpp
)

set(LIBS_SOURCES
        imgui/example_implot.cpp
        imgui/imgui_impl_glfw.cpp
        imgui/implot.cpp
        imgui/implot_demo.cpp
        imgui/implot_items.cpp
        # this one too?
        misc/cpp/imgui_stdlib.cpp

        # But all of the essential ImGui .cpp files in here
        # imgui/imgui.cpp etc...
        # maybe even glew/gl.c or anything like that
)

add_executable(${PROJECT_NAME} ${PROJ_SOURCES} ${LIBS_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC BEFORE include/ libs/)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
