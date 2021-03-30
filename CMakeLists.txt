cmake_minimum_required (VERSION 3.6)
project (imgui-vtk)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_VERBOSE_MAKEFILE 1)
set (APP_VERSION_MAJOR 1)
set (APP_VERSION_MINOR 0)

set (EXECNAME imgui_vtk)


find_package(ParaView
  QUIET
)
if (NOT ParaView_FOUND)
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
    message("Skipping imgui-vtk: ${VTK_NOT_FOUND_MESSAGE}")
    return ()
  endif()
endif()

message (STATUS "VTK_VERSION: ${VTK_VERSION}")

# glfw
list (APPEND CMAKE_MODULE_PATH "${${PROJECT_NAME}_SOURCE_DIR}")
find_package(glfw3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})
link_libraries(${GLFW_LIBRARY_DIRS})

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

set(HEADER_FILES
    imgui_impl_vtk.h
    imgui_vtk_demo.h
)

set(SOURCE_FILES
    main.cpp
    glad/glad.c
    imgui_impl_vtk.cpp
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
)

add_executable (${EXECNAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories (${EXECNAME}
  PUBLIC
    "${${PROJECT_NAME}_SOURCE_DIR}"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui"
)

target_compile_definitions(${EXECNAME} PRIVATE "IMGUI_IMPL_OPENGL_LOADER_GLAD")

if (UNIX)
  if (APPLE)
      target_link_libraries (
         ${EXECNAME}
              "-framework Cocoa"
              "-framework IOKit"
              "-framework CoreVideo"
              glfw
              OpenGL::GL
              ${VTK_LIBRARIES}
      )
  else ()
      target_link_libraries (
         ${EXECNAME}
              ${CMAKE_THREAD_LIBS_INIT}
              ${X11_LIBRARIES}
              ${CMAKE_DL_LIBS}
              glfw
              OpenGL::GL
              ${VTK_LIBRARIES}
      )
  endif()
else ()
  target_link_libraries (
     ${EXECNAME}
          glfw
          OpenGL::GL
          ${VTK_LIBRARIES}
  )
endif()

if (VTK_VERSION VERSION_LESS "9.0.0")
  include(${VTK_USE_FILE})
else ()
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${EXECNAME}
    MODULES ${VTK_LIBRARIES}
    )
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${EXECNAME}
    MODULES ${VTK_LIBRARIES}
    )
endif () 