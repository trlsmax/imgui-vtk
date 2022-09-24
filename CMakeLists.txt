cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(imgui-vtk)

set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_VERBOSE_MAKEFILE 1)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(EXEC_NAME imgui_vtk)

# In this CMakeLists.txt file, components are all built separately and linked together
# If you prefer to compile source files + dependencies (OpenGL loaders, imgui, VtkViewer, etc.)
# together, please see CMakeLists-alt.txt
add_executable(${EXEC_NAME} main.cpp)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${EXEC_NAME} OpenGL::GL)
if (APPLE)
	# Ignore macOS OpenGL deprecation warnings
	target_compile_definitions(${EXEC_NAME} PRIVATE GL_SILENCE_DEPRECATION)
endif()

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
	execute_process(COMMAND python3 gl3w_gen.py WORKING_DIRECTORY ${gl3w_dir}) # use python or python3
	if (NOT EXISTS ${gl3w_src})
		message(FATAL_ERROR "Could not generate gl3w source files")
	endif()
endif()
add_library(gl3w STATIC ${gl3w_src})
target_include_directories(gl3w PUBLIC ${gl3w_inc})
target_link_libraries(${EXEC_NAME} gl3w)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)
target_link_libraries(${EXEC_NAME} glfw)

# Dear ImGui
set(imgui_dir ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file(GLOB imgui_src ${imgui_dir}/*.cpp)
set(imgui_backend ${imgui_dir}/backends/imgui_impl_glfw.cpp ${imgui_dir}/backends/imgui_impl_opengl3.cpp)
add_library(imgui STATIC ${imgui_src} ${imgui_backend})
target_include_directories(imgui PUBLIC ${imgui_dir} ${imgui_dir}/backends)
target_link_libraries(imgui glfw)
target_link_libraries(${EXEC_NAME} imgui)

# VTK
find_package(VTK COMPONENTS 
  CommonCore
  CommonColor
  CommonDataModel
  FiltersCore
  InteractionStyle
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
  QUIET
)
if (NOT VTK_FOUND)
  message(FATAL_ERROR "VTK not found!")
  return()
endif()

if (VTK_VERSION VERSION_LESS "9.0.0")
  include(${VTK_USE_FILE})
else()
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${EXEC_NAME}
    MODULES ${VTK_LIBRARIES}
  )
endif()
message (STATUS "VTK Version: ${VTK_VERSION}")

# imgui-vtk (VTK Viewer class)
set(imgui_vtk_viewer_dir ${CMAKE_CURRENT_SOURCE_DIR})
add_library(imgui_vtk_viewer STATIC ${imgui_vtk_viewer_dir}/VtkViewer.cpp)
target_include_directories(imgui_vtk_viewer PRIVATE ${imgui_vtk_viewer_dir})
target_link_libraries(imgui_vtk_viewer gl3w) # Since gl3w was compiled as a static library, we need to link to it
target_link_libraries(imgui_vtk_viewer imgui) # Since imgui was compiled as a static library, we need to link to it
target_link_libraries(imgui_vtk_viewer ${VTK_LIBRARIES})
target_link_libraries(${EXEC_NAME} imgui_vtk_viewer)

# GLFW is built from source in this example
# But if you link dynamically, you may need to link some native libraries on macOS:
# target_link_libraries(${EXEC_NAME} "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
if (UNIX AND NOT APPLE)
  target_link_libraries(${EXEC_NAME} ${CMAKE_THREAD_LIBS_INIT} ${X11_LIBRARIES} ${CMAKE_DL_LIBS})
endif()
