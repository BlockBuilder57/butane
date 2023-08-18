#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_video.h>

#include <core/Types.hpp>
#include <functional>
#include <unordered_map>

namespace engine::core::sdl {

	/// A basic abstraction over SDL2 windowing facilities.
	struct Window {
        // use function_ref?
		using EventHandlerFn = std::function<void(SDL_Event&)>;

		Window(const std::string_view title, u32 width, u32 height);

		~Window();

		SDL_Window* Raw() const { return window; }

		SDL_GLContext GlContext() const { return context; }

		void Swap();

		void Poll();

		void On(SDL_EventType ev, const EventHandlerFn& fn);

	   private:
		SDL_Window* window {};
		SDL_GLContext context {};
		std::unordered_map<u32, EventHandlerFn> events {};
	};

} // namespace engine::core::sdl