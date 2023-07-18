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
#include <stddef.h> // intptr_t
#else
#include <stdint.h> // intptr_t
#endif

// OpenGL Loader
// This can be replaced with another loader, e.g. glad, but
// remember to change the corresponding initialize call below!
#include <GL/gl3w.h> // GL3w, initialized with gl3wInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

void VtkViewer::isCurrentCallbackFn(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData) {
    bool* isCurrent = static_cast<bool*>(callData);
    *isCurrent = true;
}

void VtkViewer::processEvents() {
    auto is_focused = ImGui::IsWindowFocused();
    auto is_hovered = ImGui::IsWindowHovered();
    if (!is_focused && !is_hovered) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true; // don't drag window when clicking on image.
    ImVec2 screenPos = ImGui::GetWindowPos();
    ImVec2 viewportSize = ImGui::GetWindowSize();

    auto dpiScale = ImGui::GetWindowDpiScale();
    double xpos = (io.MousePos[0] - screenPos.x) * dpiScale;
    double ypos = (screenPos.y + viewportSize.y - io.MousePos[1]) * dpiScale;
    int ctrl = static_cast<int>(io.KeyCtrl);
    int shift = static_cast<int>(io.KeyShift);
    bool dclick = io.MouseDoubleClicked[0] || io.MouseDoubleClicked[1] || io.MouseDoubleClicked[2];

    interactor->SetEventInformation(xpos, ypos, ctrl, shift, dclick);

    if (is_hovered) {
        if (io.MouseClicked[ImGuiMouseButton_Left]) {
            interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
        } else if (io.MouseClicked[ImGuiMouseButton_Right]) {
            interactor->InvokeEvent(vtkCommand::RightButtonPressEvent, nullptr);
            ImGui::SetWindowFocus(); // make right-clicks bring window into focus
        } else if (io.MouseWheel > 0) {
            interactor->InvokeEvent(vtkCommand::MouseWheelForwardEvent, nullptr);
        } else if (io.MouseWheel < 0) {
            interactor->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, nullptr);
        }
        interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
    }

    if (io.MouseReleased[ImGuiMouseButton_Left]) {
        interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
        interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
    } else if (io.MouseReleased[ImGuiMouseButton_Right]) {
        interactor->InvokeEvent(vtkCommand::RightButtonReleaseEvent, nullptr);
        interactor->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
    }

    auto ImGuiKeyToKeySym = [](const ImGuiKey& key, bool shift) -> const char* {
        const char* keysym = nullptr;
        switch (key) {
        case ImGuiKey_LeftArrow:
            keysym = "Left";
            break;
        case ImGuiKey_RightArrow:
            keysym = "Right";
            break;
        case ImGuiKey_UpArrow:
            keysym = "Up";
            break;
        case ImGuiKey_DownArrow:
            keysym = "Down";
            break;
        case ImGuiKey_PageUp:
            break;
        case ImGuiKey_PageDown:
            break;
        case ImGuiKey_Home:
            break;
        case ImGuiKey_End:
            break;
        case ImGuiKey_Insert:
            break;
        case ImGuiKey_Delete:
            break;
        case ImGuiKey_Backspace:
            break;
        case ImGuiKey_Space:
            break;
        case ImGuiKey_Enter:
            break;
        case ImGuiKey_Escape:
            break;
        case ImGuiKey_LeftCtrl:
            break;
        case ImGuiKey_LeftShift:
            break;
        case ImGuiKey_LeftAlt:
            break;
        case ImGuiKey_LeftSuper:
            break;
        case ImGuiKey_RightCtrl:
            break;
        case ImGuiKey_RightShift:
            break;
        case ImGuiKey_RightAlt:
            break;
        case ImGuiKey_RightSuper:
            break;
        case ImGuiKey_Menu:
            break;
        case ImGuiKey_0:
            keysym = "0";
            break;
        case ImGuiKey_1:
            keysym = "1";
            break;
        case ImGuiKey_2:
            keysym = "2";
            break;
        case ImGuiKey_3:
            keysym = "3";
            break;
        case ImGuiKey_4:
            keysym = "4";
            break;
        case ImGuiKey_5:
            keysym = "5";
            break;
        case ImGuiKey_6:
            keysym = "6";
            break;
        case ImGuiKey_7:
            keysym = "7";
            break;
        case ImGuiKey_8:
            keysym = "8";
            break;
        case ImGuiKey_9:
            keysym = "9";
            break;
        case ImGuiKey_A:
            keysym = shift ? "A" : "a";
            break;
        case ImGuiKey_B:
            keysym = shift ? "B" : "b";
            break;
        case ImGuiKey_C:
            keysym = shift ? "C" : "c";
            break;
        case ImGuiKey_D:
            keysym = shift ? "D" : "d";
            break;
        case ImGuiKey_E:
            keysym = shift ? "E" : "e";
            break;
        case ImGuiKey_F:
            keysym = shift ? "F" : "f";
            break;
        case ImGuiKey_G:
            keysym = shift ? "G" : "g";
            break;
        case ImGuiKey_H:
            keysym = shift ? "H" : "h";
            break;
        case ImGuiKey_I:
            keysym = shift ? "I" : "i";
            break;
        case ImGuiKey_J:
            keysym = shift ? "J" : "j";
            break;
        case ImGuiKey_K:
            keysym = shift ? "K" : "k";
            break;
        case ImGuiKey_L:
            keysym = shift ? "L" : "l";
            break;
        case ImGuiKey_M:
            keysym = shift ? "M" : "m";
            break;
        case ImGuiKey_N:
            keysym = shift ? "N" : "n";
            break;
        case ImGuiKey_O:
            keysym = shift ? "O" : "o";
            break;
        case ImGuiKey_P:
            keysym = shift ? "P" : "p";
            break;
        case ImGuiKey_Q:
            keysym = shift ? "Q" : "q";
            break;
        case ImGuiKey_R:
            keysym = shift ? "R" : "r";
            break;
        case ImGuiKey_S:
            keysym = shift ? "S" : "s";
            break;
        case ImGuiKey_T:
            keysym = shift ? "T" : "t";
            break;
        case ImGuiKey_U:
            keysym = shift ? "U" : "u";
            break;
        case ImGuiKey_V:
            keysym = shift ? "V" : "v";
            break;
        case ImGuiKey_W:
            keysym = shift ? "W" : "w";
            break;
        case ImGuiKey_X:
            keysym = shift ? "X" : "x";
            break;
        case ImGuiKey_Y:
            keysym = shift ? "Y" : "y";
            break;
        case ImGuiKey_Z:
            keysym = shift ? "Z" : "z";
            break;
        case ImGuiKey_F1:
            break;
        case ImGuiKey_F2:
            break;
        case ImGuiKey_F3:
            break;
        case ImGuiKey_F4:
            break;
        case ImGuiKey_F5:
            break;
        case ImGuiKey_F6:
            break;
        case ImGuiKey_F7:
            break;
        case ImGuiKey_F8:
            break;
        case ImGuiKey_F9:
            break;
        case ImGuiKey_F10:
            break;
        case ImGuiKey_F11:
            break;
        case ImGuiKey_F12:
            break;
        case ImGuiKey_Apostrophe: // '
            break;
        case ImGuiKey_Comma: // ,
            break;
        case ImGuiKey_Minus: // -
            if (shift) {
                keysym = "plus";
            } else {
                keysym = "minus";
            }
            break;
        case ImGuiKey_Period: // .
            break;
        case ImGuiKey_Slash: // /
            break;
        case ImGuiKey_Semicolon: // ;
            break;
        case ImGuiKey_Equal: // =
            break;
        case ImGuiKey_LeftBracket: // [
            break;
        case ImGuiKey_Backslash: // \ backslash
            break;
        case ImGuiKey_RightBracket: // ]
            break;
        case ImGuiKey_GraveAccent: // `
            break;
        default:
            // default to zero
            keysym = 0;
            break;
        }

        return keysym;
    };

    auto ImGuiKeyToKeyCode = [](const ImGuiKey& key) {
        char keycode = 0;
        switch (key) {
        case ImGuiKey_LeftArrow:
            break;
        case ImGuiKey_RightArrow:
            break;
        case ImGuiKey_UpArrow:
            break;
        case ImGuiKey_DownArrow:
            break;
        case ImGuiKey_PageUp:
            break;
        case ImGuiKey_PageDown:
            break;
        case ImGuiKey_Home:
            break;
        case ImGuiKey_End:
            break;
        case ImGuiKey_Insert:
            break;
        case ImGuiKey_Delete:
            break;
        case ImGuiKey_Backspace:
            break;
        case ImGuiKey_Space:
            break;
        case ImGuiKey_Enter:
            break;
        case ImGuiKey_Escape:
            break;
        case ImGuiKey_LeftCtrl:
            break;
        case ImGuiKey_LeftShift:
            break;
        case ImGuiKey_LeftAlt:
            break;
        case ImGuiKey_LeftSuper:
            break;
        case ImGuiKey_RightCtrl:
            break;
        case ImGuiKey_RightShift:
            break;
        case ImGuiKey_RightAlt:
            break;
        case ImGuiKey_RightSuper:
            break;
        case ImGuiKey_Menu:
            break;
        case ImGuiKey_0:
            keycode = '0';
            break;
        case ImGuiKey_1:
            keycode = '1';
            break;
        case ImGuiKey_2:
            keycode = '2';
            break;
        case ImGuiKey_3:
            keycode = '3';
            break;
        case ImGuiKey_4:
            keycode = '4';
            break;
        case ImGuiKey_5:
            keycode = '5';
            break;
        case ImGuiKey_6:
            keycode = '6';
            break;
        case ImGuiKey_7:
            keycode = '7';
            break;
        case ImGuiKey_8:
            keycode = '8';
            break;
        case ImGuiKey_9:
            keycode = '9';
            break;
        case ImGuiKey_A:
            keycode = 'A';
            break;
        case ImGuiKey_B:
            keycode = 'B';
            break;
        case ImGuiKey_C:
            keycode = 'C';
            break;
        case ImGuiKey_D:
            keycode = 'D';
            break;
        case ImGuiKey_E:
            keycode = 'E';
            break;
        case ImGuiKey_F:
            keycode = 'F';
            break;
        case ImGuiKey_G:
            keycode = 'G';
            break;
        case ImGuiKey_H:
            keycode = 'H';
            break;
        case ImGuiKey_I:
            keycode = 'I';
            break;
        case ImGuiKey_J:
            keycode = 'J';
            break;
        case ImGuiKey_K:
            keycode = 'K';
            break;
        case ImGuiKey_L:
            keycode = 'L';
            break;
        case ImGuiKey_M:
            keycode = 'M';
            break;
        case ImGuiKey_N:
            keycode = 'N';
            break;
        case ImGuiKey_O:
            keycode = 'O';
            break;
        case ImGuiKey_P:
            keycode = 'P';
            break;
        case ImGuiKey_Q:
            keycode = 'Q';
            break;
        case ImGuiKey_R:
            keycode = 'R';
            break;
        case ImGuiKey_S:
            keycode = 'S';
            break;
        case ImGuiKey_T:
            keycode = 'T';
            break;
        case ImGuiKey_U:
            keycode = 'U';
            break;
        case ImGuiKey_V:
            keycode = 'V';
            break;
        case ImGuiKey_W:
            keycode = 'W';
            break;
        case ImGuiKey_X:
            keycode = 'X';
            break;
        case ImGuiKey_Y:
            keycode = 'Y';
            break;
        case ImGuiKey_Z:
            keycode = 'Z';
            break;
        case ImGuiKey_F1:
            break;
        case ImGuiKey_F2:
            break;
        case ImGuiKey_F3:
            break;
        case ImGuiKey_F4:
            break;
        case ImGuiKey_F5:
            break;
        case ImGuiKey_F6:
            break;
        case ImGuiKey_F7:
            break;
        case ImGuiKey_F8:
            break;
        case ImGuiKey_F9:
            break;
        case ImGuiKey_F10:
            break;
        case ImGuiKey_F11:
            break;
        case ImGuiKey_F12:
            break;
        default:
            // default to zero
            keycode = 0;
            break;
        }

        return keycode;
    };

    // Keyboard Events
    if (is_focused && interactor->HasObserver(vtkCommand::KeyPressEvent)) {
        for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END;
             ++key) {
            if (ImGui::IsKeyPressed((ImGuiKey)key)) {
                interactor->SetKeySym(ImGuiKeyToKeySym((ImGuiKey)key, (bool)shift));
                interactor->SetKeyCode(ImGuiKeyToKeyCode((ImGuiKey)key));
                interactor->InvokeEvent(vtkCommand::KeyPressEvent, nullptr);
            }
        }
    }

    // Keyboard Release
    if (interactor->HasObserver(vtkCommand::KeyReleaseEvent)) {
        for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END;
             ++key) {
            if (ImGui::IsKeyReleased((ImGuiKey)key)) {
                interactor->SetKeySym(ImGuiKeyToKeySym((ImGuiKey)key, (bool)shift));
                interactor->SetKeyCode(ImGuiKeyToKeyCode((ImGuiKey)key));
                interactor->InvokeEvent(vtkCommand::KeyReleaseEvent, nullptr);
            }
        }
    }
}

VtkViewer::VtkViewer()
    : viewportWidth(0)
    , viewportHeight(0)
    , renderWindow(nullptr)
    , interactor(nullptr)
    , interactorStyle(nullptr)
    , renderer(nullptr)
    , tex(0)
    , firstRender(true) {
    init();
}

VtkViewer::VtkViewer(const VtkViewer& vtkViewer)
    : viewportWidth(0)
    , viewportHeight(0)
    , renderWindow(vtkViewer.renderWindow)
    , interactor(vtkViewer.interactor)
    , interactorStyle(vtkViewer.interactorStyle)
    , renderer(vtkViewer.renderer)
    , tex(vtkViewer.tex)
    , firstRender(vtkViewer.firstRender) {
}

VtkViewer::VtkViewer(VtkViewer&& vtkViewer) noexcept
    : viewportWidth(0)
    , viewportHeight(0)
    , renderWindow(std::move(vtkViewer.renderWindow))
    , interactor(std::move(vtkViewer.interactor))
    , interactorStyle(std::move(vtkViewer.interactorStyle))
    , renderer(std::move(vtkViewer.renderer))
    , tex(vtkViewer.tex)
    , firstRender(vtkViewer.firstRender) {
}

VtkViewer::~VtkViewer() {
    renderer = nullptr;
    interactorStyle = nullptr;
    interactor = nullptr;
    renderWindow = nullptr;

    glDeleteTextures(1, &tex);
}

VtkViewer& VtkViewer::operator=(const VtkViewer& vtkViewer) {
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

void VtkViewer::init() {
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->ResetCamera();
    renderer->SetBackground(DEFAULT_BACKGROUND);
    renderer->SetBackgroundAlpha(DEFAULT_ALPHA);

    interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    interactorStyle->SetDefaultRenderer(renderer);

    interactor = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
    interactor->SetInteractorStyle(interactorStyle);
    interactor->EnableRenderOff();

    int viewportSize[2] = { static_cast<int>(viewportWidth), static_cast<int>(viewportHeight) };

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

    if (!renderer || !interactorStyle || !renderWindow || !interactor) {
        throw VtkViewerError("Couldn't initialize VtkViewer");
    }
}

void VtkViewer::render() { render(ImGui::GetContentRegionAvail()); }
void VtkViewer::render(const ImVec2 size) {
    setViewportSize(size);

    renderWindow->Render();
    renderWindow->WaitForCompletion();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::BeginChild("##Viewport", size, true, VtkViewer::NoScrollFlags());
    ImGui::Image(reinterpret_cast<void*>(tex), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
    processEvents();
    ImGui::EndChild();
    ImGui::PopStyleVar();
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

void VtkViewer::setViewportSize(const ImVec2 newSize) {
    if (((viewportWidth == newSize.x && viewportHeight == newSize.y) || newSize.x <= 0 || newSize.y <= 0) && !firstRender) {
        return;
    }

    viewportWidth = static_cast<unsigned int>(newSize.x);
    viewportHeight = static_cast<unsigned int>(newSize.y);

    int viewportSize[] = { static_cast<int>(newSize.x),
        static_cast<int>(newSize.y) };

    auto dpiScale = ImGui::GetWindowDpiScale();

    // Free old buffers
    glDeleteTextures(1, &tex);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewportWidth * dpiScale, viewportHeight * dpiScale, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    renderWindow->InitializeFromCurrentContext();
    renderWindow->SetSize(viewportSize[0] * dpiScale, viewportSize[1] * dpiScale);
    renderWindow->SetDPI(72 * dpiScale);
    interactor->SetSize(viewportSize[0] * dpiScale, viewportSize[1] * dpiScale);

    auto vtkfbo = renderWindow->GetDisplayFramebuffer();
    vtkfbo->Bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    vtkfbo->UnBind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    firstRender = false;
}
