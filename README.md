# imgui-vtk
- Incorporate VTK's dataset visualization capabilities with versatility of Dear ImGui
  - Multiple independent `VtkViewer` instances can be used in the same program to display multiple VTK scenes simultaneously
- Uses VTK's OpenGLRenderWindow class
  - Renders to texture and displays the texture natively with ImGui
- Compatible with both ImGui's `main` and `docking` branches

![](vtkImGuiDemo.gif)

## Usage:
- CMake
  1. Build separately & link
    - Build your executable, imgui, your OpenGL Loader (e.g., gl3w), and imgui-vtk. **Make sure to link imgui-vtk against imgui, your OpenGL Loader (e.g., gl3w), and VTK!**
    - See `CMakeLists.txt` for more details
  2. Build together
    - Build your executable, imgui, your OpenGL Loader (e.g., gl3w), and imgui-vtk all together, from source.
    - See `CMakeLists-alt.txt` for more details
- See `main.cpp`

## Notes
- `imgui`, `gl3w`, and `glfw` are included in this repository as git submodules
  - For integration into an existing project, only `VtkViewer.h` and `VtkViewer.cpp` are needed. However, they will need to be linked with or built alongside Dear ImGui and VTK
- Dependencies are built separately in `CMakeLists.txt` then linked together as static libraries
  - `CMakeLists-alt.txt` builds everything together from source
- Usage (see [`main.cpp`](main.cpp) for details)
  - You must create and end ImGui windows yourself
    - Allows for other widgets to be placed in the same ImGui window as the VTK "viewport"
    - Allows for full control over window size, behavior, etc. via Dear ImGui API
- **Note: For the sake of cleanliness and readability, most ImGui preprocessor directives were removed.** Files no longer auto-detect your OpenGL loader. While everything is currently set up for OpenGL3 + GLFW + GL3W, you may need to adjust `#include` statements, etc. to match your use case.
