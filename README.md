
# Improvements to https://github.com/trlsmax/imgui-vtk. Use new capabilities in VTK >= 9.0 

* An effort to incorporate VTK's dataset visualization capabilities with versatility of DearImGUI.
* It is expected that usage of this filter will be a mix of the usual VTK's style(pipeline setup) and DearImGUI's immediate-style.
* This is meant to be as non-intrusive as possible to existing projects using DearImGUI, 
*   yet capable of building a full-fledged app with vtk + ImGUi + vtkDearImGuiVport. 
* vtkDearImGuiVport does not use a concrete platform windowing system such as Qt/Glfw/Sdl/Xwindow/Cocoa/Win32/wx/Gtk.
* Instead, it renders into a DearImGui window with a call to ImGui::Image((void*)id, ..., ...). A viewport.
* Interactions are hooked up through vtkDearImGuiInteractor.