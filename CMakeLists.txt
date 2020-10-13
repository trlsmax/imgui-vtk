cmake_minimum_required(VERSION 3.6)
project(imgui-vtk)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE 1)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)

set(APP_TARGET imgui-vtk)

add_subdirectory(imgui)
include_directories(${PROJECT_SOURCE_DIR} imgui)
add_executable(${APP_TARGET} main.cpp vtk.cpp)

if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    SET(EXTRA_LIBS ${COCOA_LIBRARY} ${OpenGL_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif (APPLE)

if (WIN32)
    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
endif (WIN32)

# glfw
find_package(glfw3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})
link_libraries(${GLFW_LIBRARY_DIRS})

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})


find_package(VTK REQUIRED)
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
else ()
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${APP_TARGET}
    MODULES ${VTK_LIBRARIES}
    )
endif () 

target_link_libraries(
        ${APP_TARGET}
        ${GLFW3_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${EXTRA_LIBS}
        ${VTK_LIBRARIES}
        imgui
)
