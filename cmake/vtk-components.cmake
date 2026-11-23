# VTK modules required by VtkViewer. This list is the single source of truth for
# both the in-tree build and the installed package config.
set(IMGUI_VTK_VTK_COMPONENTS
  CommonCore
  CommonColor
  CommonDataModel
  FiltersCore
  InteractionStyle
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
)
# Rendered into imgui-vtk-config.cmake by configure_package_config_file().
string(REPLACE ";" "\n  " IMGUI_VTK_VTK_COMPONENTS_TEXT "${IMGUI_VTK_VTK_COMPONENTS}")
