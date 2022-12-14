cmake_minimum_required(VERSION 3.20)

project(HelloOpenGLPortToDX LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE PROJ_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/include/*.hpp")
file(GLOB_RECURSE PROJ_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/lib/*.cpp")

if(WIN32)
add_executable(HelloOpenGLPortToDX WIN32 ${PROJ_SOURCES})
target_include_directories(HelloOpenGLPortToDX PRIVATE "./src/include" "./vendor/glfw/include")
#target_link_libraries(HelloOpenGLPortToDX PRIVATE )
endif()