// Created by block on 5/25/24.

#include "SSTV.hpp"

#include <fstream>

namespace engine::core::experiments {

	SSTV& SSTV::The() {
		static SSTV the;
		return the;
	}

	SSTV::SSTV() {
		SetMode("Martin 2");
	}

	void SSTV::SetMode(std::string name) {
		for (int i = 0; i < MODES.size(); i++) {
			if (MODES[i].name != name)
				continue;

			SetMode(&MODES[i]);
			return;
		}

		SetMode(nullptr);
	}

	void SSTV::SetMode(Mode* mode) {
		if (mode == nullptr) {
			this->mode = nullptr;
			return;
		}

		LogInfo("Setting SSTV mode to {}", mode->name);

		this->mode = mode;
		instructions.clear();

		CreateVISHeader();

		for (Instruction ins : mode->instructions_first_line)
			instructions.push_back(ins);

		for (int i = 0; i < mode->lines; i++) {
			for (Instruction ins : mode->instructions_per_line)
				instructions.push_back(ins);
		}
	}

	void SSTV::CreateVISHeader() {
		instructions.push_back({"Leader 1",   300, 1900});
		instructions.push_back({"break",      10,  1200});
		instructions.push_back({"Leader 2",   300, 1900});

		// VIS
		instructions.push_back({"VIS start",  30,  1200});
		bool parity = false;
		for (int i = 0; i < 7; i++) {
			bool bit = mode->vis_code & (1 << i);
			instructions.push_back({"VIS bit " + std::to_string(i), 30, bit ? 1100.f : 1300.f});

			if (bit)
				parity = !parity;
		}
		instructions.push_back({"VIS parity", 30, parity ? 1100.f : 1300.f});
		instructions.push_back({"VIS stop",   30,  1200});
	}

	SDL_Rect CreateLetterbox(SSTV::Mode* mode, SDL_Rect rect) {
		SDL_Rect ret {0, 0, mode->width, mode->lines};

		// return early here if letterboxing would be disabled

		// get scaling factors for dimensions
		float aspect_mode = mode->width / (float)mode->lines;
		float aspect_rect = rect.w / (float)rect.h;

		float scalar = aspect_mode / aspect_rect;

		if (rect.w > rect.h) {
			// for when the width is bigger than the height (ie 16:9)
			ret.h = mode->lines * scalar;
			ret.y = ((mode->lines - ret.h) / 2);
		}
		else {
			// for when the height is bigger than the width (ie 9:16)
			// i still don't know why the math checks out here, but it does
			ret.w = mode->lines * (aspect_mode / scalar);
			ret.x = ((mode->width - ret.w) / 2);
		}

		return ret;
	}

	void SSTV::DoTheThing(SDL_Rect rect) {
		if (mode == nullptr)
			return;

		current_time = 0;
		phase = 0;

		SDL_Rect letterbox = CreateLetterbox(mode, rect);

		// sampling helpers for sampling the screen
		// do it all right now to save time!
		// eventually the "realtime" idea will happen, but let's keep it simple for now
		std::vector<std::uint8_t> pixels;
		pixels.reserve(4 * rect.w * rect.h);
		std::fill(pixels.begin(), pixels.end(), 127);
		glReadPixels(0, 0, rect.w, rect.h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

		cur_x = -1;
		cur_y = -1;

		while(!instructions.empty()) {
			float started_instruction_at = current_time;
			Instruction* ins = &instructions[0];

			float length_ms = ins->length_ms;
			if (ins->flags & InstructionFlags::LengthUsesIndex) {
				length_ms = mode->timings[ins->length_ms];
				//LogDebug("Length from index: {}, {}", ins->length_ms, mode->timings[ins->length_ms]);
			}

			int len_samples = length_ms / (timestep * 1000);

			//LogDebug("New instruction \"{}\" ({} samples)", ins->name, len_samples);

			if (ins->flags & InstructionFlags::NewLine)
				cur_y++;

			/*if (ins->flags & InstructionFlags::PitchUsesIndex) {
				LogDebug("Pitch comes from index: {}, {}", ins->pitch, mode->frequencies[ins->pitch]);
			}
			else if (ins->flags & InstructionFlags::PitchIsDelegated) {
				LogDebug("Pitch would be delegated");
			}
			else {
				LogDebug("Pitch is {}", ins->pitch);
			}*/

			for (int i = 0; i < len_samples; i++) {
				float pitch = ins->pitch;

				if (ins->flags & InstructionFlags::PitchUsesIndex) {
					// take the pitch from an index
					pitch = mode->frequencies[ins->pitch];
				}
				else if (ins->flags & InstructionFlags::PitchIsDelegated) {
					// we're about to do a new scan, delegate it

					float widthfrac = ((float)i / len_samples);
					cur_x = (int)widthfrac;
					if (mode->func_scan_handler != nullptr)
						pitch = mode->func_scan_handler(mode, ins, &pixels[0], rect, letterbox, widthfrac, cur_y);
				}

				// we need to see how the phase will increase for the frequency we want
				// this is where the smooth pitch changes happen
				phase += Oscillator::GetPhaseInc(pitch, samplerate);
				if(phase == INFINITY)
					LogError("Fug");
				else
					while(phase > FTWO_PI * 2)
						phase -= FTWO_PI * 2;

				// add to the list of samples
				samples.push_back(osc.Value(phase));
			}

			// remove this instruction
			instructions.erase(instructions.begin());
		}

		// write to file
		std::ofstream file(core::filesystem::Filesystem::GetDataDir() / "sstv/sstv.f32", std::ios::binary);
		file.write(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(float));
		file.close();

		samples.clear();
	}

	float SSTV::ScanRGB(Mode* mode, Instruction* ins, std::uint8_t* pixels, SDL_Rect rect, SDL_Rect letterbox, float widthfrac, int cur_line) {
		float pitch = 1500.f;
		int pass = (int)ins->pitch;

		int pos_x = mode->width * widthfrac;
		int pos_y = cur_line;

		bool letterbox_sides = letterbox.x > 0 && (pos_x < letterbox.x || pos_x >= letterbox.x + letterbox.w);
		bool letterbox_tops = letterbox.y > 0 && (pos_y < letterbox.y || pos_y >= letterbox.y + letterbox.h);

		if(letterbox_sides || letterbox_tops) {
			bool pattern = ((pos_x + pos_y)/11) % 2;

			if (pass != 2 && pattern)
				pitch += 800.f;
		} else {
			// where we're at along our scanline (remember that height's flipped)
			int sample_x = rect.w * (std::max(pos_x - letterbox.x, 0) / (float)letterbox.w);
			int sample_y = rect.h * (1.f - (std::max(pos_y - letterbox.y, 0) / (float)letterbox.h));

			// get pixel at that sample (no nice filtering...)
			int pixel_idx = sample_x + ((sample_y - 1) * rect.w);
			pixel_idx = std::max(pixel_idx, 0);
			std::uint8_t* pixel = &pixels[pixel_idx * 4];

			// for bytes - (2300-1500 / 255)
			// martin is GBR
			pitch = 1500 + (pixel[pass] * 3.1372549);
		}

		return pitch;
	}

} // namespace engine::core::experiments
