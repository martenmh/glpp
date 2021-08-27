cmake_minimum_required(VERSION 3.6)
project(glpp)
enable_language(CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_CXX_STANDARD 20)
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG "_DEBUG")

add_library(utils INTERFACE)
target_include_directories(utils INTERFACE lib/utils/include)
add_library(ml INTERFACE)
target_include_directories(ml INTERFACE lib/math-lib2/include)

find_package(glm)   # OpenGL Math Library
# Rendering (OpenGL)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED) # Extension Wrangler
find_package(glfw3 REQUIRED) # Native OS io
find_package(Freetype REQUIRED) # Text in OpenGL
find_package(assimp REQUIRED)   # Object loading

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O0")

set(Libraries
        PRIVATE ml
        PRIVATE utils
        ${OPENGL_LIBRARIES}
        glfw
        ${FREETYPE_LIBRARIES}
        ${GLEW_LIBRARIES}
        OpenGL::GL
        ${CMAKE_DL_LIBS}
        ${ASSIMP_LIBRARIES})

set(ImguiHeaders
        lib/imgui
        lib/imgui/backends)

set(ImguiFiles
        lib/imgui/imgui.cpp
        lib/imgui/imgui_demo.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/backends/imgui_impl_glfw.cpp
        lib/imgui/backends/imgui_impl_opengl3.cpp)

set(ProjectFiles lib/glad/src/glad.c)

include_directories(include/ lib/ lib/stb_image lib/glad/include glfw ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS} ${ImguiHeaders})


add_executable(engine src/main.cpp ${ProjectFiles} ${ImguiFiles})
target_link_libraries(engine ${Libraries})

# Examples:
# 2D scene example
add_executable(scene examples/scene.cpp ${ProjectFiles} ${ImguiFiles})
target_link_libraries(scene ${Libraries})

# Sorting visualization
add_executable(sorting examples/sorting.cpp ${ProjectFiles} ${ImguiFiles})
target_link_libraries(sorting ${Libraries})

include_directories(${GTEST_INCLUDE_DIRS} include/ lib/ lib/stb_image lib/glad/include glfw ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS} ${ImguiHeaders})

add_executable(runTests test/tests.cpp)
target_link_libraries(runTests ${Libraries} ${GTEST_LIBRARIES} gtest_main )


include_directories(include/ lib/ lib/stb_image lib/glad/include glfw ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS} ${ImguiHeaders})

add_executable(runBench test/bench.cpp ${ProjectFiles} ${ImguiFiles})
target_link_libraries(runBench PUBLIC benchmark::benchmark ${Libraries})
