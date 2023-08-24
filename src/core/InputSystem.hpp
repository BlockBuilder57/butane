// Created by block on 8/21/23.

#pragma once

#include <SDL2/SDL.h>

#include <algorithm>
#include <array>
#include <core/sdl/Window.hpp>
#include <core/StaticVector.hpp>
#include <vector>

#include <glm/glm.hpp>

#include "Logger.hpp"
#include "System.hpp"

namespace engine::core {

	constexpr static auto MAX_KEY_COUNT = 64;

	struct InputSystem : public core::PerTickSystem {
		static InputSystem& The();

		// implementation of [core::PerTickSystem] interface
		void Init() override;
		void Shutdown() override;
		void StartTick() override;
		void EndTick() override;

		bool IsMouseLocked() { return isMouseLocked; }
		void SetMouseLock(bool val) {
			if (sdl::Window::CurrentWindow != nullptr) {
				isMouseLocked = val;
				SDL_SetRelativeMouseMode((SDL_bool) isMouseLocked);
				SDL_SetWindowGrab(sdl::Window::CurrentWindow->Raw(), (SDL_bool) isMouseLocked);
			}
			else {
				core::LogError("Null window when trying to set mouse lock!");
			}
		}

		void KeyEvent(const SDL_KeyboardEvent& ev);

		glm::vec<2, int> GetMouseDelta();

		bool ButtonDown(SDL_Scancode key, SDL_Keymod modifiers = SDL_Keymod::KMOD_NONE);
		bool ButtonHeld(SDL_Scancode key, SDL_Keymod modifiers = SDL_Keymod::KMOD_NONE);
		bool ButtonUp(SDL_Scancode key, SDL_Keymod modifiers = SDL_Keymod::KMOD_NONE);

	   private:
		struct InputStatus {
			// keyboard
			std::vector<SDL_Scancode> keys;
			SDL_Keymod modifiers;

			// mouse
			unsigned int mouseButtons;
			glm::vec<2, int> mouseRelative;
			glm::vec<2, int> mouseDelta;

			// controller
			// todo

			void Clear() {
				keys.clear();
				modifiers = SDL_Keymod::KMOD_NONE;

				mouseButtons = 0;
				mouseRelative = {};
				mouseDelta = {};
			}

			bool ModifiersValid(SDL_Keymod modifier) {
				if(modifiers == SDL_Keymod::KMOD_NONE && modifier != SDL_Keymod::KMOD_NONE)
					return false;

				return (modifier & modifiers) == modifiers;
			}

			bool KeyPressed(SDL_Scancode key) { return std::find(keys.begin(), keys.end(), key) != keys.end(); }

			bool KeyPressed(SDL_Scancode key, SDL_Keymod modifier) {
				return std::find(keys.begin(), keys.end(), key) != keys.end() && ModifiersValid(modifier);
			}
		};

		static InputStatus statusCurrent;
		static InputStatus statusLast;
		bool isMouseLocked;
	};

	extern InputSystem* inputSystem;

} // namespace engine::core
