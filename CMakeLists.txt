cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CXX_FLAGS "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(glfw3 3.3 REQUIRED)

project(main)
add_subdirectory(include/glad)
add_subdirectory(include/glm)
add_executable(main src/main.cpp src/Shader.cpp src/Shader.h include/stb_image.h include/stb_image.cpp src/Camera.h)
target_link_libraries(main PRIVATE glfw)
target_link_libraries(main PRIVATE glad)
target_link_libraries(main PRIVATE glm)
add_custom_target(run
        COMMAND main
        DEPENDS main
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
        )
