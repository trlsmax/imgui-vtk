#include "vtkimgui.h"
#include "imgui.h"

VtkDearImGui::VtkDearImGui() {
  this->vtkExtRenWin = vtkSmartPointer<vtkExternalOpenGLRenderWindow>::New();
  this->vtkExtRenWin->SetSize(VportSize.data());
  this->vtkExtRenWin->AutomaticWindowPositionAndResizeOff();
  this->vtkExtRenWin->UseExternalContentOff();
  this->vtkRen = vtkSmartPointer<vtkRenderer>::New();
  this->vtkRen->ResetCamera();
  this->vtkExtRenWin->AddRenderer(this->vtkRen);
  this->vtkRen->SetBackground(1, 1, 1);

  this->FBOHdl = 0;
  this->RBOHdl = 0;
  this->TexHdl = 0;
  this->VportSize[0] = 0;
  this->VportSize[1] = 0;
  this->ShowWin = true;
}

VtkDearImGui::~VtkDearImGui() {
  this->vtkExtRenWin = nullptr;
  this->vtkRen = nullptr;
  glDeleteBuffers(1, &this->FBOHdl);
  glDeleteBuffers(1, &this->RBOHdl);
  glDeleteBuffers(1, &this->TexHdl);
}

void VtkDearImGui::AddActor(vtkSmartPointer<vtkActor> actor)
{
  this->vtkRen->AddActor(actor);
  this->vtkRen->ResetCamera();
}

void VtkDearImGui::SetSize(int w, int h)
{
  this->vtkExtRenWin->SetShowWindow(this->ShowWin);

  if (this->VportSize[0] == w && this->VportSize[1] == h)
    return;
  if (w == 0 || h == 0)
    return;

  this->VportSize[0] = w;
  this->VportSize[1] = h;

  //glDeleteBuffers(1, &this->FBOHdl);
  //glDeleteBuffers(1, &this->RBOHdl);
  //glDeleteBuffers(1, &this->TexHdl);
  
  glGenTextures(1, &this->TexHdl);
  glBindTexture(GL_TEXTURE_2D, this->TexHdl);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->VportSize[0], this->VportSize[1], 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenRenderbuffers(1, &this->RBOHdl);
  glBindRenderbuffer(GL_RENDERBUFFER, this->RBOHdl);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->VportSize[0], this->VportSize[1]);
  glBindRenderbuffer(GL_RENDERBUFFER, this->RBOHdl);

  glGenFramebuffers(1, &this->FBOHdl);
  glBindFramebuffer(GL_FRAMEBUFFER, this->FBOHdl);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->TexHdl, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->RBOHdl);
  this->vtkExtRenWin->InitializeFromCurrentContext();
  this->vtkExtRenWin->SetSize(this->VportSize.data());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VtkDearImGui::Render()
{
  /// are these required?
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  //glBindVertexArray(0);

  if (!ImGui::Begin("VTK-DearImGui Viewport", &this->ShowWin, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    ImGui::End();
  else
  {
    this->SetSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
    this->vtkExtRenWin->Render();

    ImGui::BeginChild("##Viewport", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing() - 16.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::Image((void*)this->TexHdl,
      ImGui::GetContentRegionAvail(),
      ImVec2(0, 1), ImVec2(1, 0));
    ImGui::EndChild();
    ImGui::End();
  }
}

GLuint VtkDearImGui::GetTexId()
{
  return this->TexHdl;
}

GLuint VtkDearImGui::GetFboId()
{
  return this->FBOHdl;
}

