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

#include "imgui.h"
#include "imgui_impl_vtk.h"
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

// VTK includes
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

static vtkSmartPointer<vtkGenericOpenGLRenderWindow> g_RenderWindow = nullptr;
static vtkSmartPointer<vtkGenericRenderWindowInteractor> g_Interactor = nullptr;
static vtkSmartPointer<vtkInteractorStyleTrackballCamera> g_InteractorStyle = nullptr;
static vtkSmartPointer<vtkRenderer> g_Renderer = nullptr;
static int g_VportSize[2] = { 640, 480 };
static std::string g_Title = "VTK + DearImGui";
static GLuint g_FBOHdl = 0;
static GLuint g_RBOHdl = 0;
static GLuint g_TexHdl = 0;
static bool g_Show = true;

// forward declarations.
static void ImGui_ImplVTK_SetVportSize(int w, int h);
static void ImGui_ImplVTK_IsCurrentCallbackFn(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
static void ImGui_ImplVTK_ProcessEvents();

bool    ImGui_ImplVTK_Init()
{
  assert(!g_Renderer);
  assert(!g_InteractorStyle);
  assert(!g_RenderWindow);
  assert(!g_Interactor);

  g_Renderer = vtkSmartPointer<vtkRenderer>::New();
  g_Renderer->ResetCamera();
  g_Renderer->SetBackground(0.39, 0.39, 0.39);

  g_InteractorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  g_InteractorStyle->SetDefaultRenderer(g_Renderer);

  g_Interactor = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
  g_Interactor->SetInteractorStyle(g_InteractorStyle);
  g_Interactor->EnableRenderOff();
  
  g_RenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  g_RenderWindow->SetSize(g_VportSize);
  vtkSmartPointer<vtkCallbackCommand> isCurrentCallback =
    vtkSmartPointer<vtkCallbackCommand>::New();
  isCurrentCallback->SetCallback(ImGui_ImplVTK_IsCurrentCallbackFn);
  g_RenderWindow->AddObserver(vtkCommand::WindowIsCurrentEvent, isCurrentCallback);
  g_RenderWindow->SwapBuffersOn();
#if VTK_MAJOR_VERSION >= 9
  g_RenderWindow->UseOffScreenBuffersOff(); // ensures blit OffScreenBuffers -> Currently bound draw buffer
#if VTK_BUILD_VERSION >= 2
  g_RenderWindow->SetFrameBlitModeToBlitToCurrent(); // VTK library should blit its display fbo to g_FBOHdl.
#endif // VTK_BUILD_VERSION
#else
  g_RenderWindow->SetUseOffScreenBuffers(false);
#endif // VTK_MAJOR_VERSION >= 9


  g_RenderWindow->AddRenderer(g_Renderer);
  g_RenderWindow->SetInteractor(g_Interactor);

  g_FBOHdl = 0;
  g_RBOHdl = 0;
  g_TexHdl = 0;
  g_VportSize[0] = 640;
  g_VportSize[1] = 480;
  g_Show = true;

  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigWindowsMoveFromTitleBarOnly = true; // don't drag window when clicking on image.

  return true;
}

void    ImGui_ImplVTK_Shutdown()
{
  g_Renderer = nullptr;
  g_InteractorStyle = nullptr;
  g_Interactor = nullptr;
  g_RenderWindow = nullptr;

  glDeleteBuffers(1, &g_FBOHdl);
  glDeleteBuffers(1, &g_RBOHdl);
  glDeleteBuffers(1, &g_TexHdl);
}

void    ImGui_ImplVTK_AddActor(vtkSmartPointer<vtkProp> prop)
{
  g_Renderer->AddActor(prop);
  g_Renderer->ResetCamera();
}

IMGUI_IMPL_API void ImGui_ImplVTK_AddActors(vtkSmartPointer<vtkPropCollection> props)
{
  props->InitTraversal();
  vtkProp* prop; 
  vtkCollectionSimpleIterator sit;
  for (props->InitTraversal(sit);
    (prop = props->GetNextProp(sit));)
  {
    g_Renderer->AddActor(prop);
    g_Renderer->ResetCamera();
  }
}

IMGUI_IMPL_API void ImGui_ImplVTK_RemoveActor(vtkSmartPointer<vtkProp> prop)
{
  g_Renderer->RemoveActor(prop);
}

void    ImGui_ImplVTK_SetVportSize(int w, int h)
{
#if VTK_MAJOR_VERSION >= 9
  g_RenderWindow->SetShowWindow(g_Show);
#endif

  if (g_VportSize[0] == w && g_VportSize[1] == h)
    return;
  if (w == 0 || h == 0)
    return;

  g_VportSize[0] = w;
  g_VportSize[1] = h;

  // these cause a crash in glfwSwapBuffers()
  //glDeleteBuffers(1, &g_FBOHdl);
  //glDeleteBuffers(1, &g_RBOHdl);
  //glDeleteBuffers(1, &g_TexHdl);
  
  glGenTextures(1, &g_TexHdl);
  glBindTexture(GL_TEXTURE_2D, g_TexHdl);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_VportSize[0], g_VportSize[1], 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenRenderbuffers(1, &g_RBOHdl);
  glBindRenderbuffer(GL_RENDERBUFFER, g_RBOHdl);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, g_VportSize[0], g_VportSize[1]);
  glBindRenderbuffer(GL_RENDERBUFFER, g_RBOHdl);

  glGenFramebuffers(1, &g_FBOHdl);
  glBindFramebuffer(GL_FRAMEBUFFER, g_FBOHdl);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_TexHdl, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_RBOHdl);
  g_RenderWindow->InitializeFromCurrentContext();
  g_RenderWindow->SetSize(g_VportSize);
  g_Interactor->SetSize(g_VportSize);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void    ImGui_ImplVTK_Render(const std::string& title)
{
  /// are these required?
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  //glBindVertexArray(0);

  g_Title = title;
  if (!ImGui::Begin(g_Title.c_str(), &g_Show, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    ImGui::End();
  else
  {
    ImGui_ImplVTK_SetVportSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
#if VTK_MAJOR_VERSION >= 9
#if VTK_BUILD_VERSION >= 2
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_FBOHdl); // required since we set BlitToCurrent = On.
#endif // VTK_BUILD_VERSION >= 2
#else
    glBindFramebuffer(GL_FRAMEBUFFER, g_FBOHdl);
#endif // VTK_MAJOR_VERSION >= 9
    g_RenderWindow->Render();
#if VTK_MAJOR_VERSION >= 9
#if VTK_BUILD_VERSION >= 2
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif // VTK_BUILD_VERSION >= 2
#else
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif // VTK_MAJOR_VERSION >= 9

    ImGui::BeginChild("##Viewport", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing() - 16.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui_ImplVTK_ProcessEvents();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::Image((void*)g_TexHdl,
      ImGui::GetContentRegionAvail(),
      ImVec2(0, 1), ImVec2(1, 0));
    ImGui::EndChild();
    ImGui::End();
  }
}

void    ImGui_ImplVTK_IsCurrentCallbackFn(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
  bool* isCurrent = static_cast<bool*>(callData);
  *isCurrent = true;
}

void    ImGui_ImplVTK_ProcessEvents()
{
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

  g_Interactor->SetEventInformationFlipY(xpos, ypos, ctrl, shift, dclick);

  if (io.MouseClicked[ImGuiMouseButton_Left])
    g_Interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
  else if (io.MouseReleased[ImGuiMouseButton_Left])
    g_Interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
  else if (io.MouseClicked[ImGuiMouseButton_Right])
    g_Interactor->InvokeEvent(vtkCommand::RightButtonPressEvent, nullptr);
  else if (io.MouseReleased[ImGuiMouseButton_Right])
    g_Interactor->InvokeEvent(vtkCommand::RightButtonReleaseEvent, nullptr);
  else if (io.MouseWheel > 0)
    g_Interactor->InvokeEvent(vtkCommand::MouseWheelForwardEvent, nullptr);
  else if (io.MouseWheel < 0)
    g_Interactor->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, nullptr);

  g_Interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
}


