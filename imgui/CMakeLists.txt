cmake_minimum_required(VERSION 3.6)

if (WIN32)
    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
endif (WIN32)

# glfw
find_package(glfw3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})
link_libraries(${GLFW_LIBRARY_DIRS})

add_library(
        imgui STATIC
        imconfig.h
        imgui.cpp
        imgui.h
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_impl_glfw.cpp
        imgui_impl_glfw.h
        imgui_impl_opengl2.cpp
        imgui_impl_opengl2.h
        imgui_internal.h
        imgui_widgets.cpp
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h
        )
