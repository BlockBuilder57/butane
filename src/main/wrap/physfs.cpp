// Created by block on 2025-07-26.

#include <physfs.h>

namespace butane::wrap::physfs {

	void Init(int argc, char** argv) {
		PHYSFS_init(argv[0]);
	}

	void Shutdown() {
		PHYSFS_deinit();
	}

};
