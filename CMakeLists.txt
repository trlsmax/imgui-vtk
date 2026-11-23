cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(imgui-vtk VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_VERBOSE_MAKEFILE 1)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(IMGUI_VTK_BUILD_EXAMPLE "Build the imgui-vtk demo" ON)

# OpenGL
find_package(OpenGL REQUIRED)

# Dependencies: use CMake packages when configured with a vcpkg toolchain,
# otherwise build the gl3w / glfw / imgui submodules from source.
if(DEFINED VCPKG_TARGET_TRIPLET)
	find_package(imgui CONFIG REQUIRED)
	find_package(glfw3 CONFIG REQUIRED)
	find_package(gl3w CONFIG REQUIRED)
	# Reuse the package targets under the names used below
	add_library(imgui INTERFACE)
	target_link_libraries(imgui INTERFACE imgui::imgui)
	add_library(gl3w INTERFACE)
	target_link_libraries(gl3w INTERFACE unofficial::gl3w::gl3w)
else()

# OpenGL Loader - GL3W
# This can be replaced with your own OpenGL Loader
# Either compile it as a static library like this example
# Or add it to the source files for the main executable + imgui + vtkviewer
set(gl3w_dir ${CMAKE_CURRENT_SOURCE_DIR}/gl3w)
set(gl3w_inc ${gl3w_dir}/include)
set(gl3w_src ${gl3w_dir}/src/gl3w.c)
if (EXISTS ${gl3w_src})
	message(STATUS "Found gl3w source files")
else()
	find_program(GL3W_PYTHON NAMES python3 python)
	if (NOT GL3W_PYTHON)
		message(FATAL_ERROR "gl3w_gen.py needs Python 3 (python3 or python) on PATH")
	endif()
	execute_process(COMMAND ${GL3W_PYTHON} gl3w_gen.py WORKING_DIRECTORY ${gl3w_dir})
	if (NOT EXISTS ${gl3w_src})
		message(FATAL_ERROR "Could not generate gl3w source files")
	endif()
endif()
add_library(gl3w STATIC ${gl3w_src})
target_include_directories(gl3w PUBLIC ${gl3w_inc})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)

# Dear ImGui
set(imgui_dir ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file(GLOB imgui_src ${imgui_dir}/*.cpp)
set(imgui_backend ${imgui_dir}/backends/imgui_impl_glfw.cpp ${imgui_dir}/backends/imgui_impl_opengl3.cpp)
add_library(imgui STATIC ${imgui_src} ${imgui_backend})
target_include_directories(imgui PUBLIC ${imgui_dir} ${imgui_dir}/backends)
target_link_libraries(imgui glfw)

endif()

# VTK
include(cmake/vtk-components.cmake)
find_package(VTK COMPONENTS 
  ${IMGUI_VTK_VTK_COMPONENTS}
  QUIET
)
if (NOT VTK_FOUND)
  message(FATAL_ERROR "VTK not found!")
  return()
endif()
message (STATUS "VTK Version: ${VTK_VERSION}")

# imgui-vtk (VTK Viewer class)
add_library(imgui_vtk STATIC src/VtkViewer.cpp)
target_include_directories(imgui_vtk PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/imgui-vtk>
)
target_link_libraries(imgui_vtk PUBLIC
  $<BUILD_INTERFACE:imgui>
  $<BUILD_INTERFACE:gl3w>
  $<BUILD_INTERFACE:glfw>
  ${VTK_LIBRARIES}
)

install(TARGETS imgui_vtk EXPORT imgui-vtk-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES include/VtkViewer.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/imgui-vtk)
install(EXPORT imgui-vtk-targets
  FILE imgui-vtk-targets.cmake
  NAMESPACE imgui-vtk::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/imgui-vtk
)
configure_package_config_file(cmake/imgui-vtk-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/imgui-vtk-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/imgui-vtk
)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/imgui-vtk-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/imgui-vtk-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/imgui-vtk-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/imgui-vtk
)

if(IMGUI_VTK_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()
