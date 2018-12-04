#include "vtk.h"
#include <GLFW/glfw3.h>

void MyRW::SetFBO(unsigned int fbo, unsigned int tex, unsigned int w, unsigned int h)
{

	this->SetBackLeftBuffer(GL_COLOR_ATTACHMENT0);
	this->SetFrontLeftBuffer(GL_COLOR_ATTACHMENT0);
	this->SetBackBuffer(GL_COLOR_ATTACHMENT0);
	this->SetFrontBuffer(GL_COLOR_ATTACHMENT0);

	this->Size[0] = w;
	this->Size[1] = h;
	this->NumberOfFrameBuffers = 1;
	this->DepthRenderBufferObject = 0;
	this->FrameBufferObject = static_cast<unsigned int>(fbo);
	this->TextureObjects[0] = static_cast<unsigned int>(tex);
	this->OffScreenRendering = 1;
	this->OffScreenUseFrameBuffer = 1;
	this->Modified();
}

static void MakeCurrentCallback(vtkObject* vtkNotUsed(caller),
	long unsigned int vtkNotUsed(eventId),
	void * clientData,
	void * vtkNotUsed(callData))
{
	glfwMakeContextCurrent((GLFWwindow *)clientData);
}

MyVTKRenderer::MyVTKRenderer(GLFWwindow * win, unsigned int fbo, unsigned int tex, unsigned int w, unsigned int h)
{
    // Renderer
	m_vtkRenderWindow = new MyRW;
	m_renderer = vtkSmartPointer<vtkRenderer>::New();
	m_renderer->SetBackground(1, 1, 1);
	m_vtkRenderWindow->AddRenderer(m_renderer);

	// add the actors to the scene
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	m_renderer->AddActor(actor);

	vtkNew<vtkCallbackCommand> callback;
	callback->SetCallback(MakeCurrentCallback);
	callback->SetClientData(win);
	m_vtkRenderWindow->AddObserver(vtkCommand::WindowMakeCurrentEvent, callback.GetPointer());

	// Interactor
	m_vtkRenderWindowInteractor = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
	m_vtkRenderWindowInteractor->EnableRenderOff();
	m_vtkRenderWindow->SetInteractor(m_vtkRenderWindowInteractor);

	// Initialize the OpenGL context for the renderer
	m_vtkRenderWindow->OpenGLInitContext();

	// Interactor Style
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	style->SetDefaultRenderer(m_renderer);
	style->SetMotionFactor(10.0);
	m_vtkRenderWindowInteractor->SetInteractorStyle(style);

	// Picker
	m_picker = vtkSmartPointer<vtkCellPicker>::New();
	m_picker->SetTolerance(0.0);

	m_vtkRenderWindow->SetFBO(fbo, tex, w, h);
}

void MyVTKRenderer::render(void)
{
	m_vtkRenderWindow->SetSize(800, 600);
	m_vtkRenderWindow->PushState();
	m_vtkRenderWindow->OpenGLInitState();
	m_vtkRenderWindow->MakeCurrent();
	m_vtkRenderWindow->Start();
	m_vtkRenderWindow->Render();
	m_vtkRenderWindow->PopState();
}
