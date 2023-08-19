#include <core/filesystem/WatchSystem.hpp>

namespace engine::core::filesystem {

	/// Implementation of [Watch] guts for no platform.
	struct WatchImpl {
		explicit WatchImpl(const stdfs::path& path)
			: root(path) {
		}

		void Update() {

		}

		void SetCallback(const Watch::EventCallback& callback) {
		}

		stdfs::path root;
		Watch::EventCallback eventCallback;
	};
} // namespace engine::core::filesystem