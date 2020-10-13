/*
* @file vtkDearImGuiVport.hxx
* 
* @brief vtkDearImGuiVport
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