#pragma once

#include <iostream>
#include <string>
#include <exception>

#include "imgui/imgui.h"

#include <vtkProp.h>
#include <vtkPropCollection.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>

// Specific OpenGL ES versions
//#define IMGUI_IMPL_OPENGL_ES2     // Auto-detected on Emscripten
//#define IMGUI_IMPL_OPENGL_ES3     // Auto-detected on iOS/Android

// Attempt to auto-detect the default Desktop GL loader based on available header files.
// If auto-detection fails or doesn't select the same GL loader file as used by your application,
// you are likely to get a crash in ImGui_ImplOpenGL3_Init().
// You can explicitly select a loader by using one of the '#define IMGUI_IMPL_OPENGL_LOADER_XXX' in imconfig.h or compiler command-line.
#if !defined(IMGUI_IMPL_OPENGL_ES2) \
 && !defined(IMGUI_IMPL_OPENGL_ES3) \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GL3W) \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLEW) \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLAD) \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2) \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2) \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3) \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

// Try to detect GLES on matching platforms
#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif
#if (defined(__APPLE__) && (TARGET_OS_IOS || TARGET_OS_TV)) || (defined(__ANDROID__))
#define IMGUI_IMPL_OPENGL_ES3               // iOS, Android  -> GL ES 3, "#version 300 es"
#elif defined(__EMSCRIPTEN__)
#define IMGUI_IMPL_OPENGL_ES2               // Emscripten    -> GL ES 2, "#version 100"

// Otherwise try to detect supported Desktop OpenGL loaders..
#elif defined(__has_include)
#if __has_include(<GL/glew.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#elif __has_include(<glad/glad.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#elif __has_include(<glad/gl.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GLAD2
#elif __has_include(<GL/gl3w.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GL3W
#elif __has_include(<glbinding/glbinding.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GLBINDING3
#elif __has_include(<glbinding/Binding.h>)
#define IMGUI_IMPL_OPENGL_LOADER_GLBINDING2
#else
#error "Cannot detect OpenGL loader!"
#endif
#else
#define IMGUI_IMPL_OPENGL_LOADER_GL3W   // Default to GL3W embedded in our repository
#endif

#endif

#define DEFAULT_TITLE "VtkViewer"
#define DEFAULT_VIEWPORT_WIDTH 640
#define DEFAULT_VIEWPORT_HEIGHT 480
#define DEFAULT_BACKGROUND 0, 0, 0

class VtkViewerError : public std::runtime_error {
public:
	VtkViewerError() = default;
	explicit VtkViewerError(const std::string& message) throw() : std::runtime_error(message) {}
	~VtkViewerError() = default;
public:
	virtual char const* what() const throw() {
		return exception::what();
	}
};

class VtkViewer {
private:
	struct VtkViewerCallback {
		static vtkSmartPointer<vtkGenericRenderWindowInteractor> interactor;
		static void isCurrentCallbackFn(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
		static void processEvents();
	};
private:
	vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
	vtkSmartPointer<vtkGenericRenderWindowInteractor> interactor;
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> interactorStyle;
	vtkSmartPointer<vtkRenderer> renderer;
private:
	std::string title;
	unsigned int viewportWidth, viewportHeight;
	unsigned int fbo, rbo, tex;
	bool show;
public:
	VtkViewer();
	VtkViewer(const std::string& _title, unsigned int _viewportWidth, unsigned int _viewportHeight);
	VtkViewer(const VtkViewer& vtkViewer);
	VtkViewer(VtkViewer&& vtkViewer) noexcept;
	~VtkViewer();

	VtkViewer& operator=(const VtkViewer& vtkViewer);
private:
	IMGUI_IMPL_API void init();
public:
	IMGUI_IMPL_API void render();
	IMGUI_IMPL_API void addActor(const vtkSmartPointer<vtkProp>& actor);
	IMGUI_IMPL_API void addActors(const vtkSmartPointer<vtkPropCollection>& actors);
	IMGUI_IMPL_API void removeActor(const vtkSmartPointer<vtkProp>& actor);
	void setViewportSize(unsigned int viewportWidth, unsigned int viewportHeight);
public:
	inline void setTitle(const std::string& title) {
		this->title = title;
	}
public:
	inline vtkSmartPointer<vtkGenericOpenGLRenderWindow>& getRenderWindow() {
		return renderWindow;
	}

	inline vtkSmartPointer<vtkGenericRenderWindowInteractor>& getInteractor() {
		return interactor;
	}

	inline vtkSmartPointer<vtkInteractorStyleTrackballCamera>& getInteractorStyle() {
		return interactorStyle;
	}

	inline vtkSmartPointer<vtkRenderer>& getRenderer() {
		return renderer;
	}
public:
	inline const std::string& getTitle() {
		return title;
	}

	inline unsigned int getViewportWidth() const {
		return viewportWidth;
	}

	inline unsigned int getViewportHeight() const {
		return viewportHeight;
	}

	inline unsigned int getFbo() const {
		return fbo;
	}

	inline unsigned int getRbo() const {
		return rbo;
	}

	inline unsigned int getTexture() const {
		return tex;
	}

	inline bool getShow() const {
		return show;
	}
};