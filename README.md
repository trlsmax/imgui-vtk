
# VTK adapter for DearImGUI

* An effort to incorporate VTK's dataset visualization capabilities with versatility of DearImGUI.
* Renders VTK content into a dedicated Dear ImGui window. (Like a viewport)
* Use `imgui_impl_vtk.cpp`, `imgui_impl_vtk.h` as you would any of `imgui_impl_xxx.h" files.
* Set up pipeline (datasets + filter) with VTK. dataset->filter->...->mapper->actor.
* Init Imgui VTK `ImGui_ImplVTK_Init()`
* Provide actors to `ImGui_ImplVTK_AddActor(vtkProp*)`
* Call ImGui_ImplVTK_Render() in event loop.
* Look in `main.cpp`.

![](vtkImGuiDemo.gif)

## Usage:

```
dataset = ..
filter = ..
mapper1 = ..
mapper2 = ..
actor1 = ..
actor2 = ..
filter->SetInputData(dataset);

mapper1->SetInputConnection(filter->GetOutputPort(0));
mapper2->SetInputConnection(filter->GetOutputPort(1));
...

actor1->SetMapper(mapper1);
actor2->SetMapper(mapper2);
...

ImguiInit...
ImGui_ImplVTK_Init()

while (!terminate)
{
  Imgui new frame..
  custom imgui windows..
  ImGui_ImplVTK_Render();
  ImGui::Render();
  ImGui_Impl_xxx_RenderDrawData();
}
```