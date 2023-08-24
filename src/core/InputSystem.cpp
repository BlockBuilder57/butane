// Created by block on 8/21/23.

#include "InputSystem.hpp"

namespace engine::core {

	InputSystem::InputStatus InputSystem::statusCurrent = {};
	InputSystem::InputStatus InputSystem::statusLast = {};

	InputSystem& InputSystem::The() {
		static InputSystem the;
		return the;
	}

	void InputSystem::Init() {
		statusCurrent.keys.reserve(MAX_KEY_COUNT);
		statusLast.keys.reserve(MAX_KEY_COUNT);
	}

	void InputSystem::Shutdown() {
		SetMouseLock(false);
	}

	void InputSystem::StartTick() {
		// fetch mouse state here
		statusCurrent.mouseButtons = SDL_GetMouseState(&statusCurrent.mouseRelative.x, &statusCurrent.mouseRelative.y);
		statusCurrent.mouseButtons = SDL_GetRelativeMouseState(&statusCurrent.mouseDelta.x, &statusCurrent.mouseDelta.y);

		if (IsMouseLocked() && sdl::Window::CurrentWindow != nullptr) {
			// lock to screen
			SDL_WarpMouseInWindow(sdl::Window::CurrentWindow->Raw(), 800 / 2, 600 / 2);
		}
	}

	void InputSystem::EndTick() {
		// set last state to the full current state
		// do this last!!
		statusLast = statusCurrent;
	}

	void InputSystem::KeyEvent(const SDL_KeyboardEvent& ev) {
		if (ev.repeat)
			return;

		if (ev.type == SDL_KEYDOWN && statusCurrent.keys.size() < MAX_KEY_COUNT) {
			// add if it doesn't exist yet
			if(std::find(statusCurrent.keys.begin(), statusCurrent.keys.end(), ev.keysym.scancode) == statusCurrent.keys.end())
				statusCurrent.keys.push_back(ev.keysym.scancode);

			//LogInfo("Key down: {}", (int)keycode);
		}
		else if (ev.type == SDL_KEYUP) {
			// remove from list
			statusCurrent.keys.erase(std::remove(statusCurrent.keys.begin(), statusCurrent.keys.end(), ev.keysym.scancode), statusCurrent.keys.end());

			//LogInfo("Key up: {}", (int)keycode);
		}

		// update keymods
		statusCurrent.modifiers = static_cast<SDL_Keymod>(ev.keysym.mod);
	}


	glm::vec<2, int> InputSystem::GetMouseDelta() {
		return statusCurrent.mouseDelta;
	}

	bool InputSystem::ButtonDown(SDL_Scancode key, SDL_Keymod modifiers /*= SDL_Keymod::KMOD_NONE*/) {
		return statusCurrent.KeyPressed(key, modifiers) && !statusLast.KeyPressed(key, modifiers);
	}
	bool InputSystem::ButtonHeld(SDL_Scancode key, SDL_Keymod modifiers /*= SDL_Keymod::KMOD_NONE*/) {
		return statusCurrent.KeyPressed(key, modifiers);
	}
	bool InputSystem::ButtonUp(SDL_Scancode key, SDL_Keymod modifiers /*= SDL_Keymod::KMOD_NONE*/) {
		return !statusCurrent.KeyPressed(key, modifiers) && statusLast.KeyPressed(key, modifiers);
	}

} // namespace engine::core
