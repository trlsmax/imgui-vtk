#include "vtk.h"

MyVTKRenderer::MyVTKRenderer()
	:m_IsInited(false)
	,m_Width(0)
	,m_Height(0)
	,m_fbo(0)
	,m_tex(0)
	,m_rbo(0)
{
	// add the actors to the scene
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());
	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

    // Renderer
	m_vtkRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	m_renderer = vtkSmartPointer<vtkRenderer>::New();
	m_renderer->SetBackground(1, 1, 1);
	m_vtkRenderWindow->AddRenderer(m_renderer);

	m_renderer->AddActor(actor);
	m_renderer->GetActiveCamera()->SetPosition(0, 0, 10);
	m_renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
	m_renderer->GetActiveCamera()->SetViewUp(0, 1, 0);

	// Interactor
	m_vtkRenderWindowInteractor = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
	m_vtkRenderWindowInteractor->EnableRenderOff();
	m_vtkRenderWindow->SetInteractor(m_vtkRenderWindowInteractor);

	// Interactor Style
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	style->SetDefaultRenderer(m_renderer);
	style->SetMotionFactor(10.0);
	m_vtkRenderWindowInteractor->SetInteractorStyle(style);

	// Picker
	m_picker = vtkSmartPointer<vtkCellPicker>::New();
	m_picker->SetTolerance(0.0);
}

MyVTKRenderer::~MyVTKRenderer()
{
	glDeleteBuffers(1, &m_fbo);
	glDeleteBuffers(1, &m_rbo);
	glDeleteTextures(1, &m_tex);
}

void MyVTKRenderer::UpdateSize(unsigned int w, unsigned int h)
{
	if (w == m_Width && h == m_Height)
		return;

	if (w == 0 || h == 0)
		return;

	m_Width = w;
	m_Height = h;

	// resize the render window
	m_renderer->RemoveActor(actor);
	m_vtkRenderWindow->SetSize(m_Width, m_Height);
	m_vtkRenderWindow->FullScreenOn();
	m_vtkRenderWindow->OffScreenRenderingOn();
	//m_vtkRenderWindow->SetMultiSamples(0);
	m_vtkRenderWindow->Modified();
	m_vtkRenderWindowInteractor->UpdateSize(m_Width, m_Height);
	m_vtkRenderWindowInteractor->Modified();
	m_renderer->AddActor(actor);
	m_IsInited = false;

	// delete old fbo
	glDeleteBuffers(1, &m_fbo);
	glDeleteBuffers(1, &m_rbo);
	glDeleteTextures(1, &m_tex);

	// create a texture object
	glGenTextures(1, &m_tex);
	glBindTexture(GL_TEXTURE_2D, m_tex);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	// create a renderbuffer object to store depth info
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// create a framebuffer object
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void MyVTKRenderer::Render(void)
{

	if (m_fbo > 0) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		if (!m_IsInited) {
			m_vtkRenderWindow->InitializeFromCurrentContext();
			m_IsInited = true;
		}

		m_vtkRenderWindow->PushState();
		m_vtkRenderWindow->OpenGLInitState();
		glUseProgram(0);
		m_vtkRenderWindow->Render();
		m_vtkRenderWindow->PopState();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// these has to be called, otherwise imgui 
	// wouldn't show up
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// use the texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_tex);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void MyVTKRenderer::MouseButtonCallback(double xpos, double ypos, int button, int action, bool ctrl, bool shift, bool dclick)
{
	m_vtkRenderWindowInteractor->SetEventInformationFlipY(xpos, ypos, ctrl, shift, dclick);

	if (button == MOUSE_BUTTON_LEFT && action == BUTTON_PRESS)
		m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
	if (button == MOUSE_BUTTON_LEFT && action == BUTTON_RELEASE)
		m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
	if (button == MOUSE_BUTTON_RIGHT && action == BUTTON_PRESS)
		m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::RightButtonPressEvent, nullptr);
	if (button == MOUSE_BUTTON_RIGHT && action == BUTTON_RELEASE)
		m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::RightButtonReleaseEvent, nullptr);
}

void MyVTKRenderer::MousePositionCallback(double xpos, double ypos, bool ctrl, bool shift)
{
	m_vtkRenderWindowInteractor->SetEventInformationFlipY(xpos, ypos, ctrl, shift, 0);
	m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
}

void MyVTKRenderer::MouseWheelCallback(double xoffset, double yoffset)
{
	if (yoffset > 0)
		m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::MouseWheelForwardEvent, nullptr);
	else if (yoffset < 0)
		m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, nullptr);
}