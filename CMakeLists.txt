cmake_minimum_required(VERSION 3.18)

project(project LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(EXECUTABLE_NAME main)

include(fetch_glfw)
include(fetch_glad)
include(fetch_glm)
include(fetch_stb_image)
include(fetch_assimp)
include(fetch_imgui)


add_executable(${EXECUTABLE_NAME})
FILE(GLOB_RECURSE SRC_FILES src/*.cpp)
target_sources(${EXECUTABLE_NAME} PRIVATE ${SRC_FILES})
target_include_directories(${EXECUTABLE_NAME} PUBLIC include)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC glfw glad glm stb_image assimp imgui)


set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_command(
        TARGET ${EXECUTABLE_NAME}
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR})
endif()
