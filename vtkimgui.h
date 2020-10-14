#include <array>

#include <vtkActor.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtk_glew.h>

class VtkDearImGui
{
public:
  VtkDearImGui();
  ~VtkDearImGui();

  void AddActor(vtkSmartPointer<vtkActor> actor);
  void SetSize(int w, int h);
  void Render();
  GLuint GetTexId();
  GLuint GetFboId();
  //Screenshot();

protected:
  GLuint FBOHdl;
  GLuint RBOHdl;
  GLuint TexHdl;
  bool ShowWin;
  vtkSmartPointer<vtkExternalOpenGLRenderWindow> vtkExtRenWin;
  vtkSmartPointer<vtkRenderer> vtkRen;

private:
  std::string title;
  std::array<int, 2> VportSize;
};