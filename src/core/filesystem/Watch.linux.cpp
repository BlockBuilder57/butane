
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <core/Assert.hpp>
#include <core/Logger.hpp>

#include <core/filesystem/WatchSystem.hpp>

namespace engine::core::filesystem {

	/// Implementation of [Watch] guts for Linux
	struct WatchImpl {
		explicit WatchImpl(const stdfs::path& path)
			: root(path) {
			iNotifyFd = inotify_init();
			ENGINE_CHECK(iNotifyFd > 0, "Watch failed to initalize properly");
			watchId = inotify_add_watch(iNotifyFd, path.c_str(), IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE | IN_MOVED_FROM | IN_DELETE);
		}

		void Update() {
			FD_SET(iNotifyFd, &set);

			int ret = select(iNotifyFd + 1, &set, nullptr, nullptr, &timeout);
			if(ret < 0) {
				perror("select");
			} else if(FD_ISSET(iNotifyFd, &set)) {
				ssize_t len, i = 0;
				char action[81 + FILENAME_MAX] = { 0 };
				char buff[((sizeof(inotify_event)+FILENAME_MAX)*1024)] = { 0 };

				len = read(iNotifyFd, buff, sizeof(buff));

				while(i < len) {
					auto event = reinterpret_cast<inotify_event*>(&buff[i]);
					HandleActionImpl(event);
					i += sizeof(*event) + event->len;
				}
			}
		}

		void HandleActionImpl(inotify_event* event) const {
			ENGINE_CHECK(eventCallback, "Please assign a event callback before adding this watch");

			// files do not have an event name
			auto path = root;
			if (event->name[0] != '\0')
				path /= event->name;

			if(event->mask & IN_CLOSE_WRITE) {
				eventCallback(path, Watch::Event::Modify);
			}

			if(event->mask & IN_MOVED_TO || event->mask & IN_CREATE) {
				eventCallback(path, Watch::Event::Add);
			}

			if(event->mask & IN_MOVED_FROM || event->mask & IN_DELETE) {
				eventCallback(path, Watch::Event::Rem);
			}
		}

		void SetCallback(const Watch::EventCallback& callback) {
			eventCallback = callback;
		}

		int iNotifyFd {};
		int watchId {};
		struct timeval timeout {};
		fd_set set;
		stdfs::path root;

		Watch::EventCallback eventCallback;
	};
} // namespace engine::core::filesystem