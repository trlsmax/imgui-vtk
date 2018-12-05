#pragma once
#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkCubeSource.h>
#include <vtkGenericOpenGLRenderWindow.h>
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

class MyVTKRenderer
{
public:
    MyVTKRenderer();
    ~MyVTKRenderer() {}
	void UpdateSize(unsigned int w, unsigned int h);
    void Render(void);

private:
	bool m_IsInited;
	GLuint m_fbo;
	GLuint m_tex;
	GLuint m_rbo;
	unsigned int m_Width;
	unsigned int m_Height;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_vtkRenderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkGenericRenderWindowInteractor> m_vtkRenderWindowInteractor;
    vtkSmartPointer<vtkCellPicker> m_picker;
};

