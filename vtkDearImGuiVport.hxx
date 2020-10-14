/*
* @file vtkDearImGuiVport.hxx
* 
* @brief vtkDearImGuiVport
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

#ifndef vtkDearImGuiViewPort_h
#define vtkDearImGuiViewPort_h

#include <vtkGenericOpenGLRenderWindow.h>
#include "vtkRenderingExternalModule.h" // For export macro

class VTKRENDERINGEXTERNAL_EXPORT vtkDearImGuiViewPort
  : public vtkGenericOpenGLRenderWindow
{
public:
  static vtkDearImGuiViewPort* New();
  vtkTypeMacro(vtkDearImGuiViewPort, vtkGenericOpenGLRenderWindow);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkGetMacro(Title, std::string);
  vtkSetMacro(Title, std::string);
  
  vtkGetVector2Macro(Size, int);
  vtkSetVector2Macro(Size, int);

  /**
   * @brief Begin the rendering process using existing content, existing windowing system.
   *        Also a call to ImGui::Begin.
  */
  void Start(void) override;

  /**
   * @brief Tells if this window is the current graphics context for the calling thread.
  */
  bool IsCurrent(void) override;

  /**
   * @brief Render texture to ImGui ViewPort.
  */
  void Frame(void) override;

protected:
  vtkDearImGuiViewPort();
  ~vtkDearImGuiViewPort();

  std::string Title;
  int Size[2];

private:
  vtkDearImGuiViewPort(const vtkDearImGuiViewPort&) = delete;
  void operator=(const vtkDearImGuiViewPort&) = delete;
};

#endif // vtkDearImGuiViewPort_h