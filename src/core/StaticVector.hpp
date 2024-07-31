#pragma once

#include <array>
#include <cstdint>

namespace butane::core {

	template<class Elem, std::size_t Cap>
	struct StaticVector {
		constexpr StaticVector() = default;

		bool push_back(const Elem& obj) {
			if (count == Cap)
				return false;
			elems[count] = obj;
			++count;
			return true;
		}

		bool pop_back() {
			if (count == 0)
				return false;
			--count;
			return true;
		}

		const auto& front() const { return elems[0]; }
		auto& front() { return elems[0]; }

		const auto& back() const { return elems[count - 1]; }
		auto& back() { return elems[count - 1]; }


		constexpr bool empty() const {
			return count == 0;
		}

		void reset() {
			count = 0;
		}

	   private:
		std::array<Elem, Cap> elems{};
		std::size_t count;
	};
}