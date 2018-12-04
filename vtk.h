#pragma once
#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkCubeSource.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkObject.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCamera.h>
#include <vtk_glew.h>
#include <vtkCallbackCommand.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <GLFW/glfw3.h>

class MyRW : public vtkExternalOpenGLRenderWindow
{
public:
	void SetFBO(unsigned int fbo, unsigned int tex, unsigned int w, unsigned int h);
};

class MyVTKRenderer
{
public:
    MyVTKRenderer(GLFWwindow * win, unsigned int fbo, unsigned int tex, unsigned int w, unsigned int h);
    ~MyVTKRenderer() {}
    void render(void);

private:
    MyRW* m_vtkRenderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkGenericRenderWindowInteractor> m_vtkRenderWindowInteractor;
    vtkSmartPointer<vtkCellPicker> m_picker;
};

