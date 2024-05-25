// Created by block on 5/25/24.

#include <fstream>
#include "SSTV.hpp"

namespace engine::core::experiments {

	SSTV &SSTV::The() {
		static SSTV the;
		return the;
	}

	double SSTV::GetPhaseInc(float freq) {
		return freq * (FTWO_PI / samplerate);
	}

	void SSTV::DoTheThing(SDL_Rect rect) {
		// big nasty

		samples.clear();
		current_time = 0;
		phase = 0;

		int instruction_index = 0;
		float started_instruction_at = 0;

		// add the pixel instructions
		// todo: make the scan delegation nicer and handled on a per-mode basis
		for (int i = 0; i < 256; i++) {
			instructions.push_back({"(1) Sync pulse", 4.862, 1200});
			instructions.push_back({"(2) Sync porch", 0.572, 1500});

			// signals are 1500 - 2300Hz
			// 0-255 can be found with: 1500 + (byte * 3.1372549)
			//float color = 1500.f + (i * 3.1372549f);

			instructions.push_back({"(3) Green scan", 73.216, 0});
			instructions.push_back({"(4) Separator pulse", 0.572, 1500});
			instructions.push_back({"(5) Blue scan", 73.216, 0});
			instructions.push_back({"(6) Separator pulse", 0.572, 1500});
			instructions.push_back({"(7) Red scan", 73.216, 0});
			instructions.push_back({"(8) Separator pulse", 0.572, 1500});
		}

		// sampling helpers for sampling the screen
		// do it all right now to save time!
		// eventually the "realtime" idea will happen, but let's keep it simple for now
		std::vector<std::uint8_t> pixels;
		pixels.reserve(4*rect.w*rect.h);
		std::fill(pixels.begin(), pixels.end(), 127);
		glReadPixels(0, 0, rect.w, rect.h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

		std::uint8_t pass = 99; // 0/1/2 for GBR
		int cur_x = -1;
		int cur_y = -1;

		// working out letterboxing
		float widthcheck = 320/(float)rect.w;
		// store the result in rect.x/y since it's unused
		rect.x = rect.w*widthcheck; // should be equal to our width
		rect.y = rect.h*widthcheck; // adjusted height

		bool cooking = true;
		while (cooking) {
			// i don't trust floating points, let's hold the time and add in chunks
			float hold_time = current_time;

			for (int i = 0; i < samples_per_division; i++) {
				Instruction* ins = &instructions[instruction_index];

				if (current_time + time_per_sample > started_instruction_at + (ins->length_ms/1000.f)) {
					// new instruction!
					instruction_index++;
					if (instruction_index >= instructions.size()) {
						// we've hit the end
						cooking = false;
						break;
					}

					ins = &instructions[instruction_index];

					started_instruction_at = current_time;
					//LogInfo("New instruction: {}", ins->stage);

					if (ins->pitch == 0) {
						// we're about to do a new scan, set some stuff
						pass++;
						if (pass >= 3) {
							// new line
							cur_y++;
							cur_x = -1;
							pass = 0;
							//LogInfo("Finished line: {}", cur_y-1);
						}

					}
				}

				float pitch = ins->pitch;
				if (pitch == 0) {
					// calculate pixel

					// letterboxing
					// if we have an aspect difference, this will be able to letterbox everything
					int aspect_difference = 256 - rect.y;

					if (aspect_difference > 0 && (cur_y < aspect_difference/2 || cur_y > ((aspect_difference/2) + rect.y))) {
						pitch = 1500 + (2300-1500)/2; // grey
					}
					else {
						// where we're at along our scanline
						float widthfrac = (current_time - started_instruction_at) / (ins->length_ms/1000.f);
						float heightfrac = (1.f - ((cur_y-(aspect_difference/2)) / (float)rect.y));

						int sample_x = rect.w * widthfrac;
						int sample_y = rect.h * heightfrac;

						cur_x = sample_x;

						// get pixel at that sample (no nice filtering...)
						std::uint8_t* pixel = &pixels[(sample_x + ((sample_y-1) * rect.w)) * 4];

						// for bytes - (2300-1500 / 255)
						pitch = 1500 + (pixel[(pass+1) % 3] * 3.1372549);
					}
				}

				// we need to see how the phase will increase for the frequency we want
				// this is where the smooth pitch changes happen
				phase += GetPhaseInc(pitch);
				if (phase == INFINITY)
					LogError("Fug");
				else
					while (phase > FTWO_PI * 2)
						phase -= FTWO_PI * 2;

				// add to the list of samples
				samples.push_back(osc.Value(phase));
				current_time += time_per_sample;
			}

			current_time = hold_time + timestep;

			// precaution
			if (current_time > 65.0f)
				cooking = false;
		}

		// write to file
		std::ofstream file(core::filesystem::Filesystem::GetDataDir() / "data/sstv/sstv.f32", std::ios::binary);
		file.write(reinterpret_cast<const char *>(samples.data()), samples.size() * sizeof(float));
		file.close();
	}

} // engine::core::experiments
