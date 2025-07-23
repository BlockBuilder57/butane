// Created by block on 2025-07-23.

#include "Wrap.hpp"

#include <SDL2/SDL.h>
#include <libfasstv/ExportUtils.hpp>
#include <libfasstv/libfasstv.hpp>

#include <core/rendering/GLHeaders.hpp>
#include <core/sdl/Window.hpp>
#include <core/TimeSystem.hpp>

namespace butane::wrap::fasstv {

	std::uint8_t colorHolder[4] = {};
	std::uint32_t lastSample = -1;
	std::uint64_t lastTick = 0;
	std::vector<std::uint8_t> pixels;

	void UpdatePixels() {
		std::uint64_t ticks = core::TimeSystem::The().Ticks();
		if (ticks <= lastTick)
			return;

		lastTick = ticks;

		::fasstv::SSTV::Mode* mode = ::fasstv::SSTVEncode::The().GetMode();

		SDL_Rect rect = {0, 0, 1280, 720};
		if (core::sdl::Window::CurrentWindow != nullptr)
			rect = core::sdl::Window::CurrentWindow->GetRect();

		if (pixels.capacity() != 4 * rect.w * rect.h) {
			core::LogDebug("upping pixel buffer");
			pixels.clear();
			pixels.resize(4 * rect.w * rect.h);
		}

		core::LogDebug("Getting shit");
		glReadPixels(0, 0, rect.w, rect.h, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	}

	std::uint8_t* GetSampleFromSurface(int sample_x, int sample_y) {
		std::uint32_t curSample = sample_x | (sample_y << 16);

		if (curSample != lastSample) {
			UpdatePixels();
			::fasstv::SSTV::Mode* mode = ::fasstv::SSTVEncode::The().GetMode();

			SDL_Rect rect = {0, 0, 1280, 720};
			if (core::sdl::Window::CurrentWindow != nullptr)
				rect = core::sdl::Window::CurrentWindow->GetRect();

			// get pixel at sample (no nice filtering...)
			int pixel_idx = sample_x + (((rect.h - 1) - sample_y) * rect.w);
			pixel_idx = std::clamp(pixel_idx, 0, (rect.w * rect.h) - 1);
			//memcpy(&colorHolder[0], &pixels[pixel_idx * 4], 4);

			for (int i = 0; i < 3; i++) {
				colorHolder[i] = pixels[(pixel_idx * 4) + i];
			}
			colorHolder[3] = 255;

			//SDL_ReadSurfacePixel(SampleSurface, sample_x, sample_y, &colorHolder[0], &colorHolder[1], &colorHolder[2], &colorHolder[3]);
			//core::LogDebug("Sample ({}, {}) read #{:02x}{:02x}{:02x}{:02x}", sample_x, sample_y, colorHolder[0], colorHolder[1], colorHolder[2], colorHolder[3]);
			lastSample = curSample;
		}

		// lazy alpha application
		for (int i = 0; i < 3; i++)
			colorHolder[i] *= (colorHolder[3] / 255.f);

		return &colorHolder[0];
	}

};
