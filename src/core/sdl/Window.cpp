#include <backends/imgui_impl_sdl2.h>

#include <core/sdl/Window.hpp>

namespace butane::core::sdl {

	Window* Window::CurrentWindow = nullptr;

	Window::Window(const std::string_view title, u32 width, u32 height) {
		window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		context = SDL_GL_CreateContext(window);
		SDL_GL_MakeCurrent(window, context);

		// Enable VSync by default; later on this probably should be configurable
		SDL_GL_SetSwapInterval(1);

		keepOpen = true;
	}

	Window::~Window() {
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
	}

	void Window::Swap() {
		SDL_GL_SwapWindow(window);
	}

	void Window::Poll() {
		SDL_Event ev;
		while(SDL_PollEvent(&ev) > 0) {
			ImGui_ImplSDL2_ProcessEvent(&ev);

			// Call the given event handler if found
			if(events.find(ev.type) != events.end()) {
				events[ev.type](ev);
			}
		}
	}

	void Window::On(SDL_EventType ev, const EventHandlerFn& fn) {
		events[ev] = fn;
	}

	SDL_Rect Window::GetRect() {
		SDL_Rect rect {};
		SDL_GetWindowSize(window, &rect.w, &rect.h);
		return rect;
	}
	float Window::GetAspectRatio() {
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		return (float)w / (float)h;
	}

} // namespace butane::core::sdl