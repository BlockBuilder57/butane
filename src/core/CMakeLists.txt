add_library(engine_core
		debug/Assert.cpp
		debug/ImGuiExtensions.cpp

		# various experiments
		experiments/Oscillator.cpp
		experiments/SSTV.cpp

		Logger.cpp
		StdoutSink.cpp

		System.cpp
		InputSystem.cpp
		TimeSystem.cpp

		filesystem/Filesystem.cpp
		filesystem/WatchSystem.cpp
		filesystem/TomlLoader.cpp

		# SDL abstractions
		sdl/Window.cpp
		sdl/File.cpp

		# Rendering abstractions
		rendering/ShaderSystem.cpp
		rendering/Shader.cpp
		rendering/TextureSystem.cpp
		rendering/Texture.cpp
		rendering/Material.cpp
		rendering/MaterialSystem.cpp
		rendering/Mesh.cpp
		rendering/Model.cpp

		# Scene
		scene/Scene.cpp
		scene/Transform.cpp
		scene/Camera.cpp
		)

custengine_setup_target(engine_core)

target_include_directories(engine_core PRIVATE ${IMGUI_DIR})

target_compile_definitions(magic_enum INTERFACE "MAGIC_ENUM_NO_CHECK_REFLECTED_ENUM")

target_link_libraries(engine_core PUBLIC
		glad::glad
		glm::glm
		imgui
		tomlplusplus::tomlplusplus
		magic_enum
		assimp::assimp
		)


add_library(engine::core ALIAS engine_core)