cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CXX_FLAGS "-Wall -std=c++17")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(glfw3 3.3 REQUIRED)

project(main)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

add_subdirectory(include/glad)
add_subdirectory(include/glm)
add_executable(main src/main.cpp src/Shader.cpp src/Shader.h include/stb_image.h include/stb_image.cpp src/Camera.h src/Mesh.cpp src/Mesh.h src/Model.cpp src/Model.h src/filesystem.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB LIBRARIES "include/*.dylib")
message("LIBRARIES = ${LIBRARIES}")

link_directories(include)
link_libraries(main assimp)
target_link_libraries(main PRIVATE ${LIBRARIES})
target_link_libraries(main PRIVATE glfw)
target_link_libraries(main PRIVATE glad)
target_link_libraries(main PRIVATE glm)

add_custom_target(run
        COMMAND main
        DEPENDS main
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
        )
