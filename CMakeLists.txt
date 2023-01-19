cmake_minimum_required(VERSION 3.20)

project(HelloOpenGLPortToDX LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE PROJ_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/include/*.hpp")
file(GLOB_RECURSE PROJ_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/lib/*.cpp")

add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/assets
    ${PROJECT_BINARY_DIR}/assets
    COMMENT "Copying shaders to binary directory"
)

if(WIN32)
add_executable(HelloOpenGLPortToDX WIN32 ${PROJ_SOURCES} ${PROJ_HEADERS})
target_include_directories(HelloOpenGLPortToDX PRIVATE "./src/include")
target_link_libraries(HelloOpenGLPortToDX PRIVATE d3d11 d3d12 d3d9 dxgi dxguid d3dcompiler)
add_dependencies(HelloOpenGLPortToDX copy_resources)
endif()