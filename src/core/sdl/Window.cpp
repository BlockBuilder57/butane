#include <core/sdl/Window.hpp>

namespace engine::core::sdl {

        Window::Window(const std::string_view title, u32 width, u32 height) {
			window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
			context = SDL_GL_CreateContext(window);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
			SDL_GL_MakeCurrent(window, context);

            // Disable VSync by default; later on this probably should be configurable
            SDL_GL_SetSwapInterval(0);
		}

        Window::~Window() {
			SDL_GL_DeleteContext(context);
			SDL_DestroyWindow(window);
		}


		void Window::Swap() { SDL_GL_SwapWindow(window); }

		void Window::Poll() {
			SDL_Event ev;
			while(SDL_PollEvent(&ev) > 0) {
				// Call the given event handler if found
				if(events.find(ev.type) != events.end()) {
					events[ev.type](ev);
				}
			}
		}

		void Window::On(SDL_EventType ev, const EventHandlerFn& fn) { events[ev] = fn; }

}