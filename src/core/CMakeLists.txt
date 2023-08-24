add_library(engine_core
		Assert.cpp
		Logger.cpp
		StdoutSink.cpp

		InputSystem.cpp
		System.cpp

		filesystem/Filesystem.cpp
		filesystem/WatchSystem.cpp

		# SDL abstractions
		sdl/Window.cpp
		sdl/File.cpp

		# OpenGL abstractions
		gl/Shader.cpp
		gl/Texture.cpp

		# Scene
		scene/Scene.cpp
		scene/Transform.cpp
		scene/Camera.cpp
		)

custengine_setup_target(engine_core)

target_include_directories(engine_core PRIVATE ${IMGUI_DIR})

target_link_libraries(engine_core PUBLIC
		glad::glad
		glm::glm
		imgui
		)


add_library(engine::core ALIAS engine_core)