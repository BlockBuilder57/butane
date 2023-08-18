add_library(engine_core
		Assert.cpp
		Logger.cpp
		StdoutSink.cpp

		# SDL abstractions
		sdl/Window.cpp

		# OpenGL abstractions
		gl/Shader.cpp
		)

custengine_setup_target(engine_core)

target_link_libraries(engine_core PUBLIC
		glad::glad
		)


add_library(engine::core ALIAS engine_core)