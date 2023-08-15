add_executable(engine
		main.cpp
		)

custengine_setup_target(engine)

target_link_libraries(engine
		engine::core
		SDL2::SDL2
		SDL2::SDL2main
		SDL2_mixer::SDL2_mixer
		)
