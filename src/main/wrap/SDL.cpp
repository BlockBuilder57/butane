// Created by block on 2025-07-23.

#include "Wrap.hpp"

#include <core/InputSystem.hpp>
#include <core/Logger.hpp>
#include <core/rendering/GLHeaders.hpp>
#include <core/sdl/Window.hpp>

namespace butane::wrap::sdl {

	void DumpOGLInfo() {
		int maj;
		int min;

		core::LogInfo("OpenGL Information:");
		core::LogInfo("Vendor   : {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		core::LogInfo("Renderer : {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		core::LogInfo("Version  : {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
		core::LogInfo("GLSL     : {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &maj);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &min);
		core::LogInfo("Context  : {}.{}", maj, min);

		glGetIntegerv(GL_MAJOR_VERSION, &maj);
		glGetIntegerv(GL_MINOR_VERSION, &min);
		core::LogInfo("OpenGL   : {}.{}", maj, min);
	}

	int Init() {
		if(SDL_Init(SDL_INIT_EVERYTHING) > 0) {
			core::LogFatal("Failed to initialize SDL; giving up");
			return 1;
		}

		return 0;
	}

	core::sdl::Window* CreateWindow() {
		core::sdl::Window::CurrentWindow = new core::sdl::Window("butane", 1280, 720);

		// By this point the Window class has set up OpenGL and made the context it created current,
		// so now we can load OpenGL APIs.
		gladLoadGLLoader(SDL_GL_GetProcAddress);

		butane::wrap::sdl::DumpOGLInfo();

		return core::sdl::Window::CurrentWindow;
	}

	void SetupEventHandlers() {
		core::sdl::Window* window = core::sdl::Window::CurrentWindow;

		window->On(SDL_QUIT, [&](SDL_Event& ev) {
			static_cast<void>(ev);

			if (core::sdl::Window::CurrentWindow)
				core::sdl::Window::CurrentWindow->SetKeepOpen(false);
		});

		window->On(SDL_KEYDOWN, [&](SDL_Event& ev) {
			core::InputSystem::The().KeyEvent(ev.key);
		});
		window->On(SDL_KEYUP, [&](SDL_Event& ev) {
			core::InputSystem::The().KeyEvent(ev.key);
		});
	}

	void Shutdown() {
		SDL_Quit();
	}

};
