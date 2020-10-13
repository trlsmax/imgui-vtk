/*
* @file vtkDearImGuiVport.hxx
* 
* @brief vtkDearImGuiVport
*   1. An effort to incorporate VTK's dataset visualization capabilities with versatility of DearImGUI.
*   2. It is expected that usage of this filter will be a mix of the usual VTK's style(pipeline setup) and DearImGUI's immediate-style.
*   3. This is meant to be as non-intrusive as possible to existing projects using DearImGUI, 
*       yet capable of building a full-fledged app with vtk + ImGUi + vtkDearImGuiVport. 
*   4. vtkDearImGuiVport does not use a concrete platform windowing system such as Qt/Glfw/Sdl/Xwindow/Cocoa/Win32/wx/Gtk.
*   5. Instead, it renders into a DearImGui window with a call to ImGui::Image((void*)id, ..., ...). A viewport.
*   6. Interactions are hooked up through vtkDearImGuiInteractor.
* 
*
* Usage:
* 1.[Old] Set up a pipeline up to the stage where you've got your actors ready.
* 2.[New] Instead of vtkRenderWindow use vtkDearImGuiVport. Info: This uses an existing OpenGL context and an existing Platform windowing system.
* 3.[New] Instead of vtkRenderWindowInteractor use vtkDearImGuiInteractor. Info: This connects I/O events with DearImGUI.
* 4.[New] Within your DearImGUI event loop. Call the bound method vtkDearImGuiVport::Render().
* 
* 
* Implementation:
* 1. We shall use an externally created OpenGL context that features a programmable pipeline. >= 2.0.
* 2. The context could have been setup from a 
* 2. From VTK >= 9.0.0, vtk renders to a render frame buffer internally and copys it to a vtkTextureObject.
* 3. Since no concrete window is created, always assume context is current. Do this when event IsCurrent is triggered.
* 4. Proceed to grab the internal texture object's handle and pipe it to DearImGUI as an image.
* 
*/