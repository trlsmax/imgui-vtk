#include "VtkViewer.h"

// dear imgui: Renderer for VTK(OpenGL back end)
// - Desktop GL: 2.x 3.x 4.x
// - Embedded GL: ES 2.0 (WebGL 1.0), ES 3.0 (WebGL 2.0)
// This needs to be used along with a Platform Binding (e.g. GLFW, SDL, Win32, custom..) and a renderer binding (OpenGL)

// Implemented features:

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
// 

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

// GL includes
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#elif defined(IMGUI_IMPL_OPENGL_ES3)
#if (defined(__APPLE__) && (TARGET_OS_IOS || TARGET_OS_TV))
#include <OpenGLES/ES3/gl.h>    // Use GL ES 3
#else
#include <GLES3/gl3.h>          // Use GL ES 3
#endif
#else

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Needs to be initialized with gl3wInit() in user's code
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Needs to be initialized with glewInit() in user's code.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Needs to be initialized with gladLoadGL() in user's code.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Needs to be initialized with gladLoadGL(...) or gladLoaderLoadGL() in user's code.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#endif
#include <glbinding/Binding.h>  // Needs to be initialized with glbinding::Binding::initialize() in user's code.
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#endif
#include <glbinding/glbinding.h>// Needs to be initialized with glbinding::initialize() in user's code.
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif
#endif

vtkSmartPointer<vtkGenericRenderWindowInteractor> VtkViewer::VtkViewerCallback::interactor = nullptr;

void VtkViewer::VtkViewerCallback::isCurrentCallbackFn(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData) {
	bool* isCurrent = static_cast<bool*>(callData);
	*isCurrent = true;
}

void VtkViewer::VtkViewerCallback::processEvents() {
	if (!ImGui::IsWindowFocused())
		return;

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	ImVec2 wPos = ImGui::GetWindowPos();

	double xpos = static_cast<double>(io.MousePos[0]) - static_cast<double>(ImGui::GetWindowPos().x);
	double ypos = static_cast<double>(io.MousePos[1]) - static_cast<double>(ImGui::GetWindowPos().y);
	int ctrl = static_cast<int>(io.KeyCtrl);
	int shift = static_cast<int>(io.KeyShift);
	bool dclick = io.MouseDoubleClicked[0] || io.MouseDoubleClicked[1] || io.MouseDoubleClicked[2];

	interactor->SetEventInformationFlipY(xpos, ypos, ctrl, shift, dclick);

	if (io.MouseClicked[ImGuiMouseButton_Left])
		interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
	else if (io.MouseReleased[ImGuiMouseButton_Left])
		interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
	else if (io.MouseClicked[ImGuiMouseButton_Right])
		interactor->InvokeEvent(vtkCommand::RightButtonPressEvent, nullptr);
	else if (io.MouseReleased[ImGuiMouseButton_Right])
		interactor->InvokeEvent(vtkCommand::RightButtonReleaseEvent, nullptr);
	else if (io.MouseWheel > 0)
		interactor->InvokeEvent(vtkCommand::MouseWheelForwardEvent, nullptr);
	else if (io.MouseWheel < 0)
		interactor->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, nullptr);

	interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
}

VtkViewer::VtkViewer(const std::string& _title, unsigned int _viewportWidth, unsigned int _viewportHeight) 
	: title(_title), viewportWidth(_viewportWidth), viewportHeight(_viewportHeight), renderWindow(nullptr),
	interactor(nullptr), interactorStyle(nullptr), renderer(nullptr), fbo(0), rbo(0), tex(0), show(true) {
	init();
}

VtkViewer::VtkViewer() : VtkViewer(DEFAULT_TITLE, DEFAULT_VIEWPORT_WIDTH, DEFAULT_VIEWPORT_HEIGHT) { }

VtkViewer::VtkViewer(const VtkViewer& vtkViewer) 
	: title(vtkViewer.title), viewportWidth(vtkViewer.viewportWidth), viewportHeight(vtkViewer.viewportHeight),
	renderWindow(vtkViewer.renderWindow), interactor(vtkViewer.interactor), interactorStyle(vtkViewer.interactorStyle),
	renderer(vtkViewer.renderer), fbo(vtkViewer.fbo), rbo(vtkViewer.rbo), tex(vtkViewer.tex), show(vtkViewer.show) {
}

VtkViewer::VtkViewer(VtkViewer&& vtkViewer) noexcept 
	: title(vtkViewer.title), viewportWidth(vtkViewer.viewportWidth), viewportHeight(vtkViewer.viewportHeight),
	renderWindow(std::move(vtkViewer.renderWindow)), interactor(std::move(vtkViewer.interactor)), 
	interactorStyle(std::move(vtkViewer.interactorStyle)), renderer(std::move(vtkViewer.renderer)), fbo(vtkViewer.fbo), 
	rbo(vtkViewer.rbo), tex(vtkViewer.tex), show(vtkViewer.show) {
}

VtkViewer::~VtkViewer() {
	renderer = nullptr;
	interactorStyle = nullptr;
	interactor = nullptr;
	renderWindow = nullptr;

	glDeleteBuffers(1, &fbo);
	glDeleteBuffers(1, &rbo);
	glDeleteBuffers(1, &tex);
}

VtkViewer& VtkViewer::operator=(const VtkViewer& vtkViewer) {
	title = vtkViewer.title;
	viewportWidth = vtkViewer.viewportWidth;
	viewportHeight = vtkViewer.viewportHeight;
	renderWindow = vtkViewer.renderWindow;
	interactor = vtkViewer.interactor;
	interactorStyle = vtkViewer.interactorStyle;
	renderer = vtkViewer.renderer;
	fbo = vtkViewer.fbo;
	rbo = vtkViewer.rbo;
	tex = vtkViewer.tex;
	show = vtkViewer.show;
	return *this;
}

void VtkViewer::init() {

	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->ResetCamera();
	renderer->SetBackground(DEFAULT_BACKGROUND);

	interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactorStyle->SetDefaultRenderer(renderer);

	interactor = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
	interactor->SetInteractorStyle(interactorStyle);
	interactor->EnableRenderOff();
	VtkViewerCallback::interactor = interactor;

	int viewportSize[2] = {viewportWidth, viewportHeight};

	renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	renderWindow->SetSize(viewportSize);
	vtkSmartPointer<vtkCallbackCommand> isCurrentCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	isCurrentCallback->SetCallback(&VtkViewerCallback::isCurrentCallbackFn);
	renderWindow->AddObserver(vtkCommand::WindowIsCurrentEvent, isCurrentCallback);
	renderWindow->SwapBuffersOn();
#if VTK_MAJOR_VERSION >= 9
	renderWindow->UseOffScreenBuffersOff(); // ensures blit OffScreenBuffers -> Currently bound draw buffer
#if VTK_BUILD_VERSION >= 2
	renderWindow->SetFrameBlitModeToBlitToCurrent(); // VTK library should blit its display fbo to g_FBOHdl.
#endif // VTK_BUILD_VERSION
#else
	renderWindow->SetUseOffScreenBuffers(false);
#endif // VTK_MAJOR_VERSION >= 9

	renderWindow->AddRenderer(renderer);
	renderWindow->SetInteractor(interactor);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsMoveFromTitleBarOnly = true; // don't drag window when clicking on image.

	if (!renderer || !interactorStyle || !renderWindow || !interactor)
		throw VtkViewerError("Couldn't initialize VtkViewer");
}

void VtkViewer::render() {
	if (!ImGui::Begin(title.c_str(), &show, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		ImGui::End();
	else {
		setViewportSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

#if VTK_MAJOR_VERSION >= 9
#if VTK_BUILD_VERSION >= 2
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); // required since we set BlitToCurrent = On.
#endif // VTK_BUILD_VERSION >= 2
#else
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#endif // VTK_MAJOR_VERSION >= 9
		renderWindow->Render();
#if VTK_MAJOR_VERSION >= 9
#if VTK_BUILD_VERSION >= 2
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif // VTK_BUILD_VERSION >= 2
#else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif // VTK_MAJOR_VERSION >= 9

		ImGui::BeginChild("##Viewport", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing() - 16.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		VtkViewerCallback::processEvents();
		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::Image((void*)tex, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndChild();
		ImGui::End();
	}
}

void VtkViewer::addActor(const vtkSmartPointer<vtkProp>& actor) {
	renderer->AddActor(actor);
	renderer->ResetCamera();
}

void VtkViewer::addActors(const vtkSmartPointer<vtkPropCollection>& actors) {
	actors->InitTraversal();
	vtkProp* actor;
	vtkCollectionSimpleIterator sit;
	for (actors->InitTraversal(sit); (actor = actors->GetNextProp(sit));) {
		renderer->AddActor(actor);
		renderer->ResetCamera();
	}
}

void VtkViewer::removeActor(const vtkSmartPointer<vtkProp>& actor) {
	renderer->RemoveActor(actor);
}

void VtkViewer::setViewportSize(unsigned int viewportWidth, unsigned int viewportHeight) {
#if VTK_MAJOR_VERSION >= 9
	renderWindow->SetShowWindow(show);
#endif

	if ((this->viewportWidth == viewportWidth && this->viewportHeight == viewportHeight) || viewportWidth == 0 || viewportHeight == 0)
		return;

	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
	int viewportSize[2] = {viewportWidth, viewportHeight};

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportSize[0], viewportSize[1], 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, viewportSize[0], viewportSize[1]);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	renderWindow->InitializeFromCurrentContext();
	renderWindow->SetSize(viewportSize);
	interactor->SetSize(viewportSize);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}