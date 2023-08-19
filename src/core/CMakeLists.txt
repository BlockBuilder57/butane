add_library(engine_core
		Assert.cpp
		Logger.cpp
		StdoutSink.cpp

		filesystem/Filesystem.cpp

		# SDL abstractions
		sdl/Window.cpp
		sdl/File.cpp

		# OpenGL abstractions
		gl/Shader.cpp
		gl/Texture.cpp
		)

custengine_setup_target(engine_core)

target_link_libraries(engine_core PUBLIC
		glad::glad
		)


add_library(engine::core ALIAS engine_core)