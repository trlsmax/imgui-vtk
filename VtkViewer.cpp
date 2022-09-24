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

// OpenGL Loader
// This can be replaced with another loader, e.g. glad, but
// remember to change the corresponding initialize call below!
#include <GL/gl3w.h>            // GL3w, initialized with gl3wInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

void VtkViewer::isCurrentCallbackFn(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData){
	bool* isCurrent = static_cast<bool*>(callData);
	*isCurrent = true;
}

void VtkViewer::processEvents(){
	if (!ImGui::IsWindowFocused() && !ImGui::IsWindowHovered()){
		return;
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsMoveFromTitleBarOnly = true; // don't drag window when clicking on image.
	ImVec2 viewportPos = ImGui::GetCursorStartPos();

	double xpos = static_cast<double>(io.MousePos[0]) - static_cast<double>(viewportPos.x);
	double ypos = static_cast<double>(io.MousePos[1]) - static_cast<double>(viewportPos.y);
	int ctrl = static_cast<int>(io.KeyCtrl);
	int shift = static_cast<int>(io.KeyShift);
	bool dclick = io.MouseDoubleClicked[0] || io.MouseDoubleClicked[1] || io.MouseDoubleClicked[2];

	interactor->SetEventInformationFlipY(xpos, ypos, ctrl, shift, dclick);

	if (ImGui::IsWindowHovered()){
		if (io.MouseClicked[ImGuiMouseButton_Left]){
			interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
		}
		else if (io.MouseClicked[ImGuiMouseButton_Right]){
			interactor->InvokeEvent(vtkCommand::RightButtonPressEvent, nullptr);
			ImGui::SetWindowFocus(); // make right-clicks bring window into focus
		}
		else if (io.MouseWheel > 0){
			interactor->InvokeEvent(vtkCommand::MouseWheelForwardEvent, nullptr);
		}
		else if (io.MouseWheel < 0){
			interactor->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, nullptr);
		}
	}

	if (io.MouseReleased[ImGuiMouseButton_Left]){
		interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
	}
	else if (io.MouseReleased[ImGuiMouseButton_Right]){
		interactor->InvokeEvent(vtkCommand::RightButtonReleaseEvent, nullptr);
	}

	interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
}

VtkViewer::VtkViewer() 
	: viewportWidth(0), viewportHeight(0), renderWindow(nullptr), interactor(nullptr), interactorStyle(nullptr),
	renderer(nullptr), tex(0), firstRender(true){
	init();
}

VtkViewer::VtkViewer(const VtkViewer& vtkViewer) 
	: viewportWidth(0), viewportHeight(0), renderWindow(vtkViewer.renderWindow), interactor(vtkViewer.interactor),
	interactorStyle(vtkViewer.interactorStyle), renderer(vtkViewer.renderer), tex(vtkViewer.tex),
	firstRender(vtkViewer.firstRender){
}

VtkViewer::VtkViewer(VtkViewer&& vtkViewer) noexcept 
	: viewportWidth(0), viewportHeight(0), renderWindow(std::move(vtkViewer.renderWindow)),
	interactor(std::move(vtkViewer.interactor)), interactorStyle(std::move(vtkViewer.interactorStyle)),
	renderer(std::move(vtkViewer.renderer)), tex(vtkViewer.tex), firstRender(vtkViewer.firstRender){
}

VtkViewer::~VtkViewer(){
	renderer = nullptr;
	interactorStyle = nullptr;
	interactor = nullptr;
	renderWindow = nullptr;

	glDeleteTextures(1, &tex);
}

VtkViewer& VtkViewer::operator=(const VtkViewer& vtkViewer){
	viewportWidth = vtkViewer.viewportWidth;
	viewportHeight = vtkViewer.viewportHeight;
	renderWindow = vtkViewer.renderWindow;
	interactor = vtkViewer.interactor;
	interactorStyle = vtkViewer.interactorStyle;
	renderer = vtkViewer.renderer;
	tex = vtkViewer.tex;
	firstRender = vtkViewer.firstRender;
	return *this;
}

void VtkViewer::init(){

	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->ResetCamera();
	renderer->SetBackground(DEFAULT_BACKGROUND);
	renderer->SetBackgroundAlpha(DEFAULT_ALPHA);

	interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactorStyle->SetDefaultRenderer(renderer);

	interactor = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
	interactor->SetInteractorStyle(interactorStyle);
	interactor->EnableRenderOff();

	int viewportSize[2] = {static_cast<int>(viewportWidth), static_cast<int>(viewportHeight)};

	renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	renderWindow->SetSize(viewportSize);

	vtkSmartPointer<vtkCallbackCommand> isCurrentCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	isCurrentCallback->SetCallback(&isCurrentCallbackFn);
	renderWindow->AddObserver(vtkCommand::WindowIsCurrentEvent, isCurrentCallback);

	renderWindow->SwapBuffersOn();

	renderWindow->SetOffScreenRendering(true);
	renderWindow->SetFrameBlitModeToNoBlit();

	renderWindow->AddRenderer(renderer);
	renderWindow->SetInteractor(interactor);

	if (!renderer || !interactorStyle || !renderWindow || !interactor){
		throw VtkViewerError("Couldn't initialize VtkViewer");
	}
}

void VtkViewer::render(){
	render(ImGui::GetContentRegionAvail());
}
void VtkViewer::render(const ImVec2 size){
	setViewportSize(size);

	renderWindow->Render();
	renderWindow->WaitForCompletion();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
	ImGui::BeginChild("##Viewport", size, true, VtkViewer::NoScrollFlags());
	ImGui::Image(reinterpret_cast<void*>(tex), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	processEvents();
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void VtkViewer::addActor(const vtkSmartPointer<vtkProp>& actor){
	renderer->AddActor(actor);
	renderer->ResetCamera();
}

void VtkViewer::addActors(const vtkSmartPointer<vtkPropCollection>& actors){
	actors->InitTraversal();
	vtkProp* actor;
	vtkCollectionSimpleIterator sit;
	for (actors->InitTraversal(sit); (actor = actors->GetNextProp(sit));){
		renderer->AddActor(actor);
		renderer->ResetCamera();
	}
}

void VtkViewer::removeActor(const vtkSmartPointer<vtkProp>& actor){
	renderer->RemoveActor(actor);
}

void VtkViewer::setViewportSize(const ImVec2 newSize){
	if (((viewportWidth == newSize.x && viewportHeight == newSize.y) || viewportWidth <= 0 || viewportHeight <= 0) && !firstRender){
		return;
	}

	viewportWidth = static_cast<unsigned int>(newSize.x);
	viewportHeight = static_cast<unsigned int>(newSize.y);

	int viewportSize[] = {static_cast<int>(newSize.x), static_cast<int>(newSize.y)};

	// Free old buffers
	glDeleteTextures(1, &tex);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewportWidth, viewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	renderWindow->InitializeFromCurrentContext();
	renderWindow->SetSize(viewportSize);
	interactor->SetSize(viewportSize);

	auto vtkfbo = renderWindow->GetDisplayFramebuffer();
	vtkfbo->Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	vtkfbo->UnBind();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	firstRender = false;
}
