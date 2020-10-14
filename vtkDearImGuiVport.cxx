#include "vtkDearImGuiVport.hxx"
#include "vtkOpenGLState.h"
#include "vtkTextureObject.h"
#include <vtkObjectFactory.h>

#include "imgui.h"

#include "vtkLogger.h"

vtkStandardNewMacro(vtkDearImGuiViewPort);

vtkDearImGuiViewPort::vtkDearImGuiViewPort()
{
  this->Title = "VTK-DearImGui";
  this->UseOffScreenBuffers = false; // don't mess with this or the deprecated SetOffScreenRendering.
  this->Size[0] = 0; this->Size[1] = 0;
}

vtkDearImGuiViewPort::~vtkDearImGuiViewPort()
{

}

void vtkDearImGuiViewPort::Start(void)
{
  this->OpenGLInit();

  // Use hardware acceleration
  this->SetIsDirect(true);

  this->SetSize(this->Size);

  this->Superclass::Start();
}

bool vtkDearImGuiViewPort::IsCurrent(void)
{
  return true;
}

void vtkDearImGuiViewPort::Frame(void)
{
  if (!ImGui::Begin("VTK-DearImGui Viewport", &this->ShowWindow, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    ImGui::End();
  else
  {
    ImGui::BeginChild("##Viewport", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing() - 16.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::Image((void*)this->DrawPixelsTextureObject->GetHandle(),
      ImGui::GetContentRegionAvail(),
      ImVec2(0, 1), ImVec2(1, 0));
    ImGui::EndChild();
    ImGui::End();
  }
}

void vtkDearImGuiViewPort::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Title: " << this->Title << std::endl;
  os << indent << "Size : " << this->Size[0] << ", " << this->Size[1] << std::endl;
  os << indent << "ShowWindow: " << this->ShowWindow << std::endl;
  this->Superclass::PrintSelf(os, indent);
}
