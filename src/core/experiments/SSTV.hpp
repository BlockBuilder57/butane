// Created by block on 5/25/24.

#pragma once

#include <array>
#include <core/gl/Texture.hpp>
#include <core/Logger.hpp>
#include <core/StaticVector.hpp>
#include <vector>

#include "Oscillator.hpp"

namespace engine::core::experiments {

	class SSTV {
	   public:
		static SSTV& The();

		enum InstructionFlags : std::uint8_t {
			NewLine = 0b0001,
			LengthUsesIndex = 0b0010,
			PitchUsesIndex = 0b0100,
			PitchIsDelegated = 0b1000
		};

		struct Instruction {
			std::string name;
			float length_ms;
			float pitch;
			InstructionFlags flags;
		};

		struct Mode {
			std::string name;
			std::uint8_t vis_code;
			int width;
			int lines;
			std::vector<float> timings;
			std::vector<int> frequencies;
			float (*func_scan_handler)(Mode* mode, Instruction* ins, std::uint8_t* pixels, SDL_Rect rect, SDL_Rect letterbox, float widthfrac, int cur_line);
			std::vector<Instruction> instructions_first_line;
			std::vector<Instruction> instructions_per_line;
		};

		std::vector<Instruction> MARTIN_INSTRUCTIONS = {
			{ "(1) Sync pulse",      0, 0, (InstructionFlags)(NewLine | LengthUsesIndex | PitchUsesIndex) },
			{ "(2) Sync porch",      1, 1, (InstructionFlags)(LengthUsesIndex | PitchUsesIndex) },
			{ "(3) Green scan",      2, 1, (InstructionFlags)(LengthUsesIndex | PitchIsDelegated) },
			{ "(4) Separator pulse", 1, 1, (InstructionFlags)(LengthUsesIndex | PitchUsesIndex) },
			{ "(5) Blue scan",       2, 2, (InstructionFlags)(LengthUsesIndex | PitchIsDelegated) },
			{ "(6) Separator pulse", 1, 1, (InstructionFlags)(LengthUsesIndex | PitchUsesIndex) },
			{ "(7) Red scan",        2, 0, (InstructionFlags)(LengthUsesIndex | PitchIsDelegated) },
			{ "(8) Separator pulse", 1, 1, (InstructionFlags)(LengthUsesIndex | PitchUsesIndex) }
		};

		std::vector<Instruction> WRASSE_INSTRUCTIONS = {
			{ "(1) Sync pulse", 0, 0, (InstructionFlags)(NewLine | LengthUsesIndex | PitchUsesIndex) },
			{ "(2) Porch",      1, 1, (InstructionFlags)(LengthUsesIndex | PitchUsesIndex) },
			{ "(3) Green scan", 2, 0, (InstructionFlags)(LengthUsesIndex | PitchIsDelegated) },
			{ "(5) Blue scan",  2, 1, (InstructionFlags)(LengthUsesIndex | PitchIsDelegated) },
			{ "(7) Red scan",   2, 2, (InstructionFlags)(LengthUsesIndex | PitchIsDelegated) },
		};

		std::vector<Instruction> PASOKON_INSTRUCTIONS = {
			{ "(1) Sync pulse", 0, 0, (InstructionFlags)(NewLine | LengthUsesIndex | PitchUsesIndex) },
			{ "(2) Porch",      1, 1, (InstructionFlags)(LengthUsesIndex | PitchUsesIndex) },
			{ "(3) Red scan",   2, 0, (InstructionFlags)(LengthUsesIndex | PitchIsDelegated) },
			{ "(4) Porch",      1, 1, (InstructionFlags)(LengthUsesIndex | PitchUsesIndex) },
			{ "(5) Green scan", 2, 1, (InstructionFlags)(LengthUsesIndex | PitchIsDelegated) },
			{ "(6) Porch",      1, 1, (InstructionFlags)(LengthUsesIndex | PitchUsesIndex) },
			{ "(7) Blue scan",  2, 2, (InstructionFlags)(LengthUsesIndex | PitchIsDelegated) },
			{ "(8) Porch",      1, 1, (InstructionFlags)(LengthUsesIndex | PitchUsesIndex) }
		};

		std::vector<Mode> MODES = {
			{ "Martin 1", 44,
			  320, 256,
			  {4.862f, 0.572f, 146.432f}, // pulse, porch, color scan
			  {1200, 1500}, // pulse, porch
			  &SSTV::ScanRGB, {}, MARTIN_INSTRUCTIONS
			},
			{ "Martin 2", 40,
			  320, 256,
			  {4.862f, 0.572f, 73.216f}, // pulse, porch, color scan
			  {1200, 1500}, // sync pulse, porch
			  &SSTV::ScanRGB, {}, MARTIN_INSTRUCTIONS
			},
			{ "Wraase SC2-180", 55,
			  320, 256,
			  {5.5225f, 0.500f, 235.000f}, // pulse, porch, color scan
			  {1200, 1500}, // sync pulse, porch
			  &SSTV::ScanRGB, {}, WRASSE_INSTRUCTIONS
			},
			{ "Pasokon P3", 113,
			  640, 496,
			  {5.208f, 1.042f, 133.333f}, // pulse, porch, color scan
			  {1200, 1500}, // pulse, porch
			  &SSTV::ScanRGB, {}, PASOKON_INSTRUCTIONS
			},
			{ "Pasokon P5", 114,
			  640, 496,
			  {7.813f, 1.563f, 200.000f}, // pulse, porch, color scan
			  {1200, 1500}, // pulse, porch
			  &SSTV::ScanRGB, {}, PASOKON_INSTRUCTIONS
			},
			{ "Pasokon P7", 115,
			  640, 496,
			  {10.417f, 1.042f, 266.666f}, // pulse, porch, color scan
			  {1200, 1500}, // pulse, porch
			  &SSTV::ScanRGB, {}, PASOKON_INSTRUCTIONS
			}
		};

		SSTV();

		void SetMode(std::string name);
		void SetMode(Mode* mode);

		void DoTheThing(SDL_Rect rect);

		static float ScanRGB(Mode* mode, Instruction* ins, std::uint8_t* pixels, SDL_Rect rect, SDL_Rect letterbox, float widthfrac, int cur_line);

	   private:
		const int samplerate = 44100;
		const float timestep = 1.f / samplerate;

		void CreateVISHeader();

		std::vector<Instruction> instructions {};
		std::vector<float> samples {};
		Mode* mode = nullptr;
		float current_time = 0;
		float phase = 0;
		int cur_x = -1;
		int cur_y = -1;
		Oscillator osc { kOsc_Sin };
	};

} // namespace engine::core::experiments
