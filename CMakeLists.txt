cmake_minimum_required(VERSION 3.7...3.20)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(main VERSION 1.0
             DESCRIPTION "Space Game"
             LANGUAGES CXX)

set(CXX_FLAGS "-Wall -std=c++17")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(glfw3 3.3 REQUIRED)


configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

add_subdirectory(include/glad)
add_subdirectory(include/glm)
link_directories(/usr/local/lib)
add_executable(main src/main.cpp src/Shader.cpp src/Shader.h include/stb_image.h include/stb_image.cpp src/Camera.h src/Mesh.cpp src/Mesh.h src/Model.cpp src/Model.h src/filesystem.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB LIBRARIES "include/*.dylib")
message("LIBRARIES = ${LIBRARIES}")

link_directories(include)
link_libraries(main assimp)

#link_libraries(main luajit-2.1.0-beta3)
target_link_libraries(main PRIVATE ${LIBRARIES})
target_link_libraries(main PRIVATE glfw)
#target_include_directories(main PUBLIC lua)
target_include_directories(main PUBLIC include/luajit/src)
target_link_libraries(main PUBLIC luajit-5.1)

target_link_libraries(main PRIVATE glad)

target_link_libraries(main PRIVATE glm)

add_custom_target(run
        COMMAND main
        DEPENDS main
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
        )
