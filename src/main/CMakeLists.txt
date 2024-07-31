add_executable(butane
		main.cpp
		)

butane_setup_target(butane)

target_include_directories(butane PRIVATE ${IMGUI_DIR})

target_link_libraries(butane
		butane::core
		SDL2::SDL2
		SDL2::SDL2main
		SDL2_mixer::SDL2_mixer
		SDL2_image::SDL2_image
		imgui
		)
