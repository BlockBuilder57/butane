// Created by block on 2025-07-23.

#pragma once

#include <core/sdl/Window.hpp>

namespace butane::wrap {

	namespace fasstv {
		void UpdatePixels();
		std::uint8_t* GetSampleFromSurface(int sample_x, int sample_y);
	};

	namespace imgui {
		void Init();
		void SetDefaultStyle();
		void Shutdown();

		void NewFrame();
		void Render();
		void RenderDrawData();
	};

	namespace physfs {
		void Init(int argc, char** argv);
		void Shutdown();
	};

	namespace sdl {
		void DumpOGLInfo();

		int Init();
		core::sdl::Window* CreateWindow();
		void SetupEventHandlers();
		void Shutdown();
	};

	namespace systems {
		void RegisterSystemsAndInit();
	};

};