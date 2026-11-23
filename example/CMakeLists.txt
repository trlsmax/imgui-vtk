add_executable(imgui_vtk_demo main.cpp imgui_vtk_demo.h)
target_link_libraries(imgui_vtk_demo PRIVATE imgui_vtk glfw gl3w OpenGL::GL)

if (APPLE)
	# Ignore macOS OpenGL deprecation warnings
	target_compile_definitions(imgui_vtk_demo PRIVATE GL_SILENCE_DEPRECATION)
endif()

# VTK Module init
if (VTK_VERSION VERSION_LESS "9.0.0")
  include(${VTK_USE_FILE})
else()
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS imgui_vtk_demo
    MODULES ${VTK_LIBRARIES}
  )
endif()

# GLFW is built from source in this example
# But if you link dynamically, you may need to link some native libraries on macOS:
# target_link_libraries(imgui_vtk_demo "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
if (UNIX AND NOT APPLE)
  target_link_libraries(imgui_vtk_demo ${CMAKE_THREAD_LIBS_INIT} ${X11_LIBRARIES} ${CMAKE_DL_LIBS})
endif()
