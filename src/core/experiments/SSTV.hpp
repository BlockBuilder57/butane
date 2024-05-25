// Created by block on 5/25/24.

#pragma once

#include <core/gl/Texture.hpp>
#include <core/Logger.hpp>

#include <vector>
#include <array>
#include "Oscillator.hpp"

namespace engine::core::experiments {

	class SSTV {
	public:
		static SSTV &The();

		struct Mode {
			std::string name;
			int width;
			int height;
		};

		struct Instruction {
			std::string stage;
			float length_ms;
			float pitch;
		};

		std::vector<Instruction> instructions = {
				// VIS, hardcoded for Martin 2
				{"Leader 1",   300, 1900},
				{"break",      10,  1200},
				{"Leader 2",   300, 1900},
				{"VIS start",  30,  1200},
				{"VIS bit 0",  30,  1300}, // 0
				{"VIS bit 1",  30,  1300}, // 0
				{"VIS bit 2",  30,  1300}, // 0
				{"VIS bit 3",  30,  1100}, // 1
				{"VIS bit 4",  30,  1300}, // 0
				{"VIS bit 5",  30,  1100}, // 1
				{"VIS bit 6",  30,  1300}, // 0
				{"VIS parity", 30,  1300},
				{"VIS stop",   30,  1200}
		};

		const int samplerate = 44100;
		const int samples_per_division = 320;
		const float timestep = (float) samples_per_division / samplerate;
		const float time_per_sample = timestep / samples_per_division;
		std::vector<float> samples{};

		float current_time = 0;
		float phase = 0;

		Oscillator osc{kOsc_Sin};

		double GetPhaseInc(float freq);

		void DoTheThing(SDL_Rect rect);
	};

} // engine::core::experiments
