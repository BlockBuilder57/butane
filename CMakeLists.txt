cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

project(butane
	LANGUAGES C CXX
	)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(Policies)
include(ProjectFuncs)

# required system dependencies
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(PhysFS REQUIRED)

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/third_party/imgui")

add_library(imgui
		"${IMGUI_DIR}/imgui.cpp"
		"${IMGUI_DIR}/imgui_demo.cpp"
		"${IMGUI_DIR}/imgui_draw.cpp"
		"${IMGUI_DIR}/imgui_tables.cpp"
		"${IMGUI_DIR}/imgui_widgets.cpp"

		"${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp"
		"${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
		)

target_link_libraries(imgui SDL2::SDL2 physfs)
target_include_directories(imgui PRIVATE ${IMGUI_DIR})

# default linker
if(NOT BUTANE_LINKER AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(BUTANE_LINKER "lld")
elseif(NOT BUTANE_LINKER)
    set(BUTANE_LINKER "bfd")
endif()

butane_set_alternate_linker()

set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)

add_subdirectory(third_party/glad)
add_subdirectory(third_party/glm)
add_subdirectory(third_party/tomlpp)
add_subdirectory(third_party/magic_enum)
add_subdirectory(third_party/assimp)
add_subdirectory(third_party/fasstv)
add_subdirectory(third_party/murmur3)

add_subdirectory(src/core)
add_subdirectory(src/main)

