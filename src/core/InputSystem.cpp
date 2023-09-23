// Created by block on 8/21/23.

#include "InputSystem.hpp"

#include <imgui.h>

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
			SDL_WarpMouseInWindow(sdl::Window::CurrentWindow->Raw(), 1280 / 2, 720 / 2);
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

	bool InputSystem::ButtonDown(SDL_Scancode key) {
		return statusCurrent.KeyPressed(key) && !statusLast.KeyPressed(key);
	}
	bool InputSystem::ButtonHeld(SDL_Scancode key) {
		return statusCurrent.KeyPressed(key);
	}
	bool InputSystem::ButtonUp(SDL_Scancode key) {
		return !statusCurrent.KeyPressed(key) && statusLast.KeyPressed(key);
	}

	bool InputSystem::ButtonDown(SDL_Scancode key, SDL_Keymod modifiers) {
		return statusCurrent.KeyPressed(key, modifiers) && !statusLast.KeyPressed(key, modifiers);
	}
	bool InputSystem::ButtonHeld(SDL_Scancode key, SDL_Keymod modifiers) {
		return statusCurrent.KeyPressed(key, modifiers);
	}
	bool InputSystem::ButtonUp(SDL_Scancode key, SDL_Keymod modifiers) {
		return !statusCurrent.KeyPressed(key, modifiers) && statusLast.KeyPressed(key, modifiers);
	}

	Bind* InputSystem::RegisterBind(std::string name, std::vector<SDL_Scancode> keys, SDL_Keymod modifiers) {
		return registeredBinds.emplace_back(new Bind(name, keys, modifiers, this));
	}

	void InputSystem::ImGuiBindStatus() {
		ImGui::SeparatorText("Binds");

		ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

		if (ImGui::BeginTable("table", 5, flags)) {
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Keys");
			ImGui::TableSetupColumn("Down");
			ImGui::TableSetupColumn("Held");
			ImGui::TableSetupColumn("Up");
			ImGui::TableHeadersRow();

			for(Bind* bind : registeredBinds) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(bind->name.c_str());

				ImGui::TableSetColumnIndex(1);
				std::string keystr = "";

				if ((bind->keyModifiers & SDL_Keymod::KMOD_CTRL) != 0)
					keystr += "CTRL+";
				if ((bind->keyModifiers & SDL_Keymod::KMOD_SHIFT) != 0)
					keystr += "SHIFT+";
				if ((bind->keyModifiers & SDL_Keymod::KMOD_ALT) != 0)
					keystr += "ALT+";
				if ((bind->keyModifiers & SDL_Keymod::KMOD_GUI) != 0)
					keystr += "GUI+";

				for(int i = 0; i < bind->keyArray.size(); i++) {
					keystr += SDL_GetKeyName(SDL_GetKeyFromScancode(bind->keyArray[i]));
					keystr += "+";
				}

				// lazy stripping
				if (keystr.ends_with('+'))
					keystr = keystr.substr(0, keystr.size() - 1);

				ImGui::TextUnformatted(keystr.c_str());

				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%d", bind->Down());
				ImGui::TableSetColumnIndex(3);
				ImGui::Text("%d", bind->Held());
				ImGui::TableSetColumnIndex(4);
				ImGui::Text("%d", bind->Up());
			}

			ImGui::EndTable();
		}
	}

} // namespace engine::core
