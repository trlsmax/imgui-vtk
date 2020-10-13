cmake_minimum_required (VERSION 3.6)
project (imgui-vtk)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_VERBOSE_MAKEFILE 1)
set (APP_VERSION_MAJOR 1)
set (APP_VERSION_MINOR 0)
set (LIBNAME vtkDearImGuiVport)
set (EXECNAME vtkDearImGuiDemo)


find_package(VTK COMPONENTS 
  vtkCommonCore
  vtkCommonDataModel
  vtkFiltersCore
  vtkIOExport
  vtkIOExportPDF
  vtkIOXML
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

add_library (${LIBNAME} "")
target_sources (${LIBNAME}
  PRIVATE
    "${${PROJECT_NAME}_SOURCE_DIR}/vtkDearImGuiVport.cxx"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui_demo.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui_draw.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui/imgui_widgets.cpp"
)

target_include_directories (${LIBNAME}
  INTERFACE
    "${${PROJECT_NAME}_SOURCE_DIR}"
    "${${PROJECT_NAME}_SOURCE_DIR}/imgui"
)

if (UNIX)
  if (APPLE)
      target_link_libraries (
         ${LIBNAME}
          PUBLIC
              "-framework Cocoa"
              "-framework IOKit"
              "-framework CoreVideo"
              glfw
              ${VTK_LIBRARIES}
      )
  else ()
      target_link_libraries (
         ${LIBNAME}
          PUBLIC
              ${CMAKE_THREAD_LIBS_INIT}
              ${X11_LIBRARIES}
              ${CMAKE_DL_LIBS}
              glfw 
              ${VTK_LIBRARIES}
      )
  endif()
else ()
  target_link_libraries (
     ${LIBNAME}
      PUBLIC
          glfw
          ${VTK_LIBRARIES}
  )
endif()

if (APPLE)
  add_executable (${EXECNAME} MACOSX_BUNDLE)
elseif (WIN32)
  add_executable (${EXECNAME} WIN32)
endif ()

target_sources (${EXECNAME}
  PRIVATE
    "${${PROJECT_NAME}_SOURCE_DIR}/main.cpp"
)


if (UNIX)
  if (APPLE)
      target_link_libraries (
         ${EXECNAME}
          PUBLIC
              "-framework Cocoa"
              "-framework IOKit"
              "-framework CoreVideo"
              glfw
              ${VTK_LIBRARIES}
              ${LIBNAME}
      )
  else ()
      target_link_libraries (
         ${EXECNAME}
          PUBLIC
              ${CMAKE_THREAD_LIBS_INIT}
              ${X11_LIBRARIES}
              ${CMAKE_DL_LIBS}
              glfw 
              ${VTK_LIBRARIES}
              ${LIBNAME}
      )
  endif()
else ()
  target_link_libraries (
     ${EXECNAME}
      PUBLIC
          glfw
          ${VTK_LIBRARIES}
          ${LIBNAME}
  )
endif()


if (VTK_VERSION VERSION_LESS "9.0.0")
  message(FATAL_ERROR "Unsupported VTK version < 9.0.0")
else ()
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${LIBNAME}
    MODULES ${VTK_LIBRARIES}
    )
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${EXECNAME}
    MODULES ${VTK_LIBRARIES}
    )
endif () 