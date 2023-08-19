#pragma once

#include <core/System.hpp>
#include <core/filesystem/Filesystem.hpp>
#include <vector>
#include <functional>

namespace engine::core::filesystem {

	struct WatchSystem;

	struct Watch {
		enum class Event : u8 {
			Add = (1<<1),
			Rem = (1<<2),
			Modify = (1<<3)
		};

		///..?
		using EventCallback = std::function<void(const stdfs::path&, Event event)>;

		Watch(const stdfs::path& watchPath);

		Watch(const Watch&) = delete;
		Watch(Watch&&) = delete;

		~Watch();

		void SetCallback(const Watch::EventCallback& callback);

	   protected:
		friend struct WatchSystem;
		void Update();
	   private:
		constexpr static auto IMPL_SIZE = 224; // change if required
		u8 implData__[IMPL_SIZE]{};

		template<class T, class ...Args>
		constexpr void CtorImpl(Args&&... args) {
			static_assert(sizeof(T) <= IMPL_SIZE, "uh oh!!");
			new (&implData__[0]) T(std::forward<Args>(args)...);
		}

		template<class T>
		constexpr T* ToImpl() {
			return reinterpret_cast<T*>(&implData__[0]);
		}


		template<class T>
		constexpr void DtorImpl() {
			reinterpret_cast<T*>(&implData__[0])->~T();
		}

	};


	struct WatchSystem : public core::PerTickSystem {

		// implementation of [core::PerTickSystem] interface
		void Init() override;
		void Shutdown() override;
		void Tick() override;

		void AddWatch(Watch* watch);
		void RemoveWatch(Watch* watch);

	   private:
		std::vector<Watch*> watches;
	};

}