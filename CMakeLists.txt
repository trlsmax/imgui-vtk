cmake_minimum_required (VERSION 3.6)
project (imgui-vtk)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_VERBOSE_MAKEFILE 1)
set (APP_VERSION_MAJOR 1)
set (APP_VERSION_MINOR 0)

set (EXECNAME imgui_vtk)


find_package(VTK COMPONENTS 
  vtkCommonCore
  vtkCommonDataModel
  vtkFiltersCore
  vtkInteractionStyle
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingGL2PSOpenGL2
  vtkRenderingOpenGL2
  QUIET
)

if (NOT VTK_FOUND)
  message("Skipping ImageDataToQImage: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")

# glfw
list (APPEND CMAKE_MODULE_PATH "${${PROJECT_NAME}_SOURCE_DIR}")
find_package(GLFW3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})
link_libraries(${GLFW_LIBRARY_DIRS})

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

add_executable (${EXECNAME} 
    "${${PROJECT_NAME}_SOURCE_DIR}/main.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/glad/glad.c"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui_impl_vtk.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui_demo.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui_draw.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui_widgets.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp"
)

target_include_directories (${EXECNAME}
  PUBLIC
    "${${PROJECT_NAME}_SOURCE_DIR}"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui"
)

if (UNIX)
  if (APPLE)
      target_link_libraries (
         ${EXECNAME}
              "-framework Cocoa"
              "-framework IOKit"
              "-framework CoreVideo"
              glfw
              ${VTK_LIBRARIES}
      )
  else ()
      target_link_libraries (
         ${EXECNAME}
              ${CMAKE_THREAD_LIBS_INIT}
              ${X11_LIBRARIES}
              ${CMAKE_DL_LIBS}
              glfw
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