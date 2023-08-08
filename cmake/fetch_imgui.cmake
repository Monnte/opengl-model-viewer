cmake_minimum_required(VERSION 3.18)

include(FetchContent)

FetchContent_Declare(
    imgui-github
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.89.8
)

FetchContent_MakeAvailable(imgui-github)

add_library(imgui STATIC)
target_sources(imgui PRIVATE ${imgui-github_SOURCE_DIR}/imgui.cpp)
target_sources(imgui PRIVATE ${imgui-github_SOURCE_DIR}/imgui_demo.cpp)
target_sources(imgui PRIVATE ${imgui-github_SOURCE_DIR}/imgui_draw.cpp)
target_sources(imgui PRIVATE ${imgui-github_SOURCE_DIR}/imgui_widgets.cpp)
target_sources(imgui PRIVATE ${imgui-github_SOURCE_DIR}/imgui_tables.cpp)
target_sources(imgui PRIVATE ${imgui-github_SOURCE_DIR}/imgui_widgets.cpp)
target_sources(imgui PRIVATE ${imgui-github_SOURCE_DIR}/imgui_tables.cpp)
target_sources(imgui PRIVATE ${imgui-github_SOURCE_DIR}/imgui_widgets.cpp)
target_sources(imgui PRIVATE ${imgui-github_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
target_sources(imgui PRIVATE ${imgui-github_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC ${imgui-github_SOURCE_DIR})
target_include_directories(imgui PUBLIC ${imgui-github_SOURCE_DIR}/backends)

add_library(imgui::imgui ALIAS imgui)